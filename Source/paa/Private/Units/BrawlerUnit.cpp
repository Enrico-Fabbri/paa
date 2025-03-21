#include "Units/BrawlerUnit.h"

ABrawlerUnit::ABrawlerUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	UnitPosition = "A1";
	MovementRange = 6;
	AttackType = EAttackType::Melee;
	AttackRange = 1;
	DamageMax = 6;
	DamageMin = 1;
	LifePointsMax = 40;
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

void ABrawlerUnit::BeginPlay()
{
	ABaseUnit::BeginPlay();
}

void ABrawlerUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

