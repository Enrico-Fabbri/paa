#pragma once

#include "CoreMinimal.h"
#include "Game/StrategyGameMode.h"
#include "Units/BaseUnit.h"
#include "PlacementManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitPlaced, EUnitTypes, UnitType);

/**
 * PlacementManager class handles unit placement during the placement phase.
 * It manages unit spawning, location selection, and unit tracking.
 */
UCLASS()
class PAA_API UPlacementManager : public UObject
{
	GENERATED_BODY()
	
public:
	UPlacementManager();
	
	/**
	 * Initializes the PlacementManager with a reference to the game mode.
	 * @param GameModeRef - The game mode instance.
	 */
	void Initialize(AStrategyGameMode* GameModeRef);

	UPROPERTY()
	FOnUnitPlaced OnUnitPlaced; // Delegate triggered when a unit is placed.

	/**
	 * Places a unit on the grid for the specified player.
	 * @param bIsPlayer - True if the unit is for the player, false for AI.
	 */
	UFUNCTION()
	void PlaceUnit(bool bIsPlayer);

	/**
	 * Sets the type of unit to be placed.
	 * @param UnitType - The type of unit to place.
	 */
	UFUNCTION()
	void SetUnitToPlace(EUnitTypes UnitType);

	/**
	 * Sets the location for the unit to be placed.
	 * @param Location - The world location for the unit.
	 */
	UFUNCTION()
	void SetUnitLocation(FVector Location);

	/**
	 * Sets the color for the units.
	 * @param Color - The color index to apply.
	 */
	UFUNCTION()
	void SetUnitsColor(int32 Color);
	
	/**
	 * Returns the number of remaining units to place for a specific type and player.
	 * @param UnitType - The type of unit.
	 * @param bIsPlayer - True if checking for the player, false for AI.
	 * @return The number of remaining units to place.
	 */
	UFUNCTION()
	int32 RemainingUnitsToPlace(const EUnitTypes UnitType, const bool bIsPlayer) const;

	/**
	 * Resets the unit placement state.
	 */
	UFUNCTION()
	void ResetUnits();
	
private:
	/**
	 * Handles game phase changes.
	 * @param NewPhase - The new game phase.
	 */
	UFUNCTION()
	void OnGamePhaseChanged(EGamePhase NewPhase);
	
	TWeakObjectPtr<AStrategyGameMode> GameMode; // Reference to the game mode.
	EGamePhase CurrentGamePhase; // Current game phase.

	UPROPERTY()
	int32 UnitsColor = 0; // Color index for units.
	
	UPROPERTY(VisibleAnywhere, Category = "Spawn | Units")
	TMap<EUnitTypes, int8> PlayerUnitsPlaced; // Tracks units placed by the player.
	
	UPROPERTY(VisibleAnywhere, Category = "Spawn | Units")
	TMap<EUnitTypes, int8> AIUnitsPlaced; // Tracks units placed by the AI.

	UPROPERTY(VisibleAnywhere)
	int32 RemainingTurns; // Remaining placement turns.

	EUnitTypes SelectedUnitType = EUnitTypes::Brawler; // Currently selected unit type.
	FVector SelectedUnitLocation = FVector(-1, -1, -1); // Currently selected unit location.
};