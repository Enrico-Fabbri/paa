#include "Game/Managers/BattleManager.h"

#include "Game/Controllers/GamePlayerController.h"
#include "Systems/DamageSystem.h"
#include "Systems/MovementSystem.h"

void UBattleManager::Initialize(AStrategyGameMode* GameModeRef)
{
    GameMode = GameModeRef;

    if (GameMode.IsValid())
    {
    	// Unbind existing delegates to avoid duplicate bindings.
    	GameMode->OnGamePhaseChanged.RemoveDynamic(this, &UBattleManager::OnGamePhaseChanged);
    	GameMode->OnSwitchTurn.RemoveDynamic(this, &UBattleManager::OnSwitchTurn);

    	GameMode->GetPlayerController()->OnUnitClicked.RemoveDynamic(this, &UBattleManager::UnitSelected);
    	GameMode->GetPlayerController()->OnTileClicked.RemoveDynamic(this, &UBattleManager::TileSelected);

    	// Bind game events.
        GameMode->OnGamePhaseChanged.AddDynamic(this, &UBattleManager::OnGamePhaseChanged);
        GameMode->OnSwitchTurn.AddDynamic(this, &UBattleManager::OnSwitchTurn);
        
        // Bind input events.
        GameMode->GetPlayerController()->OnUnitClicked.AddDynamic(this, &UBattleManager::UnitSelected);
        GameMode->GetPlayerController()->OnTileClicked.AddDynamic(this, &UBattleManager::TileSelected);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize BattleManager - Invalid GameMode"));
    }
}

void UBattleManager::OnTurnSkipped()
{
	auto& CurrentUnits = bIsPlayerTurn ? PlayerUnits : AIUnits;

	// Reset actions for all units in the current turn.
	for (auto& Tuple : CurrentUnits)
	{
		Tuple.Value = EActionType::None;
	}

	OnCanSkipTurn.Broadcast(false); // Notify that the turn cannot be skipped anymore.

	ClearSelection(GameMode->GetGridManager()); // Clear the current selection.
	
	GameMode->SwitchTurn(); // Switch to the next turn.
}

void UBattleManager::OnGamePhaseChanged(EGamePhase NewPhase)
{
    CurrentGamePhase = NewPhase; // Update the current game phase.
}

void UBattleManager::OnSwitchTurn(const bool NewBIsPlayerTurn)
{
    bIsPlayerTurn = NewBIsPlayerTurn; // Update the turn state.
}

void UBattleManager::ResetUnits()
{
	// Destroy all player units.
	for (auto Unit : PlayerUnits)
	{
		Unit.Key->Destroy();
	}

	// Destroy all AI units.
	for (auto AIUnit : AIUnits)
	{
		AIUnit.Key->Destroy();
	}
	
	PlayerUnits.Empty(); // Clear the player units list.
	AIUnits.Empty(); // Clear the AI units list.
}

void UBattleManager::AddPlayerUnit(ABaseUnit* Unit)
{
    PlayerUnits.Add(Unit, EActionType::None); // Add the unit to the player's list with no action.
}

void UBattleManager::AddAIUnit(ABaseUnit* Unit)
{
    AIUnits.Add(Unit, EActionType::None); // Add the unit to the AI's list with no action.
}

TArray<FString> UBattleManager::GetOccupied() const
{
    TArray<FString> OccupiedTiles;
	
    // Combine positions from all player and AI units.
    for (const auto& Tuple : PlayerUnits) OccupiedTiles.Add(Tuple.Key->GetPosition());
    for (const auto& Tuple : AIUnits) OccupiedTiles.Add(Tuple.Key->GetPosition());
    
    return OccupiedTiles; // Return the list of occupied tiles.
}

void UBattleManager::UnitSelected(ABaseUnit* Unit, bool bIsLeftClick)
{
    const EClickType ReceivedClick = bIsLeftClick ? EClickType::Left : EClickType::Right;
    const TWeakObjectPtr<AGridManager> GridManager = GameMode->GetGridManager();
	
    // Selection Logic
    if (ShouldSelectNewUnit(Unit, ReceivedClick))
    {
    	// Clear previous highlights.
    	GridManager->ColorTiles(ColoredTiles, FLinearColor::White);
    	ColoredTiles.Empty();
    	
        HandleNewSelection(Unit, bIsLeftClick, ReceivedClick, GridManager); // Handle the new selection.
    }
    else if (IsDeselectionScenario(Unit))
    {
        ClearSelection(GridManager); // Clear the current selection.
    }
    else if (IsAttackScenario(Unit, ReceivedClick))
    {
        AttackUnit(Unit); // Attack the selected unit.
        ClearSelection(GridManager); // Clear the selection after attacking.
    }
}

