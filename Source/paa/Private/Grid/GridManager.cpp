#include "Grid/GridManager.h"

#include "Grid/Utils/GridUtilities.h"
#include "Grid/Utils/ObstaclesUtilities.h"
#include "Grid/Utils/PathfindingUtilities.h"

AGridManager::AGridManager()
{
	// Disable ticking as grid systems are static after generation.
	PrimaryActorTick.bCanEverTick = false;
	
	// Create a root component for the grid system.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("GridSystem"));
}

void AGridManager::Initialize(AStrategyGameMode* GameMode)
{
	// Bind to the obstacle percentage change event.
	GameMode->OnObstaclePercentageSet.RemoveDynamic(this, &AGridManager::SetObstaclePercentage);
	GameMode->OnObstaclePercentageSet.AddDynamic(this, &AGridManager::SetObstaclePercentage);
}

void AGridManager::GenerateGrid()
{
	// Clear any previously stored tiles.
	if (TileMap.Num() > 0) TileMap.Empty();
	
	// Loop through grid dimensions.
	for (int X = 0; X < GridSizeX; X++)
	{
		for (int Y = 0; Y < GridSizeY; Y++)
		{
			// Create a coordinate name (e.g., "A1", "B3").
			const FString Name = UGridUtilities::GetCoordinateName(X, Y, GridSizeX, GridSizeY);
			// Get the corresponding world location.
			const FVector Location = UGridUtilities::GetCoordinate(X, Y, GridSizeX, GridSizeY, TileSize);

			// Spawn a tile actor at the calculated location with no rotation.
			ATile* Tile = GetWorld()->SpawnActor<ATile>(Location, FRotator(0, 0, 0));
			// Set the tile as a child of this actor.
			Tile->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			// Set the tile's label in the editor for identification.
			Tile->SetActorLabel(Name);
			
			// Add the tile to the map with its coordinate name.
			TileMap.Add(Name, Tile);
		}
	}
}

void AGridManager::GenerateObstacles()
{
	// Generate obstacles on the grid using the specified obstacle percentage.
	UObstaclesUtilities::GenerateObstacles(TileMap, GridSizeX, GridSizeY, TileSize, ObstaclePercentage);
}

FVector AGridManager::GridToWorld(const FString& TileName) const
{
	// Convert a grid coordinate to a world position.
	return UGridUtilities::GridToWorld(TileName, GridSizeX, GridSizeY, TileSize);
}

FString AGridManager::WorldToGrid(const FVector& TilePosition) const
{
	// Convert a world position to a grid coordinate.
	return UGridUtilities::WorldToGrid(TilePosition, GridSizeX, GridSizeY, TileSize);
}

TWeakObjectPtr<ATile> AGridManager::GetTile(const FString& TileName) const
{
	// Retrieve a tile from the grid using its name.
	return UGridUtilities::GetTile(TileMap, TileName);
}

TArray<FString> AGridManager::GetNeighbours(const FString& TileName) const
{
	// Retrieve the names of neighboring tiles for a given tile.
	return UGridUtilities::GetNeighborsName(TileName, GridSizeX, GridSizeY, TileSize);
}

TArray<FString> AGridManager::FindPath(const FString& StartTile, const FString& EndTile, const TArray<FString>& OccupiedTiles) const
{
	// Find a path from the start tile to the end tile using the A* algorithm.
	return UPathfindingUtilities::GetPath(TileMap, StartTile, EndTile, GridSizeX, GridSizeY, TileSize, OccupiedTiles);
}

TArray<FString> AGridManager::FindArea(const FString& CenterTile, const int32 Size, const bool ConsiderObstacles, const TArray<FString>& OccupiedTiles) const
{
	// Find all tiles within a specified range from the center tile using BFS.
	return UPathfindingUtilities::GetArea(TileMap, CenterTile, Size, ConsiderObstacles, GridSizeX, GridSizeY, TileSize, OccupiedTiles);
}

void AGridManager::ColorTiles(TArray<FString>& Tiles, const FLinearColor Color) const
{
	// Color the specified tiles with the given color.
	for (auto TileName : Tiles)
	{
		ATile* Tile = TileMap[TileName].Get();
		Tile->SetBaseColor(Color);
		Tile->UpdateMaterial();
	}
}

int32 AGridManager::GetGridSizeX() const
{
	// Return the width of the grid.
	return GridSizeX;
}

int32 AGridManager::GetGridSizeY() const
{
	// Return the height of the grid.
	return GridSizeY;
}

float AGridManager::GetTileSize() const
{
	// Return the size of each tile.
	return TileSize;
}

FVector AGridManager::GetGridCenter() const
{
	// Return the center position of the grid in world coordinates.
	return FVector(GridSizeX * TileSize / 2, GridSizeY * TileSize / 2, 0);
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGridManager::SetObstaclePercentage(float NewObstaclePercentage)
{
	// Update the obstacle percentage for the grid.
	ObstaclePercentage = NewObstaclePercentage;
}