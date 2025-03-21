#include "Units/BaseUnit.h"

#include "Grid/Utils/GridUtilities.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseUnit::ABaseUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the static mesh component and set it as the RootComponent.
	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
	if (UnitMesh)
	{
		RootComponent = UnitMesh;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create UnitMesh component"));
	}
    
	// Find and set the default static mesh asset (a Plane shape from the Engine content).
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMesh.Succeeded())
	{
		UnitMesh->SetStaticMesh(PlaneMesh.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to set PlaneMesh asset for UnitMesh"));
	}

	// Load the base material and create a dynamic material instance.
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/Materials/M_UnitBrawler.M_UnitBrawler"));
	if (Material.Succeeded())
	{
		UnitMaterial = UMaterialInstanceDynamic::Create(Material.Object, this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load material '/Game/Materials/M_UnitBrawler.M_UnitBrawler'"));
	}

	// If the dynamic material was created successfully, assign it to the mesh.
	if (UnitMaterial)
	{
		UnitMesh->SetMaterial(0, UnitMaterial);
	}
}

// Called every frame
void ABaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If we have a valid path to follow
	if (CurrentPath.Num() > 0 && CurrentPathIndex < CurrentPath.Num())
	{
		// Get the GridManagerManager actor (assuming one exists in the scene)
		if (GridSystem)
		{
			// Convert the current tile name to a world position
			FVector Destination = GridSystem->GridToWorld(CurrentPath[CurrentPathIndex]);
			Destination.Z = 1.f;
			FVector CurrentLocation = GetActorLocation();
			
			FVector Distance = Destination - CurrentLocation;
			
			// Interpolate smoothly from the current location to the destination
			FVector NewLocation;
			if (Distance.X > 0.f)
			{
				NewLocation = FVector(CurrentLocation.X + MovementSpeed, CurrentLocation.Y, 1.f);
			}
			else if (Distance.Y > 0.f)
			{
				NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y + MovementSpeed, 1.f);
			}
			else if (Distance.X < 0.f)
			{
				NewLocation = FVector(CurrentLocation.X - MovementSpeed, CurrentLocation.Y, 1.f);
			}
			else if (Distance.Y < 0.f)
			{
				NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y - MovementSpeed, 1.f);
			}
			else
			{
				NewLocation = Destination;
				
				CurrentPathIndex++;

				// If we have reached the end of the path, clear it
				if (CurrentPathIndex >= CurrentPath.Num())
				{
					CurrentPath.Empty();
					CurrentPathIndex = 0;
				}
			}
			
			SetActorLocation(NewLocation);
			UnitPosition = GridSystem->WorldToGrid(NewLocation);
		}
	}
}

void ABaseUnit::SetTextureColor(const int32 ColorIndex)
{
	TextureIndex = ColorIndex;

	if (UnitMaterial)
	{
		UnitMaterial->SetScalarParameterValue("TextureIndex", ColorIndex);
	}
}

void ABaseUnit::SetUnitPosition(const FString& NewUnitPosition)
{
	UnitPosition = NewUnitPosition;

	if (GridSystem)
	{
		FVector Position = UGridUtilities::GridToWorld(UnitPosition, GridSystem->GetGridSizeX(), GridSystem->GetGridSizeY(), GridSystem->GetTileSize());
		Position.Z = 1.f;

		SetActorLocation(Position);
	}
}

UMaterialInstanceDynamic* ABaseUnit::GetMaterial() const
{
	return UnitMaterial;
}

int32 ABaseUnit::GetTextureColor() const
{
	return TextureIndex;
}

FString ABaseUnit::GetPosition() const
{
	return UnitPosition;
}

bool ABaseUnit::IsNeighbour(const FString& TileName) const
{
	return GridSystem->GetNeighbours(UnitPosition).Contains(TileName);
}

int32 ABaseUnit::GetMovementRange() const
{
	return MovementRange;
}

int32 ABaseUnit::GetCurrentLifePoint() const
{
	return LifePointsCurrent;
}

int32 ABaseUnit::GetMaxLifePoint() const
{
	return LifePointsMax;
}

EAttackType ABaseUnit::GetAttackType() const
{
	return AttackType;
}

int32 ABaseUnit::GetAttackRange() const
{
	return AttackRange;
}

int32 ABaseUnit::GetMaxDamage() const
{
	return DamageMax;
}

int32 ABaseUnit::GetMinDamage() const
{
	return DamageMin;
}

int32 ABaseUnit::GetDamage() const
{
	return FMath::RandRange(DamageMin, DamageMax);
}

/*void ABaseUnit::FollowPath(const FString& EndTile)
{
	CurrentPath = GridManager->FindPath(UnitPosition, EndTile);
	CurrentPathIndex = 0;
}*/

void ABaseUnit::FollowPath(const FString& EndTile, const TArray<FString>& OccupiedTiles)
{
	CurrentPath = GridSystem->FindPath(UnitPosition, EndTile, OccupiedTiles);
	CurrentPathIndex = 0;
}

void ABaseUnit::GetDamaged(const int32 Damage)
{
	LifePointsCurrent -= Damage;

	if (LifePointsCurrent <= 0)
	{
		LifePointsCurrent = 0;
	}
}

void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();

	GridSystem = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
}

bool ABaseUnit::IsDead() const
{
	return LifePointsCurrent == 0;
}

bool ABaseUnit::IsMoving() const
{
	return CurrentPath.Num() > 0;
}
