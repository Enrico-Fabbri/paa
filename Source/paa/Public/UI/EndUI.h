#pragma once

#include "CoreMinimal.h"
#include "BattleUI.h"
#include "Blueprint/UserWidget.h"
#include "Game/StrategyGameMode.h"
#include "EndUI.generated.h"

/**
 * 
 */
UCLASS()
class PAA_API UEndUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void NativeConstruct() override;

	UFUNCTION()
	void Init(TWeakObjectPtr<AStrategyGameMode> NewGameMode, UBattleUI* BattleUI);

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_Winner;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Restart;

	UPROPERTY()
	TWeakObjectPtr<AStrategyGameMode> GameMode;
	
	UFUNCTION()
	void RestartGame();

	UFUNCTION()
	void SetWinner(bool bIsPlayerVictory);
};
