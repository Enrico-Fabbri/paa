#include "Grid/Utils/ObstaclesUtilities.h"

#include "Grid/Utils/GridUtilities.h"

void UObstaclesUtilities::GenerateObstacles(TMap<FString, TWeakObjectPtr<ATile>>& TileMap, const int32 GridSizeX, const int32 GridSizeY, const float TileSize, const float ObstaclePercentage)
{
    // Reset all tiles to their default state (no obstacles).
    ResetTiles(TileMap);

    // Calculate the total number of obstacles to generate based on the percentage.
    const int32 TotalObstacles = FMath::FloorToInt(GridSizeX * GridSizeY * ObstaclePercentage);
    TArray Explored = TArray<FString>(); // Tracks tiles that have already been processed.

    // Choose a random starting tile for the DFS.
    const int32 X = FMath::RandRange(0, GridSizeX - 1);
    const int32 Y = FMath::RandRange(0, GridSizeY - 1);
    const FString Current = UGridUtilities::GetCoordinateName(X, Y, GridSizeX, GridSizeY);

    // Perform DFS to generate obstacles.
    DFS(TileMap, Explored, Current, GridSizeX, GridSizeY, TileSize, TotalObstacles);
}

void UObstaclesUtilities::DFS(TMap<FString, TWeakObjectPtr<ATile>>& TileMap, TArray<FString>& Explored,
    const FString& Current, const int32 GridSizeX, const int32 GridSizeY, const float TileSize, const int32 TotalObstacles)
{
    // Stop if the current tile is invalid, already processed, or the obstacle limit is reached.
    if (!TileMap.Contains(Current) || !TileMap[Current]->IsObstacle() || TileMap.Num() - Explored.Num() <= TotalObstacles) return;

    // Mark the current tile as not being an obstacle.
    TileMap[Current]->SetIsObstacle(false);
    TileMap[Current]->UpdateMaterial(); // Update the tile's visual representation.
    
    // Add the current tile to the list of explored tiles.
    Explored.Add(Current);

    // Get the neighboring tiles.
    TArray<FString> Neighbors = UGridUtilities::GetNeighborsName(Current, GridSizeX, GridSizeY, TileSize);

    // Shuffle the neighbors to randomize the DFS traversal.
    for (int32 i = Neighbors.Num() - 1; i > 0; --i)
    {
        const int32 j = FMath::RandRange(0, i);
        Neighbors.Swap(i, j);
    }

    // Recursively process each neighbor.
    for (const FString& Neighbor : Neighbors)
    {
        DFS(TileMap, Explored, Neighbor, GridSizeX, GridSizeY, TileSize, TotalObstacles);
    }
}

void UObstaclesUtilities::ResetTiles(TMap<FString, TWeakObjectPtr<ATile>>& TileMap)
{
    // Iterate through all tiles in the map.
    for (auto& Elem : TileMap)
    {
        if (Elem.Value.IsValid())
        {
            // Reset the tile to its default state (obstacle).
            Elem.Value->SetIsObstacle(true);
            Elem.Value->UpdateMaterial(); // Update the tile's visual representation.
        }
    }
}