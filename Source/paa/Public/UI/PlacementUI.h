#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Game/StrategyGameMode.h"
#include "Units/BaseUnit.h"
#include "PlacementUI.generated.h"

/**
 * 
 */
UCLASS()
class PAA_API UPlacementUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void NativeConstruct() override;

	UFUNCTION()
	void Init(TWeakObjectPtr<AStrategyGameMode> NewGameMode);

private:
	UPROPERTY(meta = (BindWidget))
	UButton* Sniper_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* Brawler_Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_UnitType;
	
	UPROPERTY(meta = (BindWidget))
	UImage* Image_UnitSelected;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_AttackType;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_MovementRange;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_AttackRange;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_DamageRange;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_Remaining;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_Max;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel_R;
	
	TWeakObjectPtr<AStrategyGameMode> GameMode;

	TArray<TWeakObjectPtr<UTexture2D>> Textures;
	
	UFUNCTION()
	void OnSniperButtonClicked();

	UFUNCTION()
	void OnBrawlerButtonClicked();

	UFUNCTION()
	void OnUnitPlaced(EUnitTypes UnitType);

};
