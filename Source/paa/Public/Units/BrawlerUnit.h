#pragma once

#include "CoreMinimal.h"
#include "BaseUnit.h"
#include "BrawlerUnit.generated.h"

/**
 * @brief The BrawlerUnit class represents a unit type that specializes in close-range combat.
 */
UCLASS()
class PAA_API ABrawlerUnit : public ABaseUnit
{
	GENERATED_BODY()

public:
	/**
	 * @brief Constructor for the BrawlerUnit class.
	 */
	ABrawlerUnit();

protected:
	/**
	 * @brief Handles initial setup and initialization of the brawler unit when it begins play.
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Handles behavior updates for the brawler unit on each tick event.
	 * 
	 * @param DeltaTime The time elapsed since the last tick event.
	 */
	virtual void Tick(float DeltaTime) override;
};