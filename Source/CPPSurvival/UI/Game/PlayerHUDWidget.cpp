#include "UI/Game/PlayerHUDWidget.h"
#include "UI/Inventory/InventoryGridWidget.h"
#include "Components/InventoryComponent.h"
#include "Components/SurvivalStatsComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "Characters/CPPSurvivalCharacter.h"

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// --- Inventory Initialization (Existing Code) ---
	if (InventoryGridWidget)
	{
		InventoryGridWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	// We need a valid player pawn to find the stats component
	ACPPSurvivalCharacter* PlayerCharacter = Cast<ACPPSurvivalCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		// Find the SurvivalStatsComponent on the character
		USurvivalStatsComponent* StatsComponent = PlayerCharacter->GetSurvivalStatsComponent();
		if (StatsComponent)
		{
			// Bind our new functions to the delegates from the component
			StatsComponent->OnHungerUpdated.AddDynamic(this, &UPlayerHUDWidget::UpdateHunger);
			StatsComponent->OnThirstUpdated.AddDynamic(this, &UPlayerHUDWidget::UpdateThirst);

			// Call the functions once immediately to set the initial HUD state
			UpdateHunger(StatsComponent->GetCurrentHunger(), StatsComponent->GetMaxHunger());
			UpdateThirst(StatsComponent->GetCurrentThirst(), StatsComponent->GetMaxThirst());
		}
		
		if (InventoryGridWidget)
		{
			UInventoryComponent* Inventory = PlayerCharacter->GetInventoryComponent();
			if (Inventory)
			{
				InventoryGridWidget->InitializeGrid(Inventory);
			}
		}
	}
}

void UPlayerHUDWidget::UpdateHunger(float CurrentValue, float MaxValue)
{
	if (HungerBar)
	{
		HungerBar->SetPercent(CurrentValue / MaxValue);
	}
	if (HungerText)
	{
		// Format the text to show "Current / Max"
		FText HungerValueText = FText::FromString(FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(CurrentValue), FMath::RoundToInt(MaxValue)));
		HungerText->SetText(HungerValueText);
	}
}

void UPlayerHUDWidget::UpdateThirst(float CurrentValue, float MaxValue)
{
	if (ThirstBar)
	{
		ThirstBar->SetPercent(CurrentValue / MaxValue);
	}
	if (ThirstText)
	{
		FText ThirstValueText = FText::FromString(FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(CurrentValue), FMath::RoundToInt(MaxValue)));
		ThirstText->SetText(ThirstValueText);
	}
}

void UPlayerHUDWidget::SetInventoryGridVisibility(bool bIsVisible)
{
	if (InventoryGridWidget)
	{
		const ESlateVisibility NewVisibility = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
		InventoryGridWidget->SetVisibility(NewVisibility);
	}
}