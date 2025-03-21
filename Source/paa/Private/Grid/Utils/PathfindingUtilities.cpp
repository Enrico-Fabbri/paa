#include "Grid/Utils/PathfindingUtilities.h"

#include "Grid/Utils/GridUtilities.h"
#include "Algo/Reverse.h"

/**
 * Parses a tile name into grid coordinates (e.g., "A1" → (0, 0)).
 * @param TileName - The name of the tile in grid format (e.g., "A1").
 * @return A pair of integers representing the column and row indices.
 */
TPair<int32, int32> ParseTileName(const FString& TileName)
{
    if (TileName.Len() < 2)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid tile name: %s"), *TileName);
        return {-1, -1}; // Indicates invalid tile
    }

    // Extract column (e.g., 'A' → 0, 'B' → 1)
    const TCHAR ColumnChar = TileName[0];
    const int32 ColumnIndex = ColumnChar - 'A';

    // Extract row (e.g., "1" → 0, "2" → 1)
    const FString RowString = TileName.Mid(1); // Get everything after the first character
    const int32 RowIndex = FCString::Atoi(*RowString) - 1; // Convert to zero-based index

    return {ColumnIndex, RowIndex};
}

/**
 * Calculates the Manhattan distance heuristic between two tiles.
 * @param A - The name of the first tile.
 * @param B - The name of the second tile.
 * @return The Manhattan distance between the two tiles.
 */
float Heuristic(const FString& A, const FString& B)
{
    const TPair<int32, int32> CoordA = ParseTileName(A);
    const TPair<int32, int32> CoordB = ParseTileName(B);
    
    return FMath::Abs(CoordA.Key - CoordB.Key) + FMath::Abs(CoordA.Value - CoordB.Value);
}

TArray<FString> UPathfindingUtilities::GetPath(const TMap<FString, TWeakObjectPtr<ATile>>& TileMap,
    const FString& StartTile, const FString& EndTile, int32 GridSizeX, int32 GridSizeY, float TileSize,
    const TArray<FString>& OccupiedTiles)
{
    TArray<FString> Path;

    // 1. Validate start/end tiles using safe access
    const TWeakObjectPtr<ATile> StartTilePtr = TileMap.FindRef(StartTile);
    const TWeakObjectPtr<ATile> EndTilePtr = TileMap.FindRef(EndTile);

    if (!StartTilePtr.IsValid() || !EndTilePtr.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid start or end tile"));
        return Path;
    }

    if (StartTilePtr->IsObstacle() || EndTilePtr->IsObstacle())
    {
        UE_LOG(LogTemp, Warning, TEXT("Start or End tile is obstacle"));
        return Path;
    }

    if (OccupiedTiles.Contains(EndTile))
    {
        UE_LOG(LogTemp, Warning, TEXT("End tile is occupied"));
        return Path;
    }   
    
    if (StartTile == EndTile)
    {
        Path.Add(StartTile);
        return Path;
    }

    // 2. Initialize scores for all possible grid coordinates
    TMap<FString, FString> CameFrom;
    TMap<FString, float> GScore;
    TMap<FString, float> FScore;

    for (int32 X = 0; X < GridSizeX; ++X)
    {
        for (int32 Y = 0; Y < GridSizeY; ++Y)
        {
            const FString TileName = UGridUtilities::GetCoordinateName(X, Y, GridSizeX, GridSizeY);
            GScore.Add(TileName, TNumericLimits<float>::Max());
            FScore.Add(TileName, TNumericLimits<float>::Max());
        }
    }

    GScore[StartTile] = 0.0f;
    FScore[StartTile] = Heuristic(StartTile, EndTile);

    // 3. A* Algorithm Implementation
    TArray<FString> OpenSet;
    TSet<FString> OpenSetCheck;
    OpenSet.Add(StartTile);
    OpenSetCheck.Add(StartTile);

    while (!OpenSet.IsEmpty())
    {
        // Sort by lowest FScore
        OpenSet.Sort([&FScore](const FString& A, const FString& B) {
            return FScore[A] < FScore[B];
        });

        FString Current = OpenSet[0];
        OpenSet.RemoveAt(0);
        OpenSetCheck.Remove(Current);

        if (Current == EndTile)
        {
            // Reconstruct path
            FString ReconstructCurrent = EndTile;
            while (ReconstructCurrent != StartTile)
            {
                Path.Add(ReconstructCurrent);
                ReconstructCurrent = CameFrom.FindRef(ReconstructCurrent);
            }
            Path.Add(StartTile);
            Algo::Reverse(Path);
            return Path;
        }

        // 4. Safe neighbor processing
        const TArray<FString> Neighbors = UGridUtilities::GetNeighborsName(Current, GridSizeX, GridSizeY, TileSize);

        for (const FString& Neighbor : Neighbors)
        {
            // Safe access pattern
            const TWeakObjectPtr<ATile> NeighborTile = TileMap.FindRef(Neighbor);
            const float CurrentGScore = GScore.FindRef(Current);
            
            if (!NeighborTile.IsValid() || NeighborTile->IsObstacle() || OccupiedTiles.Contains(Neighbor))
                continue;

            // 5. Safe score access using FindRef
            const float TentativeGScore = CurrentGScore + 1.0f;
            const float NeighborGScore = GScore.FindRef(Neighbor);

            if (TentativeGScore < NeighborGScore)
            {
                CameFrom.Add(Neighbor, Current);
                GScore.Add(Neighbor, TentativeGScore);
                FScore.Add(Neighbor, TentativeGScore + Heuristic(Neighbor, EndTile));

                if (!OpenSetCheck.Contains(Neighbor))
                {
                    OpenSet.Add(Neighbor);
                    OpenSetCheck.Add(Neighbor);
                }
            }
        }
    }

    return Path;
}

