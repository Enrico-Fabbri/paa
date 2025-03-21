#include "Game/PlayerPawn.h"
#include "Game/StrategyGameMode.h"
#include "Game/Controllers/GameAIController.h"
#include "Game/Controllers/GamePlayerController.h"
#include "Game/Managers/BattleManager.h"
#include "Game/Managers/PlacementManager.h"
#include "Game/Managers/UIManager.h"
#include "Kismet/GameplayStatics.h"

AStrategyGameMode::AStrategyGameMode()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	DefaultPawnClass = APlayerPawn::StaticClass();
	PlayerControllerClass = AGamePlayerController::StaticClass();
}

void AStrategyGameMode::TransitionToPhase(EGamePhase NewPhase)
{
	CurrentPhase = NewPhase;

	// Broadcast the phase change to any listeners.
	OnGamePhaseChanged.Broadcast(CurrentPhase);

	// Handle phase-specific logic.
	switch (CurrentPhase)
	{
	case EGamePhase::Begin:
		GridManager->GenerateGrid();
		break;
	case EGamePhase::CoinFlip:
		break;
	case EGamePhase::Placement:
		GridManager->GenerateObstacles();
		break;
	case EGamePhase::Battle:
		break;
	case EGamePhase::End:
		BattleManager->ResetUnits();
		PlacementManager->ResetUnits();
		break;
	}
}

void AStrategyGameMode::SwitchTurn()
{
	bIsPlayerTurn = !bIsPlayerTurn;
	
	// Broadcast the turn switch to any listeners.
	OnSwitchTurn.Broadcast(bIsPlayerTurn);
}

void AStrategyGameMode::SetTurn(bool bPlayerTurn)
{
	bIsPlayerTurn = bPlayerTurn;
	
	// Broadcast the turn change to any listeners.
	OnSwitchTurn.Broadcast(bIsPlayerTurn);
}

UWorld* AStrategyGameMode::World() const
{
	return GetWorld();
}

bool AStrategyGameMode::IsPlayerTurn() const
{
	return bIsPlayerTurn;
}

TWeakObjectPtr<AGamePlayerController> AStrategyGameMode::GetPlayerController() const
{
	return PlayerController;
}

AGridManager* AStrategyGameMode::GetGridManager()
{
	return GridManager.Get();
}

UPlacementManager* AStrategyGameMode::GetPlacementManager()
{
	return PlacementManager;
}

UBattleManager* AStrategyGameMode::GetBattleManager()
{
	return BattleManager;
}

void AStrategyGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the grid manager and attach it to the game mode.
	GridManager = GetWorld()->SpawnActor<AGridManager>();
	GridManager->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	GridManager->Initialize(this);

	// Initialize the player controller and attach it to the game mode.
	PlayerController = Cast<AGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PlayerController->InitPlayerController(this);
	PlayerController->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	// Initialize the AI controller.
	AIController = NewObject<UGameAIController>(this);
	AIController->Initialize(this);
	
	// Initialize the placement manager.
	PlacementManager = NewObject<UPlacementManager>(this);
	PlacementManager->Initialize(this);

	// Initialize the battle manager.
	BattleManager = NewObject<UBattleManager>(this);
	BattleManager->Initialize(this);

	// Initialize the UI manager.
	UIManager = NewObject<UUIManager>(this);
	UIManager->Initialize(this);

	// Initialize the player pawn and position it on the grid.
	PlayerPawn = Cast<APlayerPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	FVector Location = GridManager->GetGridCenter(); Location.Y -= 50.f; Location.X -= 50.f;
	PlayerPawn->SetActorLocation(Location);
	PlayerPawn->SetActorRotation(FRotator(-90, -90, 0));
	PlayerPawn->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	
	// Transition to the initial game phase.
	TransitionToPhase(EGamePhase::Begin);
}