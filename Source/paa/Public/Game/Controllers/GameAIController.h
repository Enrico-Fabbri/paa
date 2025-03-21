#pragma once

#include "CoreMinimal.h"
#include "Game/StrategyGameMode.h"
#include "Units/BaseUnit.h"
#include "GameAIController.generated.h"

UENUM()
enum class EBattleState { Idle, Movement, Attack };

/**
 * @brief Game AI Controller Class
 * 
 * Manages AI logic for the game, including decision-making during different phases of gameplay.
 */
UCLASS()
class PAA_API UGameAIController : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief Initializes the AI controller with a reference to the game mode
	 * 
	 * @param NewGameMode Reference to the strategy game mode instance
	 */
	void Initialize(AStrategyGameMode* NewGameMode);

private:
	// -------------------- Phase Handling --------------------
	/**
	 * @brief Handles decisions and logic during the placement phase of the game
	 */
	void HandlePlacementPhase();

	/**
	 * @brief Handles decisions and logic during the battle phase of the game
	 */
	void HandleBattlePhase();

	// -------------------- Event Handlers --------------------
	/**
	 * @brief Handles changes in game phase by updating internal state and executing appropriate logic
	 * 
	 * @param NewPhase The new game phase that has been entered
	 */
	UFUNCTION()
	void OnPhaseChanged(EGamePhase NewPhase);

	/**
	 * @brief Handles turn switching by updating internal state and preparing for AI actions
	 * 
	 * @param bIsPlayerTurn Indicates if it's now the player's turn
	 */
	UFUNCTION()
	void OnSwitchTurn(bool bIsPlayerTurn);

	// -------------------- Phase Management --------------------
	/**
	 * @brief Weak reference to the strategy game mode instance
	 */
	TWeakObjectPtr<AStrategyGameMode> GameMode;

	/**
	 * @brief Current phase of the game (e.g., placement, battle)
	 */
	EGamePhase CurrentPhase;
	
	// ---------------------- Battle State Management
	/**
	 * @brief Current state in battle (e.g., moving, attacking)
	 */
	EBattleState BattleState;

	/**
	 * @brief Current Unit being processed in battle 
	 */
	int32 CurrentUnitIndex;
	
	void ProcessNextAction();
	void ProcessMovement(UBattleManager* BattleManager, AGridManager* GridManager);
	void ProcessAttack(UBattleManager* BattleManager, AGridManager* GridManager);
	
	// --------------------- Internal Helpers -------------------
	void TryMove(const UBattleManager* BattleManager, const AGridManager* GridManager, ABaseUnit* AIUnit);
	void TryAttack(const UBattleManager* BattleManager, const AGridManager* GridManager, ABaseUnit* AIUnit);

	static ABaseUnit* FindNearestPlayerUnit(const ABaseUnit* AIUnit, const TArray<TWeakObjectPtr<ABaseUnit>>& PlayerUnits, const AGridManager* GridManager);	
	static FString FindBestMovementTile(const ABaseUnit* AIUnit, const TArray<FString>& MovementTiles, const ABaseUnit* TargetPlayer, AStrategyGameMode* GameMode);
};