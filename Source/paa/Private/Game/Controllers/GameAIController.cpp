#include "Game/Controllers/GameAIController.h"

#include "Game/Controllers/GamePlayerController.h"
#include "Game/Managers/BattleManager.h"
#include "Game/Managers/PlacementManager.h"

void UGameAIController::Initialize(AStrategyGameMode* NewGameMode)
{
	GameMode = NewGameMode;
	
	if (GameMode.IsValid())
	{
		GameMode->OnGamePhaseChanged.RemoveDynamic(this, &UGameAIController::OnPhaseChanged);
		GameMode->OnSwitchTurn.RemoveDynamic(this, &UGameAIController::OnSwitchTurn);
		
		GameMode->OnGamePhaseChanged.AddDynamic(this, &UGameAIController::OnPhaseChanged);
		GameMode->OnSwitchTurn.AddDynamic(this, &UGameAIController::OnSwitchTurn);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to register delegates for GameAIController"));
	}
}

void UGameAIController::OnPhaseChanged(EGamePhase NewPhase)
{
	CurrentPhase = NewPhase;
}

void UGameAIController::OnSwitchTurn(bool bIsPlayerTurn)
{
	if (bIsPlayerTurn) return;
	
	FTimerHandle TimerHandle;
	
	switch (CurrentPhase)
	{
	case EGamePhase::Placement:
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UGameAIController::HandlePlacementPhase, 0.35f, false);
		break;
	case EGamePhase::Battle:
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UGameAIController::HandleBattlePhase, 0.35f, false);
		break;
	case EGamePhase::CoinFlip:
	case EGamePhase::Begin:
	case EGamePhase::End:
		break;
	}
}

void UGameAIController::HandlePlacementPhase()
{
	UPlacementManager* PlacementManager = GameMode->GetPlacementManager();
	const UBattleManager* BattleManager = GameMode->GetBattleManager();
	const AGridManager* GridManager = GameMode->GetGridManager();

	FVector Location;
	
	const int32 X = GridManager->GetGridSizeX();
	const int32 Y = GridManager->GetGridSizeY();
	const float S = GridManager->GetTileSize();

	const TArray<FString> Occupied = BattleManager->GetOccupied();
	
	while (true)
	{
		Location = FVector(FMath::RandRange(1, X) * S, FMath::RandRange(1, Y) * S, 0);
		FString TileName = GridManager->WorldToGrid(Location);

		//UE_LOG(LogTemp, Display, TEXT("%s"), *TileName)
		
		const ATile* Tile = GridManager->GetTile(TileName).Get();

		if (!Tile) UE_LOG(LogTemp, Error, TEXT("NO TILE WITH SUCH LOCATION"));
		
		if (Tile && !Tile->IsObstacle() && !Occupied.Contains(GridManager->WorldToGrid(Location))) break;
	}

	//UE_LOG(LogTemp, Display, TEXT("%s"), *Location.ToString());
	
	PlacementManager->SetUnitLocation(Location);
	PlacementManager->SetUnitToPlace(PlacementManager->RemainingUnitsToPlace(EUnitTypes::Brawler, false) > 0 ? EUnitTypes::Brawler : EUnitTypes::Sniper);
}

void UGameAIController::HandleBattlePhase()
{
	UBattleManager* BattleManager = GameMode->GetBattleManager();
	
	// Retrieve AI and Player units


	TMap<TWeakObjectPtr<ABaseUnit>, EActionType> PlayerUnitsMap = BattleManager->GetPlayerUnits();
	TArray<TWeakObjectPtr<ABaseUnit>> PlayerUnits;
	PlayerUnitsMap.GetKeys(PlayerUnits);

	BattleState = EBattleState::Movement;
	CurrentUnitIndex = 0;
	
	ProcessNextAction();	
}

void UGameAIController::ProcessNextAction()
{
	UBattleManager* BattleManager = GameMode->GetBattleManager();
	AGridManager* GridManager = GameMode->GetGridManager();
	
	switch (BattleState)
	{
	case EBattleState::Idle:
		GameMode->GetBattleManager()->OnTurnSkipped();
		break;
	case EBattleState::Attack:
		ProcessAttack(BattleManager, GridManager);
		break;
	case EBattleState::Movement:
		ProcessMovement(BattleManager, GridManager);
		break;
	}
}

