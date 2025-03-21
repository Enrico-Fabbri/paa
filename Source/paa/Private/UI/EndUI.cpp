#include "UI/EndUI.h"

#include "Components/Button.h"
#include "UI/BattleUI.h"

void UEndUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Button_Restart)
	{
		Button_Restart->OnClicked.RemoveDynamic(this, &UEndUI::RestartGame);
		Button_Restart->OnClicked.AddDynamic(this, &UEndUI::RestartGame);
	}
}

void UEndUI::Init(TWeakObjectPtr<AStrategyGameMode> NewGameMode, UBattleUI* BattleUI)
{
	GameMode = NewGameMode;

	if (BattleUI)
	{
		BattleUI->OnEndSignal.RemoveDynamic(this, &UEndUI::SetWinner);
		BattleUI->OnEndSignal.AddDynamic(this, &UEndUI::SetWinner);
	}
}

void UEndUI::RestartGame()
{
	GameMode->TransitionToPhase(EGamePhase::Begin);
}

void UEndUI::SetWinner(bool bIsPlayerVictory)
{
	TextBlock_Winner->SetText(FText::FromString(bIsPlayerVictory ? "PLAYER WON" : "AI WON"));
}