void UBattleManager::TileSelected(ATile* Tile, bool bIsLeftClick)
{
	if (!Tile) return;
	
	const FString GridPosition = GameMode->GetGridManager()->WorldToGrid(Tile->GetActorLocation());

	// Validate the selection.
	if (!SelectedUnit.Get() || bIsPlayerTurn != PlayerUnits.Contains(SelectedUnit) ||
		!bIsLeftClick || ClickType != EClickType::Left || !ColoredTiles.Contains(GridPosition)) return;

	if (PlayerUnits.Contains(SelectedUnit))
	{
		if (PlayerUnits[SelectedUnit] != EActionType::None)
			return;
	}
	else
	{
		if (AIUnits[SelectedUnit] != EActionType::None)
			return;
	}
	
    MoveUnit(GridPosition); // Move the selected unit to the tile.

    ClearSelection(GameMode->GetGridManager()); // Clear the selection after moving.
}

void UBattleManager::AttackUnit(ABaseUnit* Unit)
{
    const FString StartingTile = SelectedUnit->GetPosition();
    const TPair<int32, int32> DamageValues = UDamageSystem::ApplyDamage(SelectedUnit, Unit);
	
    FormatAction(DamageValues.Key, StartingTile, "", Unit, DamageValues.Value); // Format and broadcast the attack action.

	auto& CurrentUnits = bIsPlayerTurn ? PlayerUnits : AIUnits;
	
	// Update the unit's action type.
	switch (CurrentUnits[SelectedUnit])
	{
	case EActionType::None:
		CurrentUnits[SelectedUnit] = EActionType::Attack;
		break;
	case EActionType::Move:
		CurrentUnits[SelectedUnit] = EActionType::MoveAndAttack;
		break;
	default:
		break;
	}

	HandleUnitDeath(SelectedUnit.Get(), Unit); // Handle unit death if applicable.

	CheckCanSkipTurn(); // Check if the player can skip their turn.
	CheckEndConditions(); // Check if the game has ended.
}

void UBattleManager::MoveUnit(const FString& GridPosition)
{
	const FString OriginalPosition = SelectedUnit->GetPosition();
	
	UMovementSystem::ApplyMovement(SelectedUnit, GridPosition, GetOccupied()); // Move the unit.
	FormatAction(-1, OriginalPosition, GridPosition, nullptr, -1); // Format and broadcast the move action.

	auto& CurrentUnits = bIsPlayerTurn ? PlayerUnits : AIUnits;
	
	// Update the unit's action type.
	switch (CurrentUnits[SelectedUnit])
	{
	case EActionType::None:
		CurrentUnits[SelectedUnit] = EActionType::Move;
		break;
	default:
		break;
	}

	CheckCanSkipTurn(); // Check if the player can skip their turn.
}

void UBattleManager::CheckCanSkipTurn() const
{
	auto& CurrentUnits = bIsPlayerTurn ? PlayerUnits : AIUnits;

	// Check if all units have performed an action.
	for (const auto& Tuple : CurrentUnits)
	{
		if (Tuple.Value == EActionType::None) return;
	}

	OnCanSkipTurn.Broadcast(true); // Notify that the turn can be skipped.
}

void UBattleManager::CheckEndConditions() const
{
	OnCanEnd.Broadcast(AIUnits.IsEmpty() || PlayerUnits.IsEmpty() , AIUnits.IsEmpty()); // Notify if the game has ended.
	OnCanSkipTurn.Broadcast(true); // Enable End Game button
}

void UBattleManager::FormatAction(const int32 Damage, const FString& StartingTile, 
                                 const FString& EndTile, ABaseUnit* Unit, const int32 DamageCounter) const
{
	FString Text = "";
	
	if (SelectedUnit.Get())
	{
		bIsPlayerTurn ? Text += "HP: " : Text += "AI: ";
		Cast<ABrawlerUnit>(SelectedUnit) ? Text += "B "	: Text += "S ";
		
		if (Damage > 0) Text += Unit->GetPosition() + " " + FString::FromInt(Damage); // Append damage if attacking.
		else Text += StartingTile + " -> " + EndTile; // Append movement if moving.	
	}
	else
	{
		Text += "ERROR: SELECTED UNIT ISN'T VALID";
	}

	OnActionExecuted.Broadcast(Text); // Broadcast the formatted action.
	
	if (Unit && DamageCounter > 0)
	{
		Text = "";

		bIsPlayerTurn ? Text += "HP: " : Text += "AI: ";
		Cast<ABrawlerUnit>(SelectedUnit) ? Text += "B "	: Text += "S ";
		Text += StartingTile + " GOT COUNTERED BY ";
		Cast<ABrawlerUnit>(Unit) ? Text += "B "	: Text += "S ";
		Text += Unit->GetPosition() + " ";
		Text += FString::FromInt(DamageCounter);

		OnActionExecuted.Broadcast(Text); // Broadcast the counter-attack action.
	}
}