void UGameAIController::ProcessMovement(UBattleManager* BattleManager, AGridManager* GridManager)
{
	TMap<TWeakObjectPtr<ABaseUnit>, EActionType> AIUnitsMap = BattleManager->GetAIUnits();
	TArray<TWeakObjectPtr<ABaseUnit>> AIUnits;
	AIUnitsMap.GetKeys(AIUnits);
	
	if (CurrentUnitIndex >= AIUnits.Num())
	{
		// All units processed for movement, transition to Attack state
		BattleState = EBattleState::Attack;
		CurrentUnitIndex = 0;
		ProcessNextAction();
		return;
	}

	ABaseUnit* AIUnit = AIUnits[CurrentUnitIndex].Get();
	if (AIUnit)
	{
		CurrentUnitIndex++;
		TryMove(BattleManager, GridManager, AIUnit);
	}
}

void UGameAIController::TryMove(const UBattleManager* BattleManager, const AGridManager* GridManager, ABaseUnit* AIUnit)
{
	if (!BattleManager || !GridManager) 
	{
		ProcessNextAction();
		return;
	}
	
	TMap<TWeakObjectPtr<ABaseUnit>, EActionType> PlayerUnitsMap = BattleManager->GetPlayerUnits();
	TArray<TWeakObjectPtr<ABaseUnit>> PlayerUnits;
	PlayerUnitsMap.GetKeys(PlayerUnits);
	
	const TArray<FString> MovementTiles = GridManager->FindArea(AIUnit->GetPosition(), AIUnit->GetMovementRange(), true, BattleManager->GetOccupied());
	const ABaseUnit* NearestPlayerUnit = FindNearestPlayerUnit(AIUnit, PlayerUnits, GridManager);
	const FString BestMovementTile = FindBestMovementTile(AIUnit, MovementTiles, NearestPlayerUnit, GameMode.Get());

	// If no valid movement tile is found, proceed to the next action
	if (BestMovementTile.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid movement tile found for AI unit. Skipping move."));
		ProcessNextAction();
		return;
	}
	
	if (ATile* Tile = GridManager->GetTile(BestMovementTile).Get())
	{
		if (BestMovementTile != AIUnit->GetPosition())
		{
			// Broadcast the first event immediately
			GameMode->GetPlayerController()->OnUnitClicked.Broadcast(AIUnit, true);

			// Set a timer to broadcast the second event after 0.4 seconds
			FTimerHandle TileClickTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TileClickTimerHandle,
				[this, Tile]()
				{
					GameMode->GetPlayerController()->OnTileClicked.Broadcast(Tile, true);

					// Set a timer to process the next action after 0.2 seconds
					FTimerHandle NextActionTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(
						NextActionTimerHandle,
						this,
						&UGameAIController::ProcessNextAction,
						0.2f,
						false
					);
				},
				0.4f,
				false
			);
		}
		else
		{
			// If the best tile is the current tile, skip moving and proceed to the next action
			ProcessNextAction();
		}
	}
	else
	{
		// If the tile is invalid, proceed to the next action
		ProcessNextAction();
	}
}

void UGameAIController::ProcessAttack(UBattleManager* BattleManager, AGridManager* GridManager)
{
	TMap<TWeakObjectPtr<ABaseUnit>, EActionType> AIUnitsMap = BattleManager->GetAIUnits();
	TArray<TWeakObjectPtr<ABaseUnit>> AIUnits;
	AIUnitsMap.GetKeys(AIUnits);
	
	if (CurrentUnitIndex >= AIUnits.Num())
	{
		// All attacks processed, transition to Idle
		BattleState = EBattleState::Idle;
		CurrentUnitIndex = 0;
		ProcessNextAction();
		return;
	}

	ABaseUnit* AIUnit = AIUnits[CurrentUnitIndex].Get();
	if (AIUnit)
	{
		CurrentUnitIndex++;
		TryAttack(BattleManager, GridManager, AIUnit);
	}
}

