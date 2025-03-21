#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Game/StrategyGameMode.h"
#include "GameFramework/PlayerController.h"
#include "Units/BaseUnit.h"
#include "GamePlayerController.generated.h"

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlacementClick, FVector, Location); // Triggered when a tile is clicked during the placement phase.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitClicked, ABaseUnit*, Unit, bool, bIsLeftClick); // Triggered when a unit is clicked.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTileClicked, ATile*, Tile, bool, bIsLeftClick); // Triggered when a tile is clicked.

/**
 * AGamePlayerController handles player input and interactions during the game.
 * It manages mouse clicks, delegates input events, and processes interactions based on the current game phase.
 */
UCLASS()
class PAA_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGamePlayerController();

	virtual void BeginPlay() override;
	
	/**
	 * Initializes the player controller with a reference to the game mode.
	 * @param NewGameMode - The game mode instance.
	 */
	void InitPlayerController(AStrategyGameMode* NewGameMode);
	
	virtual void SetupInputComponent() override; // Overrides the default input setup to bind custom input actions.

	FOnPlacementClick OnPlacementClick; // Delegate for placement click events.
	void PlacementClick(const ATile* Tile) const; // Handles tile clicks during the placement phase.

	FOnUnitClicked OnUnitClicked; // Delegate for unit click events.
	void UnitClicked(ABaseUnit* Unit, const bool bIsLeftClick) const; // Handles unit clicks.

	FOnTileClicked OnTileClicked; // Delegate for tile click events.
	void TileClicked(ATile* Tile, const bool bIsLeftClick) const; // Handles tile clicks during the battle phase.
	
private:
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AStrategyGameMode> GameMode; // Reference to the game mode.

	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<UInputMappingContext> DefaultMappingContext; // Default input mapping context.
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<UInputAction> LeftMouseClickAction; // Input action for left mouse clicks.
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<UInputAction> RightMouseClickAction; // Input action for right mouse clicks.

	void OnLeftMouseClicked(); // Handles left mouse click events.
	void OnRightMouseClicked(); // Handles right mouse click events.

	void ProcessTileClick(ATile* Tile, bool bIsLeftClick); // Processes tile clicks based on the current game phase.
	void ProcessUnitClick(ABaseUnit* Unit, bool bIsLeftClick); // Processes unit clicks based on the current game phase.

	UFUNCTION()
	void OnPhaseChanged(EGamePhase NewPhase); // Handles game phase changes.

	UPROPERTY(VisibleAnywhere, Category = "Input")
	EGamePhase CurrentPhase; // Current game phase.
	
	UFUNCTION()
	void OnSwitchTurn(bool bIsPlayerTurn); // Handles turn switching.

	UPROPERTY(VisibleAnywhere, Category = "Input | Placement")
	bool bEnableInput; // Whether input is enabled for the player.
};