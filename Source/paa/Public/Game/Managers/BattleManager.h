#pragma once

#include "CoreMinimal.h"
#include "Game/StrategyGameMode.h"
#include "Units/BrawlerUnit.h"
#include "BattleManager.generated.h"

// Forward Declarations
class ABaseUnit;
class ATile;
class AStrategyGameMode;

// Enum Definitions
UENUM()
enum class EActionType : int8 { None, Move, Attack, MoveAndAttack }; // Represents the type of action a unit can perform.

UENUM()
enum class EClickType { Null, Left, Right }; // Represents the type of click (left or right).

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitSelected, ABaseUnit*, Unit, bool, bIsPlayerUnit); // Triggered when a unit is selected.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionExecuted, FString, FormattedAction); // Triggered when an action is executed.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCanSkipTurn, bool, bCanSkipTurn); // Triggered when the player can skip their turn.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCanEnd, bool, bEnd, bool, bIsPlayerVictory); // Triggered when the game can end.

/**
 * BattleManager class handles the battle logic, including unit selection, movement, attacks, and turn management.
 */
UCLASS()
class PAA_API UBattleManager : public UObject
{
    GENERATED_BODY()

public:
    void Initialize(AStrategyGameMode* GameModeRef); // Initializes the BattleManager with a reference to the game mode.
	void OnTurnSkipped(); // Handles logic when a turn is skipped.
	
	void ResetUnits(); // Resets all units and clears the battle state.
    void AddPlayerUnit(ABaseUnit* Unit); // Adds a unit to the player's unit list.
    void AddAIUnit(ABaseUnit* Unit); // Adds a unit to the AI's unit list.
	
    UFUNCTION()
    TArray<FString> GetOccupied() const; // Returns a list of tiles occupied by units.

    UFUNCTION()
    void UnitSelected(ABaseUnit* Unit, bool bIsLeftClick); // Handles unit selection logic.
    UFUNCTION()
    void TileSelected(ATile* Tile, bool bIsLeftClick); // Handles tile selection logic.

    UFUNCTION()
    void FormatAction(const int32 Damage, const FString& StartingTile, const FString& EndTile, 
                      ABaseUnit* Unit, const int32 DamageCounter) const; // Formats and broadcasts action details.

	UFUNCTION()
	TMap<TWeakObjectPtr<ABaseUnit>, EActionType> GetAIUnits() const; // Returns the AI's units.
	UFUNCTION()
	TMap<TWeakObjectPtr<ABaseUnit>, EActionType> GetPlayerUnits() const; // Returns the player's units.
	UFUNCTION()
	TArray<FString> GetColored() const; // Returns the currently highlighted tiles.

    UPROPERTY()
    FOnUnitSelected OnUnitSelected; // Delegate for unit selection events.
    
    UPROPERTY()
    FOnActionExecuted OnActionExecuted; // Delegate for action execution events.

	UPROPERTY()
	FOnCanSkipTurn OnCanSkipTurn; // Delegate for turn skip events.

	UPROPERTY()
	FOnCanEnd OnCanEnd; // Delegate for game end events.

private:
    void AttackUnit(ABaseUnit* Unit); // Handles attacking a unit.
    void MoveUnit(const FString& GridPosition); // Handles moving a unit.
	void CheckCanSkipTurn() const; // Checks if the player can skip their turn.
	void CheckEndConditions() const; // Checks if the game has ended.

	bool ShouldSelectNewUnit(const ABaseUnit* Unit, const EClickType Click) const; // Determines if a new unit should be selected.
	void HandleNewSelection(ABaseUnit* Unit, bool bIsLeftClick, EClickType Click, TWeakObjectPtr<AGridManager> GridManager); // Handles new unit selection.
	bool IsDeselectionScenario(const ABaseUnit* Unit) const; // Determines if the current selection should be deselected.
	void ClearSelection(TWeakObjectPtr<AGridManager> GridManager); // Clears the current selection.
	bool IsAttackScenario(const ABaseUnit* Unit, const EClickType Click) const; // Determines if an attack should occur.
	void HandleUnitDeath(ABaseUnit* Attacker, ABaseUnit* Target); // Handles unit death logic.
	
    UFUNCTION()
    void OnGamePhaseChanged(EGamePhase NewPhase); // Handles game phase changes.
    UFUNCTION()
    void OnSwitchTurn(const bool NewBIsPlayerTurn); // Handles turn switching.

    TWeakObjectPtr<AStrategyGameMode> GameMode; // Reference to the game mode.
    EGamePhase CurrentGamePhase; // Current game phase.
    bool bIsPlayerTurn; // True if it's the player's turn, false for AI.

    UPROPERTY(VisibleAnywhere)
    TMap<TWeakObjectPtr<ABaseUnit>, EActionType> PlayerUnits; // Tracks player units and their actions.
    
    UPROPERTY(VisibleAnywhere)
    TMap<TWeakObjectPtr<ABaseUnit>, EActionType> AIUnits; // Tracks AI units and their actions.

    UPROPERTY(VisibleAnywhere)
    TWeakObjectPtr<ABaseUnit> SelectedUnit; // Currently selected unit.
    
    UPROPERTY(VisibleAnywhere)
    EClickType ClickType = EClickType::Null; // Type of the last click (left or right).
    
    UPROPERTY(VisibleAnywhere)
    TArray<FString> ColoredTiles; // Tiles currently highlighted for movement/attack.
};