void UGameAIController::TryAttack(const UBattleManager* BattleManager, const AGridManager* GridManager, ABaseUnit* AIUnit)
{
	if (!BattleManager || !GridManager) 
	{
		ProcessNextAction();
		return;
	}

	TMap<TWeakObjectPtr<ABaseUnit>, EActionType> PlayerUnitsMap = BattleManager->GetPlayerUnits();
	TArray<TWeakObjectPtr<ABaseUnit>> PlayerUnits;
	PlayerUnitsMap.GetKeys(PlayerUnits);
	
	const TArray<FString> AttackTiles = GridManager->FindArea(AIUnit->GetPosition(), AIUnit->GetAttackRange(), false, BattleManager->GetOccupied());
	TArray<ABaseUnit*> Targets;
	
	for (TWeakObjectPtr PlayerUnitPtr : PlayerUnits)
	{
		ABaseUnit* PlayerUnit = PlayerUnitPtr.Get();
		
		if (PlayerUnit && AttackTiles.Contains(PlayerUnit->GetPosition()))
		{
			Targets.Add(PlayerUnit);
		}
	}

	// If no valid target is found, proceed to the next action
	if (Targets.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid attack target found for AI unit. Skipping attack."));
		ProcessNextAction();
		return;
	}

	// Broadcast the first event immediately
	GameMode->GetPlayerController()->OnUnitClicked.Broadcast(AIUnit, false);

	// Set a timer to broadcast the second event after 0.4 seconds
	FTimerHandle AttackTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		AttackTimerHandle,
		[this, Targets]()
		{
			ABaseUnit* Target = Targets[0];
			GameMode->GetPlayerController()->OnUnitClicked.Broadcast(Target, false);

			// Set a timer to process the next action after 0.2 seconds
			FTimerHandle NextActionTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				NextActionTimerHandle,
				this,
				&UGameAIController::ProcessNextAction,
				0.2f,
				false
			);
		},
		0.4f,
		false
	);
}

ABaseUnit* UGameAIController::FindNearestPlayerUnit(const ABaseUnit* AIUnit, const TArray<TWeakObjectPtr<ABaseUnit>>& PlayerUnits, const AGridManager* GridManager)
{
	if (!AIUnit || !GridManager) return nullptr;

	const FVector AIWorldPos = GridManager->GridToWorld(AIUnit->GetPosition());
	
	float MinDistance = FLT_MAX;
	ABaseUnit* NearestPlayer = nullptr;

	for (TWeakObjectPtr PlayerUnitPtr : PlayerUnits)
	{
		ABaseUnit* PlayerUnit = PlayerUnitPtr.Get();
		
		if (PlayerUnit)
		{
			float Distance = FVector::Distance(AIWorldPos, GridManager->GridToWorld(PlayerUnit->GetPosition()));

			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				NearestPlayer = PlayerUnit;
			}
		}
	}

	return NearestPlayer;
}

FString UGameAIController::FindBestMovementTile(const ABaseUnit* AIUnit, const TArray<FString>& MovementTiles, const ABaseUnit* TargetPlayer, AStrategyGameMode* GameMode)
{
	if (!TargetPlayer || !GameMode || !GameMode->GetGridManager()) return FString();

	// Get the grid manager and the positions of the AI unit and the target player
	AGridManager* GridManager = GameMode->GetGridManager();
	FString StartTile = AIUnit->GetPosition();
	FString TargetTile = TargetPlayer->GetPosition();

	// Get the list of occupied tiles (obstacles)
	TArray<FString> OccupiedTiles = GameMode->GetBattleManager()->GetOccupied();
	OccupiedTiles.Remove(TargetTile);
	OccupiedTiles.Remove(StartTile);
	
	// Use A* to find the path from the AI unit to the target player
	TArray<FString> Path = GridManager->FindPath(StartTile, TargetTile, OccupiedTiles);

	// Find the latest tile in the path that is within the AI unit's movement range AND not occupied
	for (int32 i = Path.Num() - 1; i >= 0; --i)
	{
		const FString& Tile = Path[i];

		// Check if the tile is in the movement range and not occupied
		if (MovementTiles.Contains(Tile) && !OccupiedTiles.Contains(Tile))
		{
			return Tile; // Return the best tile
		}
	}

	// If no valid tile is found, return an empty string
	return FString();
}
