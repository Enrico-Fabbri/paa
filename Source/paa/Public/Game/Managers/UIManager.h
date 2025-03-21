#pragma once

#include "CoreMinimal.h"
#include "Game/StrategyGameMode.h"
#include "UI/BattleUI.h"
#include "UI/BeginUI.h"
#include "UI/CoinFlipUI.h"
#include "UI/EndUI.h"
#include "UI/PlacementUI.h"
#include "UIManager.generated.h"

/**
 * UIManager class manages the UI for different game phases.
 * It handles the creation, updating, and switching of UI widgets.
 */
UCLASS()
class PAA_API UUIManager : public UObject
{
	GENERATED_BODY()

public:
	UUIManager();
	
	/**
	 * Initializes the UIManager with a reference to the game mode.
	 * @param GameModeRef - The game mode instance.
	 */
	UFUNCTION()
	void Initialize(AStrategyGameMode* GameModeRef);

	/**
	 * Updates the UI based on the current game phase.
	 * @param NewPhase - The new game phase.
	 */
	UFUNCTION()
	void UpdateUI(EGamePhase NewPhase);

	/**
	 * Returns the currently active UI widget.
	 * @return The active UI widget.
	 */
	UFUNCTION()
	TWeakObjectPtr<UUserWidget> GetActiveUI() const;
	
private:
	/**
	 * Handles game phase changes and updates the UI accordingly.
	 * @param NewPhase - The new game phase.
	 */
	UFUNCTION()
	void OnGamePhaseChanged(EGamePhase NewPhase);

	UPROPERTY()
	TWeakObjectPtr<AStrategyGameMode> GameMode; // Reference to the game mode.

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<UUserWidget> BeginUIClass; // UI class for the Begin phase.

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<UUserWidget> CoinFlipClass; // UI class for the CoinFlip phase.

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<UUserWidget> PlacementUIClass; // UI class for the Placement phase.

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<UUserWidget> BattleUIClass; // UI class for the Battle phase.

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<UUserWidget> EndUIClass; // UI class for the End phase.
	
	UPROPERTY(VisibleAnywhere)
	UBeginUI* BeginUI; // Instance of the Begin UI.

	UPROPERTY(VisibleAnywhere)
	UCoinFlipUI* CoinFlipUI; // Instance of the CoinFlip UI.

	UPROPERTY(VisibleAnywhere)
	UPlacementUI* PlacementUI; // Instance of the Placement UI.

	UPROPERTY(VisibleAnywhere)
	UBattleUI* BattleUI; // Instance of the Battle UI.

	UPROPERTY(VisibleAnywhere)
	UEndUI* EndUI; // Instance of the End UI.

	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<UUserWidget> ActiveUI; // Currently active UI widget.
};