TMap<TWeakObjectPtr<ABaseUnit>, EActionType> UBattleManager::GetAIUnits() const
{
	return AIUnits; // Return the AI's units.
}

TMap<TWeakObjectPtr<ABaseUnit>, EActionType> UBattleManager::GetPlayerUnits() const
{
	return PlayerUnits; // Return the player's units.
}

TArray<FString> UBattleManager::GetColored() const
{
	return ColoredTiles; // Return the currently highlighted tiles.
}

bool UBattleManager::ShouldSelectNewUnit(const ABaseUnit* Unit, const EClickType Click) const
{
    return (!SelectedUnit.Get() && Unit) || 
           (SelectedUnit != Unit && (Click == ClickType && Click == EClickType::Left)); // Determine if a new unit should be selected.
}

void UBattleManager::HandleNewSelection(ABaseUnit* Unit, bool bIsLeftClick, 
                                       EClickType Click, TWeakObjectPtr<AGridManager> GridManager)
{
    SelectedUnit = Unit;
    OnUnitSelected.Broadcast(SelectedUnit.Get(), PlayerUnits.Contains(SelectedUnit)); // Notify that a unit has been selected.

    // Determine range and color based on click type.
    const int32 Range = bIsLeftClick ? SelectedUnit->GetMovementRange() : SelectedUnit->GetAttackRange();
    const FLinearColor Color = bIsLeftClick ? FLinearColor::Green : FLinearColor::Red;
    
    ColoredTiles = GridManager->FindArea(SelectedUnit->GetPosition(), Range, bIsLeftClick, GetOccupied()); // Find and highlight tiles.
    GridManager->ColorTiles(ColoredTiles, Color); // Color the tiles.
    
    ClickType = Click; // Update the click type.
}

bool UBattleManager::IsDeselectionScenario(const ABaseUnit* Unit) const
{
	return SelectedUnit == Unit; // Determine if the current selection should be deselected.
}

void UBattleManager::ClearSelection(TWeakObjectPtr<AGridManager> GridManager)
{
    SelectedUnit = nullptr;
    OnUnitSelected.Broadcast(nullptr, false); // Notify that the selection has been cleared.
    GridManager->ColorTiles(ColoredTiles, FLinearColor::White); // Reset tile colors.
    ColoredTiles.Empty(); // Clear the highlighted tiles.
    ClickType = EClickType::Null; // Reset the click type.
}

bool UBattleManager::IsAttackScenario(const ABaseUnit* Unit, const EClickType Click) const
{
	if (PlayerUnits.Contains(SelectedUnit))
	{
		if (PlayerUnits[SelectedUnit] != EActionType::None &&
			PlayerUnits[SelectedUnit] != EActionType::Move)
			return false;
	}
	else
	{
		if (AIUnits[SelectedUnit] != EActionType::None &&
			AIUnits[SelectedUnit] != EActionType::Move)
			return false;
	}

	return SelectedUnit.Get() && Unit && ClickType == Click && Click == EClickType::Right &&
		ColoredTiles.Contains(Unit->GetPosition()) &&
			PlayerUnits.Contains(Unit) != PlayerUnits.Contains(SelectedUnit) &&
				bIsPlayerTurn == PlayerUnits.Contains(SelectedUnit); // Determine if an attack should occur.
}

void UBattleManager::HandleUnitDeath(ABaseUnit* Attacker, ABaseUnit* Target)
{
    // Check and remove dead units.
    TArray UnitsToCheck = { Attacker, Target };
    for (ABaseUnit* Unit : UnitsToCheck)
    {
        if (Unit && Unit->IsDead())
        {
            (PlayerUnits.Contains(Unit) ? PlayerUnits : AIUnits).Remove(Unit); // Remove the unit from the list.
            Unit->Destroy(); // Destroy the unit.
        }
    }
}