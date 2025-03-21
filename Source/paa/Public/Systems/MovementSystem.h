#pragma once

#include "CoreMinimal.h"
#include "Units/BaseUnit.h"
#include "MovementSystem.generated.h"

/**
 * System responsible for handling unit movement within the game.
 */
UCLASS()
class PAA_API UMovementSystem : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Applies movement to a unit, moving it to the specified end tile while avoiding occupied tiles.
	 * @param Mover - The unit to move.
	 * @param EndTile - The target tile to move to.
	 * @param OccupiedTiles - List of tiles currently occupied by other units or obstacles.
	 */
	static void ApplyMovement(const TWeakObjectPtr<ABaseUnit> Mover, const FString& EndTile, const TArray<FString>& OccupiedTiles);
};