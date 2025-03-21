#include "Game/Managers/UIManager.h"

#include "Blueprint/UserWidget.h"
#include "UI/BattleUI.h"
#include "UI/CoinFlipUI.h"
#include "UI/PlacementUI.h"

UUIManager::UUIManager()
{
	// Load UI widget classes from the game content.
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBeginClassFinder(TEXT("/Game/UI/W_Begin.W_Begin_C"));
	if (WidgetBeginClassFinder.Succeeded())
	{
		BeginUIClass = WidgetBeginClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Unable to initialize BeginClass"));
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetCoinFlipClassFinder(TEXT("/Game/UI/W_CoinFlip.W_CoinFlip_C"));
	if (WidgetCoinFlipClassFinder.Succeeded())
	{
		CoinFlipClass = WidgetCoinFlipClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Unable to initialize CoinFlipClass"));
	}
	
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetPlacementClassFinder(TEXT("/Game/UI/W_Placement.W_Placement_C"));
	if (WidgetPlacementClassFinder.Succeeded())
	{
		PlacementUIClass = WidgetPlacementClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Unable to initialize PlacementClass"));
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBattleClassFinder(TEXT("/Game/UI/W_Battle.W_Battle_C"));
	if (WidgetBattleClassFinder.Succeeded())
	{
		BattleUIClass = WidgetBattleClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Unable to initialize BattleClass"));
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetEndClassFinder(TEXT("/Game/UI/W_End.W_End_C"));
	if (WidgetEndClassFinder.Succeeded())
	{
		EndUIClass = WidgetEndClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Unable to initialize EndClass"));
	}
}

void UUIManager::Initialize(AStrategyGameMode* GameModeRef)
{
	GameMode = GameModeRef;

	if (GameMode.IsValid())
	{
		// Bind to game phase change events.
		GameMode->OnGamePhaseChanged.RemoveDynamic(this, &UUIManager::OnGamePhaseChanged);
		GameMode->OnGamePhaseChanged.AddDynamic(this, &UUIManager::OnGamePhaseChanged);

		// Create UI widgets.
		if (UWorld* World = GameMode->World())
		{
			BeginUI = CreateWidget<UBeginUI>(World, BeginUIClass);
			CoinFlipUI = CreateWidget<UCoinFlipUI>(World, CoinFlipClass);
			PlacementUI = CreateWidget<UPlacementUI>(World, PlacementUIClass);
			BattleUI = CreateWidget<UBattleUI>(World, BattleUIClass);
			EndUI = CreateWidget<UEndUI>(World, EndUIClass);

			// Initialize UI widgets.
			if (BeginUI) BeginUI->Init(GameMode); 
			else UE_LOG(LogTemp, Error, TEXT("Failed to create BeginUI!"));

			if (CoinFlipUI) CoinFlipUI->Init(GameMode);
			else UE_LOG(LogTemp, Error, TEXT("Failed to create CoinFlipUI!"));

			if (PlacementUI) PlacementUI->Init(GameMode);
			else UE_LOG(LogTemp, Error, TEXT("Failed to create PlacementUI!"));

			if (BattleUI) BattleUI->Init(GameMode);
			else UE_LOG(LogTemp, Error, TEXT("Failed to create BattleUI!"));

			if (EndUI) EndUI->Init(GameMode, BattleUI);
			else UE_LOG(LogTemp, Error, TEXT("Failed to create EndUI!"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Unable to initialize UI"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to register for GamePhase change in UIManager"));
	}
}

void UUIManager::UpdateUI(EGamePhase NewPhase)
{
	// Remove the current active UI.
	if (ActiveUI.Get()) ActiveUI->RemoveFromParent();
	
	// Switch to the appropriate UI for the new phase.
	switch (NewPhase)
	{
		case EGamePhase::Begin:
			if (BeginUI) { BeginUI->AddToViewport(1); ActiveUI = BeginUI; }
			break;
		case EGamePhase::CoinFlip:
			if (CoinFlipUI) { CoinFlipUI->AddToViewport(1); ActiveUI = CoinFlipUI; }
			break;
		case EGamePhase::Placement:
			if (PlacementUI) { PlacementUI->AddToViewport(1); ActiveUI = PlacementUI; }
			break;
		case EGamePhase::Battle:
			if (BattleUI) { BattleUI->AddToViewport(1); ActiveUI = BattleUI; }
			break;
		case EGamePhase::End:
			if (EndUI) { EndUI->AddToViewport(1); ActiveUI = EndUI; }
			break;
		default:
			break;
	}
}

TWeakObjectPtr<UUserWidget> UUIManager::GetActiveUI() const
{
	return ActiveUI; // Return the currently active UI widget.
}

void UUIManager::OnGamePhaseChanged(EGamePhase NewPhase)
{
	UpdateUI(NewPhase); // Update the UI when the game phase changes.
}