TArray<FString> UPathfindingUtilities::GetArea(const TMap<FString, TWeakObjectPtr<ATile>>& TileMap,
    const FString& CenterTile, const int32 Size, const bool ConsiderObstacles, const int32 GridSizeX,
    const int32 GridSizeY, const float TileSize, const TArray<FString>& OccupiedTiles)
{
    TArray<FString> ReachableTiles;

    // Ensure the center tile exists.
    if (!TileMap.Contains(CenterTile))
    {
        return ReachableTiles;
    }

    TWeakObjectPtr<ATile> CenterTilePtr = TileMap[CenterTile];
    if (!CenterTilePtr.IsValid())
    {
        return ReachableTiles;
    }

    // If obstacles should be considered and the center tile is blocked or occupied, return an empty area.
    if (ConsiderObstacles && CenterTilePtr->IsObstacle())
    {
        return ReachableTiles;
    }

    // We'll perform a breadth-first search (BFS) using a queue.
    // Each element is a pair (TileName, MovementCost) where MovementCost is how far the tile is from the center.
    TArray<TPair<FString, int32>> Queue;
    TSet<FString> Visited;

    // Start from the center tile, which is at distance 0.
    Queue.Add(TPair<FString, int32>(CenterTile, 0));
    Visited.Add(CenterTile);

    while (Queue.Num() > 0)
    {
        // Pop the first element (FIFO).
        TPair<FString, int32> CurrentPair = Queue[0];
        Queue.RemoveAt(0);

        FString CurrentTile = CurrentPair.Key;
        int32 CurrentDistance = CurrentPair.Value;

        // Add the current tile to the reachable list.
        ReachableTiles.Add(CurrentTile);

        // If we haven't reached the movement limit, check the neighbors.
        if (CurrentDistance < Size)
        {
            // Get the neighbors of the current tile.
            TArray<FString> Neighbors = UGridUtilities::GetNeighborsName(CurrentTile, GridSizeX, GridSizeY, TileSize);

            for (const FString& Neighbor : Neighbors)
            {
                // Skip if the neighbor is not part of the map.
                if (!TileMap.Contains(Neighbor))
                {
                    continue;
                }

                TWeakObjectPtr<ATile> NeighborTile = TileMap[Neighbor];
                if (!NeighborTile.IsValid())
                {
                    continue;
                }

                // If we must consider obstacles, then skip any neighbor that is an obstacle or occupied.
                if (ConsiderObstacles && (NeighborTile->IsObstacle() || OccupiedTiles.Contains(Neighbor)))
                {
                    continue;
                }

                // If the neighbor has not yet been visited, add it to the queue.
                if (!Visited.Contains(Neighbor))
                {
                    Visited.Add(Neighbor);
                    Queue.Add(TPair<FString, int32>(Neighbor, CurrentDistance + 1));
                }
            }
        }
    }

    return ReachableTiles;  
}