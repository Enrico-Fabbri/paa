#pragma once

#include "CoreMinimal.h"
#include "Units/BaseUnit.h"
#include "Units/SniperUnit.h"
#include "DamageSystem.generated.h"

/**
 * System responsible for handling damage calculations and counter-attacks between units.
 */
UCLASS()
class PAA_API UDamageSystem : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Applies damage from an attacker to a defender and handles counter-attacks if applicable.
	 * @param Attacker - The unit initiating the attack.
	 * @param Defender - The unit receiving the attack.
	 * @return A pair of integers representing the damage dealt and the counter-attack damage (if any).
	 */
	static TPair<int32, int32> ApplyDamage(const TWeakObjectPtr<ABaseUnit> Attacker, const TWeakObjectPtr<ABaseUnit> Defender);

private:
	/**
	 * Handles counter-attack logic for sniper units.
	 * @param Attacker - The sniper unit initiating the counter-attack.
	 * @return The damage dealt by the counter-attack.
	 */
	static int32 ApplyCounterAttack(const TWeakObjectPtr<ASniperUnit> Attacker);
};
