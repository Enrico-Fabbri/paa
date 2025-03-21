#include "Units/SniperUnit.h"

ASniperUnit::ASniperUnit()
{
	PrimaryActorTick.bCanEverTick = true;
	
	UnitPosition = "A1";
	MovementRange = 3;
	AttackType = EAttackType::Ranged;
	AttackRange = 10;
	DamageMax = 8;
	DamageMin = 4;
	LifePointsMax = 20;
	LifePointsCurrent = LifePointsMax;
    
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
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/Materials/M_UnitSniper.M_UnitSniper"));
	if (Material.Succeeded())
	{
		UnitMaterial = UMaterialInstanceDynamic::Create(Material.Object, this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load material '/Game/Materials/M_UnitSniper.M_UnitSniper'"));
	}

	// If the dynamic material was created successfully, assign it to the mesh.
	if (UnitMaterial)
	{
		UnitMesh->SetMaterial(0, UnitMaterial);
	}
}

void ASniperUnit::BeginPlay()
{
	Super::BeginPlay();
}

void ASniperUnit::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
