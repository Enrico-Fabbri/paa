#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Game/StrategyGameMode.h"
#include "Game/Managers/BattleManager.h"
#include "Units/BaseUnit.h"
#include "BattleUI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndSignal, bool, bIsPlayerVictory);

/**
 * 
 */
UCLASS()
class PAA_API UBattleUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void NativeConstruct() override;

	UFUNCTION()
	void Init(TWeakObjectPtr<AStrategyGameMode> NewGameMode);

	UPROPERTY()
	FOnEndSignal OnEndSignal;
	
private:
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Help;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_CloseOverlay;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Overlay_Info;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel_UnitInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_UnitBelonging;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_UnitType;

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
	UProgressBar* ProgressBar_HP;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_Position;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_UnitSelected;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollBox_Events;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_NextTurn;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_Turn;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_NextTurn;
	
	UPROPERTY()
	bool bIsOverlayVisible;

	UPROPERTY()
	TWeakObjectPtr<AStrategyGameMode> GameMode;

	UPROPERTY()
	TArray<TWeakObjectPtr<UTexture2D>> Textures;

	UPROPERTY()
	bool bIsPlayerTurn = false;
	UPROPERTY()
	bool bEndSignal = false;
	UPROPERTY()
	bool bIsPlayerVictory = false;
	
	UFUNCTION()
	void OnHelpButtonClicked();

	UFUNCTION()
	void OnCloseOverlayButtonClicked();

	UFUNCTION()
	void OnUnitSelected(ABaseUnit* Unit, bool bIsPlayerUnit);

	UFUNCTION()
	void OnActionExecuted(const FString Text);

	UFUNCTION()
	void OnCanSkipTurn(const bool bCanSkipTurn);

	UFUNCTION()
	void OnTurnSkipped();

	UFUNCTION()
	void OnSwitchTurn(bool NewBIsPlayerTurn);

	UFUNCTION()
	void OnCanEnd(bool bEnd, bool NewBIsPlayerVictory);

	UFUNCTION()
	void EndTurn();
};

