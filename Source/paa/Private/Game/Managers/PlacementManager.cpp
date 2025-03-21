#include "Game/Managers/PlacementManager.h"

#include "Game/Controllers/GamePlayerController.h"
#include "Game/Managers/BattleManager.h"
#include "Units/BrawlerUnit.h"
#include "Units/SniperUnit.h"

UPlacementManager::UPlacementManager()
{
	// Initialize unit placement counts for player and AI.
	PlayerUnitsPlaced = { {EUnitTypes::Brawler, 1}, {EUnitTypes::Sniper, 1} };
	AIUnitsPlaced = { {EUnitTypes::Brawler, 1}, {EUnitTypes::Sniper, 1} };

	RemainingTurns = 4; // Set the number of placement turns.
}

void UPlacementManager::Initialize(AStrategyGameMode* GameModeRef)
{
	GameMode = GameModeRef;

	if (GameMode.IsValid())
	{
		// Bind to input and game phase events.
		GameMode->GetPlayerController()->OnPlacementClick.RemoveDynamic(this, &UPlacementManager::SetUnitLocation);
		GameMode->GetPlayerController()->OnPlacementClick.AddDynamic(this, &UPlacementManager::SetUnitLocation);

		GameMode->OnGamePhaseChanged.RemoveDynamic(this, &UPlacementManager::OnGamePhaseChanged);
		GameMode->OnGamePhaseChanged.AddDynamic(this, &UPlacementManager::OnGamePhaseChanged);

		GameMode->OnUnitsColorSet.RemoveDynamic(this, &UPlacementManager::SetUnitsColor);
		GameMode->OnUnitsColorSet.AddDynamic(this, &UPlacementManager::SetUnitsColor);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to register delegates for PlacementManager"));
	}
}

void UPlacementManager::PlaceUnit(bool bIsPlayer)
{
	if (SelectedUnitType == EUnitTypes::None || CurrentGamePhase != EGamePhase::Placement) return;
	
	ABaseUnit* Unit;
	
	if (bIsPlayer)
	{		
		if (PlayerUnitsPlaced[SelectedUnitType] == 0) return;

		if (SelectedUnitLocation == FVector(-1, -1, -1)) return;

		if (const FString Location = GameMode->GetGridManager()->WorldToGrid(SelectedUnitLocation); GameMode->GetGridManager()->GetTile(Location)->IsObstacle()) return;
		
		if (SelectedUnitType == EUnitTypes::Brawler)
		{
			Unit = GameMode->World()->SpawnActor<ABrawlerUnit>(ABrawlerUnit::StaticClass(), SelectedUnitLocation, FRotator::ZeroRotator);
		}
		else
		{
			Unit = GameMode->World()->SpawnActor<ASniperUnit>(ASniperUnit::StaticClass(), SelectedUnitLocation, FRotator::ZeroRotator);
		}

		GameMode->GetBattleManager()->AddPlayerUnit(Unit);

		Unit->SetTextureColor(UnitsColor);	
		
		PlayerUnitsPlaced[SelectedUnitType]--;
		OnUnitPlaced.Broadcast(SelectedUnitType);
	}
	else
	{
		if (AIUnitsPlaced[SelectedUnitType] == 0) return;
		
		if (SelectedUnitLocation == FVector(-1, -1, -1)) return;
		
		if (SelectedUnitType == EUnitTypes::Brawler)
		{
			Unit = GameMode->World()->SpawnActor<ABrawlerUnit>(ABrawlerUnit::StaticClass(), SelectedUnitLocation, FRotator(0.0, -180.0, 0.0));
		}
		else
		{
			Unit = GameMode->World()->SpawnActor<ASniperUnit>(ASniperUnit::StaticClass(), SelectedUnitLocation, FRotator(0.0, -180.0, 0.0));
		}

		GameMode->GetBattleManager()->AddAIUnit(Unit);

		Unit->SetTextureColor(UnitsColor < 4 ? UnitsColor + 1 : 0);	
		
		AIUnitsPlaced[SelectedUnitType]--;
	}
	
	Unit->SetUnitPosition(GameMode->GetGridManager()->WorldToGrid(SelectedUnitLocation));
	
	SelectedUnitLocation = FVector(-1, -1, -1);
	SelectedUnitType = EUnitTypes::None;
	RemainingTurns -= 1;
	
	if (RemainingTurns == 0 && CurrentGamePhase != EGamePhase::Battle) GameMode->TransitionToPhase(EGamePhase::Battle);

	GameMode->SwitchTurn();
}

void UPlacementManager::SetUnitToPlace(EUnitTypes UnitType)
{
	SelectedUnitType = UnitType;
	
	PlaceUnit(GameMode->IsPlayerTurn());
}

void UPlacementManager::SetUnitLocation(FVector Location)
{
	SelectedUnitLocation = Location;
	SelectedUnitLocation.Z = 1;
	
	PlaceUnit(GameMode->IsPlayerTurn());
}

void UPlacementManager::SetUnitsColor(int32 Color)
{
	UnitsColor = Color;
}

int32 UPlacementManager::RemainingUnitsToPlace(const EUnitTypes UnitType, const bool bIsPlayer) const
{
	if (UnitType == EUnitTypes::None) return 0;

	return bIsPlayer ? PlayerUnitsPlaced[UnitType] : AIUnitsPlaced[UnitType];
}

void UPlacementManager::ResetUnits()
{
	// Reset unit placement counts.
	PlayerUnitsPlaced = { {EUnitTypes::Brawler, 1}, {EUnitTypes::Sniper, 1} };
	AIUnitsPlaced = { {EUnitTypes::Brawler, 1}, {EUnitTypes::Sniper, 1} };

	RemainingTurns = 4; // Reset the number of placement turns.
}

void UPlacementManager::OnGamePhaseChanged(EGamePhase NewPhase)
{
	CurrentGamePhase = NewPhase; // Update the current game phase.
}