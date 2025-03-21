#include "UI/PlacementUI.h"

#include "Game/StrategyGameMode.h"
#include "Game/Managers/PlacementManager.h"
#include "Components/Button.h"

void UPlacementUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (Sniper_Button)
	{
		Sniper_Button->OnClicked.RemoveDynamic(this, &UPlacementUI::OnSniperButtonClicked);
		Sniper_Button->OnClicked.AddDynamic(this, &UPlacementUI::OnSniperButtonClicked);

		Sniper_Button->SetIsEnabled(true);
	}

	if (Brawler_Button)
	{
		Brawler_Button->OnClicked.RemoveDynamic(this, &UPlacementUI::OnBrawlerButtonClicked);
		Brawler_Button->OnClicked.AddDynamic(this, &UPlacementUI::OnBrawlerButtonClicked);

		Brawler_Button->SetIsEnabled(true);
	}

	if (CanvasPanel_R)
	{
		CanvasPanel_R->SetIsEnabled(false); CanvasPanel_R->SetRenderOpacity(0);
	}
}

void UPlacementUI::Init(TWeakObjectPtr<AStrategyGameMode> NewGameMode)
{
	GameMode = NewGameMode;

	if (GameMode.IsValid() && GameMode->GetPlacementManager())
	{
		GameMode->GetPlacementManager()->OnUnitPlaced.RemoveDynamic(this, &UPlacementUI::OnUnitPlaced);
		GameMode->GetPlacementManager()->OnUnitPlaced.AddDynamic(this, &UPlacementUI::OnUnitPlaced);
	}

	TArray<FString> TexturePaths = {
		TEXT("/Game/Textures/Units/Soldier1_Blue.Soldier1_Blue"),
		/*TEXT("/Game/Textures/Units/Soldier1_Brown.Soldier1_Brown"),
		TEXT("/Game/Textures/Units/Soldier1_Green.Soldier1_Green"),
		TEXT("/Game/Textures/Units/Soldier1_Red.Soldier1_Red"),
		TEXT("/Game/Textures/Units/Soldier1_Violet.Soldier1_Violet"),*/
		TEXT("/Game/Textures/Units/Soldier2_Blue.Soldier2_Blue"),
		/*TEXT("/Game/Textures/Units/Soldier2_Brown.Soldier2_Brown"),
		TEXT("/Game/Textures/Units/Soldier2_Green.Soldier2_Green"),
		TEXT("/Game/Textures/Units/Soldier2_Red.Soldier2_Red"),
		TEXT("/Game/Textures/Units/Soldier2_Violet.Soldier2_Violet")*/
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

void UPlacementUI::OnSniperButtonClicked()
{
	if (GameMode.IsValid() && GameMode->GetPlacementManager())
	{
		GameMode->GetPlacementManager()->SetUnitToPlace(EUnitTypes::Sniper);

		if (CanvasPanel_R)
		{
			CanvasPanel_R->SetIsEnabled(true); CanvasPanel_R->SetRenderOpacity(1);
		}
		
		const FString Type = "Type: Sniper";
		const FString AttackType = "Attack Type: Ranged";
		const FString MovementRange = "Movement Range: " + FString::FromInt(3) + " Tiles";
		const FString AttackRange = "Attack Range: " + FString::FromInt(10) + " Tiles";
		const FString DamageRange = "Damage Range: from " + FString::FromInt(4) + " to " + FString::FromInt(8);
		const FString Remaining = FString::FromInt(20);
		const FString Max = "/" + FString::FromInt(20);

		if (TextBlock_UnitType) TextBlock_UnitType->SetText(FText::FromString(Type));
		if (TextBlock_AttackType) TextBlock_AttackType->SetText(FText::FromString(AttackType));
		if (TextBlock_MovementRange) TextBlock_MovementRange->SetText(FText::FromString(MovementRange));
		if (TextBlock_AttackRange) TextBlock_AttackRange->SetText(FText::FromString(AttackRange));
		if (TextBlock_DamageRange) TextBlock_DamageRange->SetText(FText::FromString(DamageRange));
		if (TextBlock_Remaining) TextBlock_Remaining->SetText(FText::FromString(Remaining));
		if (TextBlock_Max) TextBlock_Max->SetText(FText::FromString(Max));

		if (Image_UnitSelected)
		{
			Image_UnitSelected->SetBrushFromTexture(Textures[0].Get(), false);
		}
	}
}

void UPlacementUI::OnBrawlerButtonClicked()
{
	if (GameMode.IsValid() && GameMode->GetPlacementManager())
	{
		GameMode->GetPlacementManager()->SetUnitToPlace(EUnitTypes::Brawler);

		if (CanvasPanel_R)
		{
			CanvasPanel_R->SetIsEnabled(true); CanvasPanel_R->SetRenderOpacity(1);
		}
		
		const FString Type = "Type: Brawler";
		const FString AttackType = "Attack Type: Melee";
		const FString MovementRange = "Movement Range: " + FString::FromInt(6) + " Tiles";
		const FString AttackRange = "Attack Range: " + FString::FromInt(1) + " Tiles";
		const FString DamageRange = "Damage Range: from " + FString::FromInt(1) + " to " + FString::FromInt(6);
		const FString Remaining = FString::FromInt(40);
		const FString Max = "/" + FString::FromInt(40);

		if (TextBlock_UnitType) TextBlock_UnitType->SetText(FText::FromString(Type));
		if (TextBlock_AttackType) TextBlock_AttackType->SetText(FText::FromString(AttackType));
		if (TextBlock_MovementRange) TextBlock_MovementRange->SetText(FText::FromString(MovementRange));
		if (TextBlock_AttackRange) TextBlock_AttackRange->SetText(FText::FromString(AttackRange));
		if (TextBlock_DamageRange) TextBlock_DamageRange->SetText(FText::FromString(DamageRange));
		if (TextBlock_Remaining) TextBlock_Remaining->SetText(FText::FromString(Remaining));
		if (TextBlock_Max) TextBlock_Max->SetText(FText::FromString(Max));

		if (Image_UnitSelected)
		{
			Image_UnitSelected->SetBrushFromTexture(Textures[1].Get(), false);
		}
	}
}

void UPlacementUI::OnUnitPlaced(EUnitTypes UnitType)
{
	switch (UnitType)
	{
	case EUnitTypes::Sniper:
		if (Sniper_Button && CanvasPanel_R)
		{
			Sniper_Button->SetIsEnabled(false);
			CanvasPanel_R->SetIsEnabled(false); CanvasPanel_R->SetRenderOpacity(0);
		}
		break;

	case EUnitTypes::Brawler:
		if (Brawler_Button && CanvasPanel_R)
		{
			Brawler_Button->SetIsEnabled(false);
			CanvasPanel_R->SetIsEnabled(false); CanvasPanel_R->SetRenderOpacity(0);
		}
		break;

	default:
		break;
	}
}