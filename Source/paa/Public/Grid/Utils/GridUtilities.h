#pragma once

#include "CoreMinimal.h"
#include "Grid/Tile.h"
#include "GridUtilities.Generated.h"

/**
 * UGridUtilities provides utility functions for working with grid-based systems.
 * It includes functions for converting between grid coordinates and world positions,
 * finding neighboring tiles, and retrieving tile information.
 */
UCLASS()
class PAA_API UGridUtilities final : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	 * Retrieves a tile from the tile map using its name.
	 * @param TileMap - The map of tile names to tile objects.
	 * @param TileName - The name of the tile to retrieve.
	 * @return A weak pointer to the tile, or nullptr if not found.
	 */
	static TWeakObjectPtr<ATile> GetTile(const TMap<FString, TWeakObjectPtr<ATile>>& TileMap, const FString& TileName);
	
	/**
	 * Converts a grid coordinate (e.g., "A1") to a world position.
	 * @param TileName - The name of the tile in grid format (e.g., "A1").
	 * @param GridSizeX - The width of the grid.
	 * @param GridSizeY - The height of the grid.
	 * @param TileSize - The size of each tile in world units.
	 * @return The world position of the tile.
	 */
	static FVector GridToWorld(const FString& TileName, const int32 GridSizeX, const int32 GridSizeY, const float TileSize);

	/**
	 * Converts a world position to a grid coordinate (e.g., "A1").
	 * @param TilePosition - The world position of the tile.
	 * @param GridSizeX - The width of the grid.
	 * @param GridSizeY - The height of the grid.
	 * @param TileSize - The size of each tile in world units.
	 * @return The grid coordinate as a string (e.g., "A1").
	 */
	static FString WorldToGrid(const FVector& TilePosition, const int32 GridSizeX, const int32 GridSizeY, const float TileSize);

	/**
	 * Generates a tile name from grid coordinates (e.g., "A1").
	 * @param X - The X coordinate of the tile.
	 * @param Y - The Y coordinate of the tile.
	 * @param GridSizeX - The width of the grid.
	 * @param GridSizeY - The height of the grid.
	 * @return The tile name as a string (e.g., "A1").
	 */
	static FString GetCoordinateName(const int32 X, const int32 Y, const int32 GridSizeX, const  int32 GridSizeY);

	/**
	 * Converts grid coordinates to a world position.
	 * @param X - The X coordinate of the tile.
	 * @param Y - The Y coordinate of the tile.
	 * @param GridSizeX - The width of the grid.
	 * @param GridSizeY - The height of the grid.
	 * @param TileSize - The size of each tile in world units.
	 * @return The world position of the tile.
	 */
	static FVector GetCoordinate(const int32 X, const int32 Y, const int32 GridSizeX, const  int32 GridSizeY, const float TileSize);

	/**
	 * Retrieves the names of neighboring tiles for a given tile.
	 * @param TileName - The name of the tile in grid format (e.g., "A1").
	 * @param GridSizeX - The width of the grid.
	 * @param GridSizeY - The height of the grid.
	 * @param TileSize - The size of each tile in world units.
	 * @return An array of neighboring tile names.
	 */
	static TArray<FString> GetNeighborsName(const FString& TileName, const int32 GridSizeX, const int32 GridSizeY, const float TileSize);

	/**
	 * Retrieves the neighboring tiles for a given tile.
	 * @param TileMap - The map of tile names to tile objects.
	 * @param TileName - The name of the tile in grid format (e.g., "A1").
	 * @param GridSizeX - The width of the grid.
	 * @param GridSizeY - The height of the grid.
	 * @param TileSize - The size of each tile in world units.
	 * @return An array of weak pointers to neighboring tiles.
	 */
	static TArray<TWeakObjectPtr<ATile>> GetNeighbors(const TMap<FString, TWeakObjectPtr<ATile>>& TileMap, const FString& TileName, const int32 GridSizeX, const int32 GridSizeY, const float TileSize);
};