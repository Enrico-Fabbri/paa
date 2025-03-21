#include "Systems/MovementSystem.h"

void UMovementSystem::ApplyMovement(const TWeakObjectPtr<ABaseUnit> Mover,
	const FString& EndTile, const TArray<FString>& OccupiedTiles)
{
	// Moves the unit along the path to the end tile, avoiding occupied tiles.
	Mover->FollowPath(EndTile, OccupiedTiles);
}