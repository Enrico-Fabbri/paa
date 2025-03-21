#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid/GridManager.h"
#include "BaseUnit.generated.h"

UENUM()
enum class EUnitTypes : uint8
{
	None,
	Brawler,
	Sniper,
};

UENUM()
enum class EAttackType : uint8
{
	Melee,
	Ranged,
};


/**
 * @brief The base class for all unit types in the game.
 * 
 * This class provides fundamental functionality and properties that are common to all units,
 * including movement, combat, and stat management. Derived classes (e.g., BrawlerUnit, SniperUnit)
 * can extend this base implementation with specific behaviors.
 */
UCLASS()
class PAA_API ABaseUnit : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseUnit();

	UFUNCTION()
	void SetTextureColor(const int32 ColorIndex);
	UFUNCTION()
	void SetUnitPosition(const FString& UnitPosition);

	UFUNCTION()
	UMaterialInstanceDynamic* GetMaterial() const;
	UFUNCTION()
	int32 GetTextureColor() const;
	UFUNCTION()
	FString GetPosition() const;
	UFUNCTION()
	int32 GetMovementRange() const;
	UFUNCTION()
	int32 GetCurrentLifePoint() const;
	UFUNCTION()
	int32 GetMaxLifePoint() const;
	UFUNCTION()
	EAttackType GetAttackType() const;
	UFUNCTION()
	int32 GetAttackRange() const;
	UFUNCTION()
	int32 GetMaxDamage() const;
	UFUNCTION()
	int32 GetMinDamage() const;
	UFUNCTION()
	int32 GetDamage() const;
	
	UFUNCTION()
	void FollowPath(const FString& EndTile, const TArray<FString>& OccupiedTiles);
	UFUNCTION()
	void GetDamaged(const int32 Damage);

	UFUNCTION()
	bool IsDead() const;
	UFUNCTION()
	bool IsMoving() const;
	UFUNCTION()
	bool IsNeighbour(const FString& TileName) const;
		
protected:
	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere)
	AGridManager* GridSystem = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* UnitMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* UnitMaterial = nullptr;

	UPROPERTY(VisibleAnywhere)
	int32 TextureIndex = 0;

	UPROPERTY(VisibleAnywhere)
	FString UnitPosition = "";
	
	UPROPERTY(VisibleAnywhere)
	TArray<FString> CurrentPath = {};

	UPROPERTY(VisibleAnywhere)
	int32 CurrentPathIndex = 0;
	
	UPROPERTY(VisibleAnywhere)
	float MovementSpeed = 10.f;
	
	UPROPERTY(VisibleAnywhere)
	int32 MovementRange = 0;

	UPROPERTY(VisibleAnywhere)
	EAttackType AttackType = EAttackType::Melee;

	UPROPERTY(VisibleAnywhere)
	int32 AttackRange = 0;

	UPROPERTY(VisibleAnywhere)
	int32 DamageMax = 0;

	UPROPERTY(VisibleAnywhere)
	int32 DamageMin = 0;

	UPROPERTY(VisibleAnywhere)
	int32 LifePointsMax = 0;

	UPROPERTY(VisibleAnywhere)
	int32 LifePointsCurrent = 0;
};
