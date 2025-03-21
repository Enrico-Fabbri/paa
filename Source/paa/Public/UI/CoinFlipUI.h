#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Game/StrategyGameMode.h"
#include "CoinFlipUI.generated.h"

/**
 * 
 */
UCLASS()
class PAA_API UCoinFlipUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void NativeConstruct() override;

	UFUNCTION()
	void Init(TWeakObjectPtr<AStrategyGameMode> NewGameMode);
	
private:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_0;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ButtonText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text;

	UFUNCTION()
	void OnClicked();

	UPROPERTY()
	TWeakObjectPtr<AStrategyGameMode> GameMode;

	UPROPERTY()
	bool bContinue = false;
	UPROPERTY()
	bool RandomValue;
	
	UPROPERTY()
	int8 CurrentDots = 0;

	UFUNCTION()
	void UpdateDots();
	UFUNCTION()
	void RandomBool();
};
