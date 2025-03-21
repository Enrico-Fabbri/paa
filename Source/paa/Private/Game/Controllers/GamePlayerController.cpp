#include "Game/Controllers/GamePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "IContentBrowserSingleton.h"
#include "Game/Managers/BattleManager.h"

AGamePlayerController::AGamePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	// Load the default input mapping context.
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultMappingContextAsset(TEXT("/Game/Input/IMC_Default"));
	if (DefaultMappingContextAsset.Succeeded())
	{
		DefaultMappingContext = DefaultMappingContextAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find DefaultMappingContext asset!"));
	}

	// Load the left mouse click input action.
	static ConstructorHelpers::FObjectFinder<UInputAction> LeftMouseClickActionAsset(TEXT("/Game/Input/IA_LeftMouseClick"));
	if (LeftMouseClickActionAsset.Succeeded())
	{
		LeftMouseClickAction = LeftMouseClickActionAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find LeftMouseClickAction asset!"));
	}

	// Load the right mouse click input action.
	static ConstructorHelpers::FObjectFinder<UInputAction> RightMouseClickActionAsset(TEXT("/Game/Input/IA_RightMouseClick"));
	if (RightMouseClickActionAsset.Succeeded())
	{
		RightMouseClickAction = RightMouseClickActionAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find RightMouseClickAction asset!"));
	}
}

void AGamePlayerController::InitPlayerController(AStrategyGameMode* NewGameMode)
{
	GameMode = NewGameMode;

	if (GameMode.IsValid())
	{
		// Bind to game phase and turn switch events.
		GameMode->OnGamePhaseChanged.RemoveDynamic(this, &AGamePlayerController::OnPhaseChanged);
		GameMode->OnSwitchTurn.RemoveDynamic(this, &AGamePlayerController::OnSwitchTurn);

		GameMode->OnGamePhaseChanged.AddDynamic(this, &AGamePlayerController::OnPhaseChanged);
		GameMode->OnSwitchTurn.AddDynamic(this, &AGamePlayerController::OnSwitchTurn);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to register for GamePhase change in PlacementManager"));
	}
		
	UE_LOG(LogTemp, Display, TEXT("InitPlayerController"));
}

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// Set the input mode to allow both game and UI interactions.
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void AGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind the left mouse click action.
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(LeftMouseClickAction.Get(), ETriggerEvent::Triggered, this, &AGamePlayerController::OnLeftMouseClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to setup LeftMouseClickAction!"));
	}

	// Bind the right mouse click action.
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(RightMouseClickAction.Get(), ETriggerEvent::Triggered, this, &AGamePlayerController::OnRightMouseClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to setup RightMouseClickAction!"));
	}

	// Add the default input mapping context.
	if (DefaultMappingContext.Get())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext.Get(), 0);	
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find Subsystem LocalPlayer"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is null!"));
	}
	
	bShowMouseCursor = true; // Show the mouse cursor.
	
	UE_LOG(LogTemp, Log, TEXT("SetupInputComponent Completed"));
}

void AGamePlayerController::PlacementClick(const ATile* Tile) const
{
	// Broadcast the placement click event with the tile's location.
	OnPlacementClick.Broadcast(Tile->GetActorLocation());
}

void AGamePlayerController::UnitClicked(ABaseUnit* Unit, const bool bIsLeftClick) const
{
	// Broadcast the unit click event.
	OnUnitClicked.Broadcast(Unit, bIsLeftClick);
}

void AGamePlayerController::TileClicked(ATile* Tile, const bool bIsLeftClick) const
{
	// Broadcast the tile click event.
	OnTileClicked.Broadcast(Tile, bIsLeftClick);
}

void AGamePlayerController::OnLeftMouseClicked()
{
	if (!bEnableInput) return; // Ignore input if it is disabled.

	//UE_LOG(LogTemp, Log, TEXT("Left Mouse Clicked"));

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit); // Get the object under the cursor.

	if (!Hit.bBlockingHit)
	{
		UE_LOG(LogTemp, Log, TEXT("Hit Nothing"));
		return;
	}

	// Process tile clicks.
	if (ATile* HitTile = Cast<ATile>(Hit.GetActor()))
	{
		ProcessTileClick(HitTile, true);
		return;
	}

	// Process unit clicks.
	if (ABaseUnit* HitUnit = Cast<ABaseUnit>(Hit.GetActor()))
	{
		ProcessUnitClick(HitUnit, true);
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("Clicked object is not recognized"));
}

void AGamePlayerController::OnRightMouseClicked()
{
	if (!bEnableInput) return; // Ignore input if it is disabled.

	//UE_LOG(LogTemp, Log, TEXT("Right Mouse Clicked"));
	
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit); // Get the object under the cursor.

	if (!Hit.bBlockingHit)
	{
		UE_LOG(LogTemp, Log, TEXT("Hit Nothing"));
		return;
	}

	// Process tile clicks.
	if (ATile* HitTile = Cast<ATile>(Hit.GetActor()))
	{
		ProcessTileClick(HitTile, false);
		return;
	}

	// Process unit clicks.
	if (ABaseUnit* HitUnit = Cast<ABaseUnit>(Hit.GetActor()))
	{
		ProcessUnitClick(HitUnit, false);
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("Clicked object is not recognized"));
}

void AGamePlayerController::ProcessTileClick(ATile* Tile, bool bIsLeftClick)
{
	// Handle tile clicks based on the current game phase.
	switch (CurrentPhase)
	{
	case EGamePhase::Placement:
		PlacementClick(Tile); // Handle placement phase clicks.
		break;
	case EGamePhase::Battle:
		TileClicked(Tile, bIsLeftClick); // Handle battle phase clicks.
		break;
	case EGamePhase::CoinFlip:
	case EGamePhase::Begin:
	case EGamePhase::End:
		break; // Ignore clicks in other phases.
	}
}

void AGamePlayerController::ProcessUnitClick(ABaseUnit* Unit, bool bIsLeftClick)
{
	// Handle unit clicks based on the current game phase.
	switch (CurrentPhase)
	{
	case EGamePhase::Battle:
		UnitClicked(Unit, bIsLeftClick); // Handle battle phase clicks.
		break;
	case EGamePhase::CoinFlip:
	case EGamePhase::Placement:
	case EGamePhase::Begin:
	case EGamePhase::End:
		break; // Ignore clicks in other phases.
	}
}

void AGamePlayerController::OnPhaseChanged(EGamePhase NewPhase)
{
	CurrentPhase = NewPhase; // Update the current game phase.

	UE_LOG(LogTemp, Display, TEXT("Phase Changed : %u"), CurrentPhase);
}

void AGamePlayerController::OnSwitchTurn(bool bIsPlayerTurn)
{
	bEnableInput = bIsPlayerTurn; // Enable or disable input based on whose turn it is.

	UE_LOG(LogTemp, Display, TEXT("Enable Input : %u"), bIsPlayerTurn);
}