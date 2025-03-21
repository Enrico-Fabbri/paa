#include "Grid/Utils/GridUtilities.h"

TWeakObjectPtr<ATile> UGridUtilities::GetTile(const TMap<FString, TWeakObjectPtr<ATile>>& TileMap, const FString& TileName)
{
	// Look up the tile in the map.
	if (const TWeakObjectPtr<ATile>* FoundTile = TileMap.Find(TileName))
	{
		// Return the weak pointer directly.
		return *FoundTile;
	} 
	// Return an empty weak pointer if not found.
	return TWeakObjectPtr<ATile>();
}

FVector UGridUtilities::GridToWorld(const FString& TileName, const int32 GridSizeX, const int32 GridSizeY, const float TileSize)
{
	// Ensure the TileName is at least 2 characters long.
	if (TileName.Len() < 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("GridToWorld: Invalid TileName '%s' (too short)."), *TileName);
		return FVector::ZeroVector;
	}
    
	// Extract the column letter (assumes a single letter column, e.g., 'A' ... 'Y').
	TCHAR ColumnChar = TileName[0];
	int32 ColumnIndex = ColumnChar - 'A';
    
	// Extract the row part (the rest of the string) and convert it to an integer.
	FString RowString = TileName.Mid(1);  // Get everything after the first character.
	int32 RowNumber = FCString::Atoi(*RowString);
    
	// Convert from 1-indexed row number (e.g., "1" for the first row) to a zero-based index.
	int32 RowIndex = RowNumber - 1;

	// Optionally, add bounds checking here to ensure that ColumnIndex and RowIndex are within valid ranges.
	if (ColumnIndex < 0 || ColumnIndex >= GridSizeX || RowIndex < 0 || RowIndex >= GridSizeY)
	{
		UE_LOG(LogTemp, Warning, TEXT("GridToWorld: TileName '%s' out of grid bounds."), *TileName);
		return FVector::ZeroVector;
	}
    
	// Calculate and return the world location.
	return FVector(ColumnIndex * TileSize, (GridSizeY - RowIndex - 1) * TileSize, 0.0f);
}

FString UGridUtilities::WorldToGrid(const FVector& TilePosition, const int32 GridSizeX, const int32 GridSizeY, const float TileSize)
{
	// Convert world position to grid coordinates.
	const int32 X = FMath::FloorToInt(TilePosition.X / TileSize);
	const int32 Y = FMath::FloorToInt(TilePosition.Y / TileSize);

	// Convert grid coordinates to a tile name.
	return GetCoordinateName(X, GridSizeY - Y - 1, GridSizeX, GridSizeY);
}

FString UGridUtilities::GetCoordinateName(const int32 X, const int32 Y, const int32 GridSizeX, const  int32 GridSizeY)
{
	// Check for valid indices.
	if (X < 0 || X >= GridSizeX || Y < 0 || Y >= GridSizeY) return "";
	
	// Format the coordinate as a letter (X offset from 'A') and a number (Y + 1).
	return FString::Printf(TEXT("%c%d"), 'A' + X, Y + 1);
}

FVector UGridUtilities::GetCoordinate(const int32 X, const int32 Y, const int32 GridSizeX, const  int32 GridSizeY, const float TileSize)
{
	// Check for valid indices.
	if (X < 0 || X >= GridSizeX || Y < 0 || Y >= GridSizeY) return FVector(-1, -1, 0);
	
	// Calculate and return the world position.
	return FVector(X * TileSize, (GridSizeY - Y - 1) * TileSize, 0.0f);
}

TArray<FString> UGridUtilities::GetNeighborsName(const FString& TileName, const int32 GridSizeX, const int32 GridSizeY, const float TileSize)
{
	TArray<FString> Neighbours;

	// Get the world position of the tile.
	const FVector TilePos = GridToWorld(TileName, GridSizeX, GridSizeY, TileSize);

	// Define the four cardinal directions.
	const FVector Directions[] = {
		FVector(0, TileSize, 0),  // North
		FVector(0, -TileSize, 0), // South
		FVector(-TileSize, 0, 0), // West
		FVector(TileSize, 0, 0)   // East
	};

	// Check each direction for a valid neighboring tile.
	for (const FVector& Dir : Directions)
	{
		const FVector AdjacentPos = TilePos + Dir;
		const FString AdjacentName = WorldToGrid(AdjacentPos, GridSizeX, GridSizeY, TileSize);
		if (!AdjacentName.IsEmpty())
		{
			Neighbours.Add(AdjacentName); // Add the neighboring tile name to the list.
		}
	}

	return Neighbours;
}

TArray<TWeakObjectPtr<ATile>> UGridUtilities::GetNeighbors(const TMap<FString, TWeakObjectPtr<ATile>>& TileMap, const FString& TileName, const int32 GridSizeX, const int32 GridSizeY, const float TileSize)
{
	TArray<TWeakObjectPtr<ATile>> Neighbours;

	// Get the names of neighboring tiles.
	for (TArray<FString> Names = GetNeighborsName(TileName, GridSizeX, GridSizeY, TileSize); const auto& Name : Names)
	{
		// Add the neighboring tile to the list.
		Neighbours.Add(TileMap[Name]);
	}
	
	return Neighbours;
}