#include "Grid/Tile.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ATile::ATile()
{
    // This actor does not require ticking.
    PrimaryActorTick.bCanEverTick = false;

    TextureIndex = FMath::RandRange(0, 2);
    
    // Create the static mesh component and set it as the RootComponent.
    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
    if (TileMesh)
    {
        RootComponent = TileMesh;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create TileMesh component"));
    }
    
    // Find and set the default static mesh asset (a Plane shape from the Engine content).
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
    if (PlaneMesh.Succeeded())
    {
        TileMesh->SetStaticMesh(PlaneMesh.Object);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to set PlaneMesh asset for TileMesh"));
    }

    // Load the base material and create a dynamic material instance.
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/Materials/M_GridTile.M_GridTile"));
    if (Material.Succeeded())
    {
        TileMaterial = UMaterialInstanceDynamic::Create(Material.Object, this);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load material '/Game/Materials/M_GridTile.M_GridTile'"));
    }

    // If the dynamic material was created successfully, assign it to the mesh.
    if (TileMaterial)
    {
        TileMesh->SetMaterial(0, TileMaterial);
    }
}

// Getter for obstacle flag
bool ATile::IsObstacle() const
{
    return bIsObstacle;
}

// Getter for base color
FLinearColor ATile::GetBaseColor() const
{
    return BaseColor;
}

// Getter for texture index
int32 ATile::GetTextureIndex() const
{
    return TextureIndex;
}

// Setter for obstacle flag
void ATile::SetIsObstacle(const bool NewbIsObstacle)
{
    bIsObstacle = NewbIsObstacle;
}

// Setter for base color
void ATile::SetBaseColor(const FLinearColor& NewBaseColor)
{
    BaseColor = NewBaseColor;
}

// Setter for texture index
void ATile::SetTextureIndex(const int32 NewTextureIndex)
{
    TextureIndex = NewTextureIndex;
}

// Update the dynamic material parameters based on the current property values.
void ATile::UpdateMaterial() const
{
    if (TileMaterial)
    {
        // Update scalar parameters ("TextureIndex" and "bIsObstacle")
        TileMaterial->SetScalarParameterValue(TEXT("TextureIndex"), TextureIndex);
        TileMaterial->SetScalarParameterValue(TEXT("bIsObstacle"), bIsObstacle ? 1.0f : 0.0f);
        // Update vector parameter ("BaseColor")
        TileMaterial->SetVectorParameterValue(TEXT("BaseColor"), BaseColor);
    }
}

// Called when the actor is constructed or modified in the editor.
void ATile::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // Ensure the material reflects any property changes.
    UpdateMaterial();
}
