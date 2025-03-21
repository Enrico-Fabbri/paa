#include "UI/CoinFlipUI.h"

#include "UI/BeginUI.h"

void UCoinFlipUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_0)
	{
		Button_0->OnClicked.RemoveDynamic(this, &UCoinFlipUI::OnClicked);	
		Button_0->OnClicked.AddDynamic(this, &UCoinFlipUI::OnClicked);

		bContinue = false;
		Text->SetText(FText::FromString(""));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Button_0 is not valid!"));
	}
}

void UCoinFlipUI::Init(TWeakObjectPtr<AStrategyGameMode> NewGameMode)
{
	GameMode = NewGameMode;
}

void UCoinFlipUI::OnClicked()
{
	if (!bContinue)
	{
		CurrentDots = 0; 
		UpdateDots();
	}
	else
	{	
		GameMode->TransitionToPhase(EGamePhase::Placement);
		GameMode->SetTurn(RandomValue);
	}
}

void UCoinFlipUI::UpdateDots()
{
	Button_0->SetIsEnabled(false);
	
	const FString DotsString = FString::ChrN(CurrentDots + 1, '.'); // Crea una stringa con N punti

	if (Text)
	{
		Text->SetText(FText::FromString(DotsString));
	}

	CurrentDots++;

	FTimerHandle TimerHandle;
	
	if (CurrentDots < 5)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UCoinFlipUI::UpdateDots, 0.35f, false);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UCoinFlipUI::RandomBool, 0.35f, false);
	}
}

void UCoinFlipUI::RandomBool()
{
	RandomValue = FMath::RandBool();
	const FString RandomBoolUser = FString(RandomValue ? "PLAYER" : "AI");

	const FString RandomBoolString = FString(RandomBoolUser + "\n TURN");

	Button_0->SetIsEnabled(true);
	
	Text->SetText(FText::FromString(RandomBoolString));
	ButtonText->SetText(FText::FromString("Continue"));

	bContinue = true;
}
