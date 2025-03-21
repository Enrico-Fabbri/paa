#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Game/StrategyGameMode.h"
#include "BeginUI.generated.h"

/**
 * 
 */
UCLASS()
class PAA_API UBeginUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void Init(TWeakObjectPtr<AStrategyGameMode> NewGameMode);
	
	UFUNCTION()
	virtual void NativeConstruct() override;

private:
	// UI
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_0;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_0;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_1;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Blue;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Brown;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Green;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Pink;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Purple;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_ObstaclePercentage;

	UPROPERTY(meta = (BindWidget))
	USlider* Slider_ObstaclePercentage;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Close;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Options;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel_Options;
	
	UFUNCTION()
	void OnBeginGameClicked();

	UFUNCTION()
	void OnShowOptions();

	UFUNCTION()
	void OnHideOptions();

	UFUNCTION()
	void OnSetBlueUnitColor();
	
	UFUNCTION()
	void OnSetBrownUnitColor();

	UFUNCTION()
	void OnSetGreenUnitColor();

	UFUNCTION()
	void OnSetPinkUnitColor();

	UFUNCTION()
	void OnSetPurpleUnitColor();
	
	UFUNCTION()
	void OnSetUnitsColor(int32 Color);

	UFUNCTION()
	void OnSetObstaclePercentage(float Percentage);
	
	UPROPERTY()
	TWeakObjectPtr<AStrategyGameMode> GameMode;
};
