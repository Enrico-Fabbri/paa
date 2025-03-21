#pragma once

#include "CoreMinimal.h"
#include "Grid/Tile.h"
#include "PathfindingUtilities.generated.h"

/**
 * UPathfindingUtilities provides utility functions for pathfinding and area exploration on a grid.
 * It includes functions for finding paths between tiles using A* and exploring areas using BFS.
 */
UCLASS()
class PAA_API UPathfindingUtilities : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	 * Finds a path from a start tile to an end tile using the A* algorithm.
	 * @param TileMap - The map of tile names to tile objects.
	 * @param StartTile - The name of the starting tile.
	 * @param EndTile - The name of the destination tile.
	 * @param GridSizeX - The width of the grid.
	 * @param GridSizeY - The height of the grid.
	 * @param TileSize - The size of each tile in world units.
	 * @param OccupiedTiles - A list of tiles that are currently occupied and cannot be traversed.
	 * @return An array of tile names representing the path from start to end.
	 */
	static TArray<FString> GetPath(const TMap<FString, TWeakObjectPtr<ATile>>& TileMap, const FString& StartTile, const FString& EndTile, int32 GridSizeX, int32 GridSizeY, float TileSize, const TArray<FString>& OccupiedTiles);

	/**
	 * Finds all tiles within a specified range from a center tile using BFS.
	 * @param TileMap - The map of tile names to tile objects.
	 * @param CenterTile - The name of the center tile.
	 * @param Size - The maximum distance (in tiles) from the center tile.
	 * @param ConsiderObstacles - Whether to consider obstacles and occupied tiles.
	 * @param GridSizeX - The width of the grid.
	 * @param GridSizeY - The height of the grid.
	 * @param TileSize - The size of each tile in world units.
	 * @param OccupiedTiles - A list of tiles that are currently occupied and cannot be traversed.
	 * @return An array of tile names representing the reachable area.
	 */
	static TArray<FString> GetArea(const TMap<FString, TWeakObjectPtr<ATile>>& TileMap, const FString& CenterTile, const int32 Size, const bool ConsiderObstacles, const int32 GridSizeX, const int32 GridSizeY, const float TileSize, const TArray<FString>& OccupiedTiles);
};