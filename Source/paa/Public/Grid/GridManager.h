#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "Game/StrategyGameMode.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

/**
 * AGridManager is responsible for creating and managing a grid of tiles.
 * It provides functions to generate the grid, place obstacles, reset the grid state,
 * and convert between grid coordinates (tile names) and world coordinates.
 */
UCLASS()
class PAA_API AGridManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGridManager();

	/**
	 * Initializes the grid manager with a reference to the game mode.
	 * @param GameMode - The game mode instance.
	 */
	UFUNCTION()
	void Initialize(AStrategyGameMode* GameMode);
	
	/**
	 * Generates the grid by spawning tiles and storing them in a map.
	 */
	UFUNCTION()
	void GenerateGrid();

	/**
	 * Generates obstacles on the grid based on the obstacle percentage.
	 */
	UFUNCTION()
	void GenerateObstacles();

	/**
	 * Converts a grid coordinate (e.g., "A1") to a world position.
	 * @param TileName - The name of the tile in grid format (e.g., "A1").
	 * @return The world position of the tile.
	 */
	UFUNCTION()
	FVector GridToWorld(const FString& TileName) const;

	/**
	 * Converts a world position to a grid coordinate (e.g., "A1").
	 * @param TilePosition - The world position of the tile.
	 * @return The grid coordinate as a string (e.g., "A1").
	 */
	UFUNCTION()
	FString WorldToGrid(const FVector& TilePosition) const;

	/**
	 * Retrieves a tile from the grid using its name.
	 * @param TileName - The name of the tile in grid format (e.g., "A1").
	 * @return A weak pointer to the tile, or nullptr if not found.
	 */
	UFUNCTION()
	TWeakObjectPtr<ATile> GetTile(const FString& TileName) const;

	/**
	 * Retrieves the names of neighboring tiles for a given tile.
	 * @param TileName - The name of the tile in grid format (e.g., "A1").
	 * @return An array of neighboring tile names.
	 */
	UFUNCTION()
	TArray<FString> GetNeighbours(const FString& TileName) const;
	
	/**
	 * Finds a path from a start tile to an end tile using the A* algorithm.
	 * @param StartTile - The name of the starting tile.
	 * @param EndTile - The name of the destination tile.
	 * @param OccupiedTiles - A list of tiles that are currently occupied and cannot be traversed.
	 * @return An array of tile names representing the path from start to end.
	 */
	UFUNCTION()
	TArray<FString> FindPath(const FString& StartTile, const FString& EndTile, const TArray<FString>& OccupiedTiles) const;

	/**
	 * Finds all tiles within a specified range from a center tile using BFS.
	 * @param CenterTile - The name of the center tile.
	 * @param Size - The maximum distance (in tiles) from the center tile.
	 * @param ConsiderObstacles - Whether to consider obstacles and occupied tiles.
	 * @param OccupiedTiles - A list of tiles that are currently occupied and cannot be traversed.
	 * @return An array of tile names representing the reachable area.
	 */
	UFUNCTION()
	TArray<FString> FindArea(const FString& CenterTile, const int32 Size, const bool ConsiderObstacles, const TArray<FString>& OccupiedTiles) const;

	/**
	 * Colors a list of tiles with a specified color.
	 * @param Tiles - The list of tile names to color.
	 * @param Color - The color to apply to the tiles.
	 */
	UFUNCTION()
	void ColorTiles(TArray<FString>& Tiles, const FLinearColor Color) const;

	/**
	 * Returns the width of the grid.
	 * @return The width of the grid.
	 */
	UFUNCTION()
	int32 GetGridSizeX() const;

	/**
	 * Returns the height of the grid.
	 * @return The height of the grid.
	 */
	UFUNCTION()
	int32 GetGridSizeY() const;

	/**
	 * Returns the size of each tile in world units.
	 * @return The size of each tile.
	 */
	UFUNCTION()
	float GetTileSize() const;

	/**
	 * Returns the center position of the grid in world coordinates.
	 * @return The center position of the grid.
	 */
	UFUNCTION()
	FVector GetGridCenter() const;
	
protected:
	/**
	 * Called when the game starts or when spawned.
	 */
	UFUNCTION()
	virtual void BeginPlay() override;
	
private:
	/**
	 * Updates the obstacle percentage for the grid.
	 * @param NewObstaclePercentage - The new obstacle percentage (0.0 to 1.0).
	 */
	UFUNCTION()
	void SetObstaclePercentage(float NewObstaclePercentage);
	
	UPROPERTY(EditAnywhere)
	int32 GridSizeX = 25; // The width of the grid.

	UPROPERTY(EditAnywhere)
	int32 GridSizeY = 25; // The height of the grid.

	UPROPERTY(EditAnywhere)
	float TileSize = 100.f; // The size of each tile in world units.

	UPROPERTY(EditAnywhere)
	float ObstaclePercentage = 0.3f; // The percentage of tiles to be obstacles (0.0 to 1.0).

	UPROPERTY()
	TMap<FString, TWeakObjectPtr<ATile>> TileMap; // A map of tile names to tile objects.
};