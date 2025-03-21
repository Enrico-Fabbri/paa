#pragma once

#include "CoreMinimal.h"
#include "PlayerPawn.h"
#include "GameFramework/GameModeBase.h"
#include "StrategyGameMode.generated.h"

class UUIManager;
class UBattleManager;
class UPlacementManager;
class AGamePlayerController;
class AGridManager;
class UGameAIController;

UENUM(BlueprintType)
enum class EGamePhase : uint8 { Begin, CoinFlip, Placement, Battle, End };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EGamePhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchedTurn, bool, bIsPlayerTurn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitsColorSet, int32, Color);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObstaclePercentageSet, float, ObstaclePercentage);

/**
 * Main game mode class responsible for managing the game's state, phases, and transitions.
 */
UCLASS()
class PAA_API AStrategyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AStrategyGameMode();

	UPROPERTY()
	FOnGamePhaseChanged OnGamePhaseChanged;

	/**
	 * Transitions the game to a new phase and triggers associated logic.
	 * @param NewPhase - The phase to transition to.
	 */
	UFUNCTION()
	void TransitionToPhase(EGamePhase NewPhase);

	UPROPERTY()
	FOnSwitchedTurn OnSwitchTurn;

	/**
	 * Switches the turn between the player and AI.
	 */
	UFUNCTION()
	void SwitchTurn();

	/**
	 * Sets the current turn to either the player or AI.
	 * @param bPlayerTurn - True if it's the player's turn, false for AI.
	 */
	UFUNCTION()
	void SetTurn(bool bPlayerTurn);

	UPROPERTY()
	FOnUnitsColorSet OnUnitsColorSet;
	
	UPROPERTY()
	FOnObstaclePercentageSet OnObstaclePercentageSet;
	
	// Getters
	UFUNCTION()
	UWorld* World() const;
	UFUNCTION()
	bool IsPlayerTurn() const;
	
	UFUNCTION()
	TWeakObjectPtr<AGamePlayerController> GetPlayerController() const;
	
	UFUNCTION()
	UPlacementManager* GetPlacementManager();
	UFUNCTION()
	UBattleManager* GetBattleManager();
	UFUNCTION()
	AGridManager* GetGridManager();

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<APlayerPawn> PlayerPawn;
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AGamePlayerController> PlayerController;
	UPROPERTY(VisibleAnywhere)
	UGameAIController* AIController;
	
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AGridManager> GridManager;

	UPROPERTY(VisibleAnywhere)
	UPlacementManager* PlacementManager;
	UPROPERTY(VisibleAnywhere)
	UBattleManager* BattleManager;
	UPROPERTY(VisibleAnywhere)
	UUIManager* UIManager;
	
	UPROPERTY(VisibleAnywhere, Category = "GameMode | Phase")
	EGamePhase CurrentPhase;

	UPROPERTY(VisibleAnywhere, Category = "GameMode | Turn")
	bool bIsPlayerTurn;
};