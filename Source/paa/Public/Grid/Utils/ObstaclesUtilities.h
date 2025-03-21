#pragma once

#include "CoreMinimal.h"
#include "Grid/Tile.h"
#include "ObstaclesUtilities.generated.h"

/**
 * UObstaclesUtilities provides utility functions for generating and managing obstacles on a grid.
 * It includes functions for resetting tiles, generating obstacles, and performing depth-first search (DFS) for obstacle placement.
 */
UCLASS()
class PAA_API UObstaclesUtilities : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	 * Resets all tiles in the grid to their default state (no obstacles).
	 * @param TileMap - The map of tile names to tile objects.
	 */
	static void ResetTiles(TMap<FString, TWeakObjectPtr<ATile>>& TileMap);

	/**
	 * Generates obstacles on the grid based on a specified percentage.
	 * @param TileMap - The map of tile names to tile objects.
	 * @param GridSizeX - The width of the grid.
	 * @param GridSizeY - The height of the grid.
	 * @param TileSize - The size of each tile in world units.
	 * @param ObstaclePercentage - The percentage of tiles to be obstacles (0.0 to 1.0).
	 */
	static void GenerateObstacles(TMap<FString, TWeakObjectPtr<ATile>>& TileMap, const int32 GridSizeX, const int32 GridSizeY, const float TileSize, const float ObstaclePercentage);

private:
	/**
	 * Performs a depth-first search (DFS) to generate obstacles on the grid.
	 * @param TileMap - The map of tile names to tile objects.
	 * @param Explored - A list of tiles that have already been processed.
	 * @param Current - The current tile being processed.
	 * @param GridSizeX - The width of the grid.
	 * @param GridSizeY - The height of the grid.
	 * @param TileSize - The size of each tile in world units.
	 * @param TotalObstacles - The total number of obstacles to generate.
	 */
	static void DFS(TMap<FString, TWeakObjectPtr<ATile>>& TileMap, TArray<FString>& Explored, const FString& Current, const int32 GridSizeX, const int32 GridSizeY, const float TileSize, const int32 TotalObstacles);
};