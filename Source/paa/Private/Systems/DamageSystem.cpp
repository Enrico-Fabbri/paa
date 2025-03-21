#include "Systems/DamageSystem.h"

#include "Units/BrawlerUnit.h"

TPair<int32, int32> UDamageSystem::ApplyDamage(const TWeakObjectPtr<ABaseUnit> Attacker, const TWeakObjectPtr<ABaseUnit> Defender)
{
	const int32 Damage = Attacker->GetDamage();
	int32 DamageCounter = -1;
	
	// Apply damage to the defender.
	Defender->GetDamaged(Damage);

	// Handle counter-attack logic for sniper units.
	if (ASniperUnit* Sniper = Cast<ASniperUnit>(Attacker))
	{
		if (const ABrawlerUnit* Brawler = Cast<ABrawlerUnit>(Defender); Brawler && !Sniper->IsNeighbour(Brawler->GetPosition())) 
			return TPair<int32, int32>(Damage, DamageCounter);
		
		DamageCounter = ApplyCounterAttack(Sniper);
	}

	return TPair<int32, int32>(Damage, DamageCounter);
}

int32 UDamageSystem::ApplyCounterAttack(const TWeakObjectPtr<ASniperUnit> Attacker)
{
	// Apply a random amount of counter-attack damage to the sniper.
	const int32 Damage = FMath::RandRange(1, 3);
	
	Attacker->GetDamaged(Damage);

	return Damage;
}