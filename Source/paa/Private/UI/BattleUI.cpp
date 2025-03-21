#include "UI/BattleUI.h"

#include "Game/Managers/BattleManager.h"
#include "Storage/Nodes/FileEntry.h"

void UBattleUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Button_Help)
	{
		Button_Help->OnClicked.RemoveDynamic(this, &UBattleUI::OnHelpButtonClicked);
		Button_Help->OnClicked.AddDynamic(this, &UBattleUI::OnHelpButtonClicked);
	}

	if (Button_CloseOverlay)
	{
		Button_CloseOverlay->OnClicked.RemoveDynamic(this, &UBattleUI::OnCloseOverlayButtonClicked);
		Button_CloseOverlay->OnClicked.AddDynamic(this, &UBattleUI::OnCloseOverlayButtonClicked);
	}

	if (Overlay_Info)
	{
		Overlay_Info->SetVisibility(ESlateVisibility::Hidden);
		bIsOverlayVisible = false;
	}

	if (CanvasPanel_UnitInfo)
	{
		CanvasPanel_UnitInfo->SetVisibility(ESlateVisibility::Hidden);
	}

	if (Button_NextTurn)
	{
		Button_NextTurn->OnClicked.RemoveDynamic(this, &UBattleUI::OnTurnSkipped);
		Button_NextTurn->OnClicked.AddDynamic(this, &UBattleUI::OnTurnSkipped);

		TextBlock_NextTurn->SetText(FText::FromString("Next Turn"));
		bEndSignal = false;
	}

	if (ScrollBox_Events)
	{
		ScrollBox_Events->ClearChildren();
	}
}

void UBattleUI::Init(TWeakObjectPtr<AStrategyGameMode> NewGameMode)
{
	GameMode = NewGameMode;

	if (GameMode.IsValid())
	{
		GameMode->OnSwitchTurn.RemoveDynamic(this, &UBattleUI::OnSwitchTurn);
		GameMode->GetBattleManager()->OnUnitSelected.RemoveDynamic(this, &UBattleUI::OnUnitSelected);
		GameMode->GetBattleManager()->OnActionExecuted.RemoveDynamic(this, &UBattleUI::OnActionExecuted);
		GameMode->GetBattleManager()->OnCanSkipTurn.RemoveDynamic(this, &UBattleUI::OnCanSkipTurn);
		GameMode->GetBattleManager()->OnCanEnd.RemoveDynamic(this, &UBattleUI::OnCanEnd);

		GameMode->OnSwitchTurn.AddDynamic(this, &UBattleUI::OnSwitchTurn);
		GameMode->GetBattleManager()->OnUnitSelected.AddDynamic(this, &UBattleUI::OnUnitSelected);
		GameMode->GetBattleManager()->OnActionExecuted.AddDynamic(this, &UBattleUI::OnActionExecuted);
		GameMode->GetBattleManager()->OnCanSkipTurn.AddDynamic(this, &UBattleUI::OnCanSkipTurn);
		GameMode->GetBattleManager()->OnCanEnd.AddDynamic(this, &UBattleUI::OnCanEnd);
	}

	TArray<FString> TexturePaths = {
		TEXT("/Game/Textures/Units/Soldier1_Blue.Soldier1_Blue"),
		TEXT("/Game/Textures/Units/Soldier1_Brown.Soldier1_Brown"),
		TEXT("/Game/Textures/Units/Soldier1_Green.Soldier1_Green"),
		TEXT("/Game/Textures/Units/Soldier1_Red.Soldier1_Red"),
		TEXT("/Game/Textures/Units/Soldier1_Violet.Soldier1_Violet"),
		TEXT("/Game/Textures/Units/Soldier2_Blue.Soldier2_Blue"),
		TEXT("/Game/Textures/Units/Soldier2_Brown.Soldier2_Brown"),
		TEXT("/Game/Textures/Units/Soldier2_Green.Soldier2_Green"),
		TEXT("/Game/Textures/Units/Soldier2_Red.Soldier2_Red"),
		TEXT("/Game/Textures/Units/Soldier2_Violet.Soldier2_Violet")
	};

	// Carica ogni texture e aggiungila all'array
	for (const FString& Path : TexturePaths)
	{
		UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *Path);
		if (Texture)
		{
			Textures.Add(Texture);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load texture: %s"), *Path);
		}
	}
}

void UBattleUI::OnHelpButtonClicked()
{
	if (Overlay_Info)
	{
		if (bIsOverlayVisible)
		{
			Overlay_Info->SetVisibility(ESlateVisibility::Hidden);
			bIsOverlayVisible = false;
			Overlay_Info->SetRenderOpacity(0.0);
		}
		else
		{
			Overlay_Info->SetVisibility(ESlateVisibility::Visible);
			bIsOverlayVisible = true;
			Overlay_Info->SetRenderOpacity(1.0);
		}
	}
}

void UBattleUI::OnCloseOverlayButtonClicked()
{
	if (Overlay_Info)
	{
		Overlay_Info->SetVisibility(ESlateVisibility::Hidden);
		bIsOverlayVisible = false;
		Overlay_Info->SetRenderOpacity(0.0);
	}
}

