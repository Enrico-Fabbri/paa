#include "UI/BeginUI.h"

#include "Game/StrategyGameMode.h"

void UBeginUI::Init(TWeakObjectPtr<AStrategyGameMode> NewGameMode)
{
	GameMode = NewGameMode;
}

void UBeginUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_0->OnPressed.RemoveDynamic(this, &UBeginUI::OnBeginGameClicked);
	Button_0->OnPressed.AddDynamic(this, &UBeginUI::OnBeginGameClicked);

	Button_Options->OnPressed.RemoveDynamic(this, &UBeginUI::OnShowOptions);
	Button_Options->OnPressed.AddDynamic(this, &UBeginUI::OnShowOptions);

	Button_Close->OnPressed.RemoveDynamic(this, &UBeginUI::OnHideOptions);
	Button_Close->OnPressed.AddDynamic(this, &UBeginUI::OnHideOptions);

	Slider_ObstaclePercentage->OnValueChanged.RemoveDynamic(this, &UBeginUI::OnSetObstaclePercentage);
	Slider_ObstaclePercentage->OnValueChanged.AddDynamic(this, &UBeginUI::OnSetObstaclePercentage);

	Button_Blue->OnPressed.RemoveDynamic(this, &UBeginUI::OnSetBlueUnitColor);
	Button_Blue->OnPressed.AddDynamic(this, &UBeginUI::OnSetBlueUnitColor);
	
	Button_Brown->OnPressed.RemoveDynamic(this, &UBeginUI::OnSetBrownUnitColor);
	Button_Brown->OnPressed.AddDynamic(this, &UBeginUI::OnSetBrownUnitColor);
	
	Button_Green->OnPressed.RemoveDynamic(this, &UBeginUI::OnSetGreenUnitColor);
	Button_Green->OnPressed.AddDynamic(this, &UBeginUI::OnSetGreenUnitColor);
	
	Button_Pink->OnPressed.RemoveDynamic(this, &UBeginUI::OnSetPinkUnitColor);
	Button_Pink->OnPressed.AddDynamic(this, &UBeginUI::OnSetPinkUnitColor);
	
	Button_Purple->OnPressed.RemoveDynamic(this, &UBeginUI::OnSetPurpleUnitColor);
	Button_Purple->OnPressed.AddDynamic(this, &UBeginUI::OnSetPurpleUnitColor);

	CanvasPanel_Options->SetIsEnabled(false);
	CanvasPanel_Options->SetVisibility(ESlateVisibility::HitTestInvisible);
	CanvasPanel_Options->SetRenderOpacity(0.0f);
}

void UBeginUI::OnBeginGameClicked()
{
	GameMode->TransitionToPhase(EGamePhase::CoinFlip);
}

void UBeginUI::OnShowOptions()
{
	CanvasPanel_Options->SetIsEnabled(true);
	CanvasPanel_Options->SetVisibility(ESlateVisibility::Visible);
	CanvasPanel_Options->SetRenderOpacity(1.0f);
}

void UBeginUI::OnHideOptions()
{
	CanvasPanel_Options->SetIsEnabled(false);
	CanvasPanel_Options->SetVisibility(ESlateVisibility::HitTestInvisible);
	CanvasPanel_Options->SetRenderOpacity(0.0f);
}

void UBeginUI::OnSetBlueUnitColor()
{
	OnSetUnitsColor(0);
}

void UBeginUI::OnSetBrownUnitColor()
{
	OnSetUnitsColor(1);
}

void UBeginUI::OnSetGreenUnitColor()
{
	OnSetUnitsColor(2);
}

void UBeginUI::OnSetPinkUnitColor()
{
	OnSetUnitsColor(3);
}

void UBeginUI::OnSetPurpleUnitColor()
{
	OnSetUnitsColor(4);
}

void UBeginUI::OnSetUnitsColor(int32 Color)
{
	GameMode->OnUnitsColorSet.Broadcast(Color);
}

void UBeginUI::OnSetObstaclePercentage(float Percentage)
{
	TextBlock_ObstaclePercentage->SetText(FText::FromString(FString::Printf(TEXT("Obstacle Percentage: %.2f"), Percentage)));

	GameMode->OnObstaclePercentageSet.Broadcast(Percentage);
}
