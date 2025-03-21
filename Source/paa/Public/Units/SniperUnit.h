// Source/paa/Public/Units/SniperUnit.h
#pragma once

#include "CoreMinimal.h"
#include "Units/BaseUnit.h"
#include "SniperUnit.generated.h"

/**
 * @brief The SniperUnit class represents a unit type that specializes in long-range attacks.
 */
UCLASS()
class PAA_API ASniperUnit : public ABaseUnit
{
	GENERATED_BODY()

public:
	/**
	 * @brief Constructor for the SniperUnit class.
	 */
	ASniperUnit();

protected:
	/**
	 * @brief Handles initial setup and initialization of the sniper unit when it begins play.
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Handles behavior updates for the sniper unit on each tick event.
	 * 
	 * @param DeltaSeconds The time elapsed since the last tick event.
	 */
	virtual void Tick(float DeltaSeconds) override;
};