void UBattleUI::OnUnitSelected(ABaseUnit* Unit, bool bIsPlayerUnit)
{
	if (CanvasPanel_UnitInfo)
	{
		if (!Unit)
		{
			CanvasPanel_UnitInfo->SetVisibility(ESlateVisibility::Hidden);
			return;
		}
		
		const bool bIsBrawler = Cast<ABrawlerUnit>(Unit) != nullptr;
		
		FString Belonging = "Unit belongs to: ";
        Belonging += bIsPlayerUnit ? "Player" : "AI";

        FString Type = "Type: ";
        Type += bIsBrawler ? "Brawler" : "Sniper";

        FString AttackType = "Attack Type: ";
        AttackType += Unit->GetAttackType() == EAttackType::Melee ? "Melee" : "Ranged";

        FString MovementRange = "Movement Range: ";
        MovementRange += FString::FromInt(Unit->GetMovementRange()) + " Tiles";

        FString AttackRange = "Attack Range: ";
        AttackRange += FString::FromInt(Unit->GetAttackRange()) + " Tiles";

        FString DamageRange = "Damage Range: from ";
        DamageRange += FString::FromInt(Unit->GetMinDamage()) + " to " + FString::FromInt(Unit->GetMaxDamage());

        FString Remaining = FString::FromInt(Unit->GetCurrentLifePoint());

        FString Max = "/" + FString::FromInt(Unit->GetMaxLifePoint());

        FString Position = "Position: " + Unit->GetPosition();

        if (TextBlock_UnitBelonging) TextBlock_UnitBelonging->SetText(FText::FromString(Belonging));
        if (TextBlock_UnitType) TextBlock_UnitType->SetText(FText::FromString(Type));
        if (TextBlock_AttackType) TextBlock_AttackType->SetText(FText::FromString(AttackType));
        if (TextBlock_MovementRange) TextBlock_MovementRange->SetText(FText::FromString(MovementRange));
        if (TextBlock_AttackRange) TextBlock_AttackRange->SetText(FText::FromString(AttackRange));
        if (TextBlock_DamageRange) TextBlock_DamageRange->SetText(FText::FromString(DamageRange));
        if (TextBlock_Remaining) TextBlock_Remaining->SetText(FText::FromString(Remaining));
        if (TextBlock_Max) TextBlock_Max->SetText(FText::FromString(Max));
        if (TextBlock_Position) TextBlock_Position->SetText(FText::FromString(Position));

        if (ProgressBar_HP)
        {
            float Percent = (float)Unit->GetCurrentLifePoint() / (float)Unit->GetMaxLifePoint();
            ProgressBar_HP->SetPercent(Percent);
        }

		if (Image_UnitSelected)
		{
			const int32 Index = bIsBrawler * 5 + Unit->GetTextureColor();
			Image_UnitSelected->SetBrushFromTexture(Textures[Index].Get(), false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Image_UnitSelected is not valid!"));
		}
		
		CanvasPanel_UnitInfo->SetVisibility(ESlateVisibility::Visible);
		CanvasPanel_UnitInfo->SetRenderOpacity(1.0);
		CanvasPanel_UnitInfo->SetIsEnabled(true);
	}
}

void UBattleUI::OnActionExecuted(const FString Text)
{
	if (ScrollBox_Events)
	{
		// Create a new UTextBlock
		UTextBlock* NewTextBlock = NewObject<UTextBlock>(this);
		NewTextBlock->SetText(FText::FromString(Text));
		NewTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
		NewTextBlock->SetJustification(ETextJustify::Left);
		NewTextBlock->SetAutoWrapText(true);

		// Add the new UTextBlock to the ScrollBox
		ScrollBox_Events->AddChild(NewTextBlock);

		// Scroll to the bottom
		ScrollBox_Events->ScrollToEnd();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ScrollBox_Events is not valid!"));
	}
}

void UBattleUI::OnCanSkipTurn(const bool bCanSkipTurn)
{
	if (Button_NextTurn)
	{
		if (bCanSkipTurn && bIsPlayerTurn) Button_NextTurn->SetIsEnabled(true);
		else Button_NextTurn->SetIsEnabled(false);
	}
}

void UBattleUI::OnTurnSkipped()
{
	if (!bEndSignal) GameMode->GetBattleManager()->OnTurnSkipped();
	else EndTurn();
}

void UBattleUI::OnSwitchTurn(bool NewBIsPlayerTurn)
{
	bIsPlayerTurn = NewBIsPlayerTurn;

	if (TextBlock_Turn)
	{
		bIsPlayerTurn ?
			TextBlock_Turn->SetText(FText::FromString("PLAYER TURN")) :
			TextBlock_Turn->SetText(FText::FromString("AI TURN"));
	}
}

void UBattleUI::OnCanEnd(bool bEnd, bool NewBIsPlayerVictory)
{
	bEndSignal = bEnd;

	if (TextBlock_NextTurn && bEndSignal)
	{
		TextBlock_NextTurn->SetText(FText::FromString("END GAME"));
	}

	bIsPlayerVictory = NewBIsPlayerVictory;

	if (!bIsPlayerTurn && bEndSignal) EndTurn();
}

void UBattleUI::EndTurn()
{
	OnEndSignal.Broadcast(bIsPlayerVictory);
	GameMode->TransitionToPhase(EGamePhase::End);
}
