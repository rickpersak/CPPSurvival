#include "UI/Game/PlayerHUDWidget.h"
#include "UI/Game/InventoryContainerWidget.h"
#include "UI/Inventory/InventoryGridWidget.h"
#include "Components/InventoryComponent.h"
#include "Components/SurvivalStatsComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "Characters/CPPSurvivalCharacter.h"
#include "Blueprint/DragDropOperation.h"
#include "UI/Inventory/InventoryDragDropOperation.h"
#include "Components/InventoryComponent.h"

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InventoryContainerWidget)
	{
		InventoryContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	ACPPSurvivalCharacter* PlayerCharacter = Cast<ACPPSurvivalCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		// Find the SurvivalStatsComponent on the character
		USurvivalStatsComponent* StatsComponent = PlayerCharacter->GetSurvivalStatsComponent();
		if (StatsComponent)
		{
			StatsComponent->OnHungerUpdated.AddDynamic(this, &UPlayerHUDWidget::UpdateHunger);
			StatsComponent->OnThirstUpdated.AddDynamic(this, &UPlayerHUDWidget::UpdateThirst);

			UpdateHunger(StatsComponent->GetCurrentHunger(), StatsComponent->GetMaxHunger());
			UpdateThirst(StatsComponent->GetCurrentThirst(), StatsComponent->GetMaxThirst());
		}

		if (InventoryContainerWidget)
		{
			UInventoryGridWidget* Grid = InventoryContainerWidget->GetInventoryGridWidget();
			UInventoryComponent* Inventory = PlayerCharacter->GetInventoryComponent();

			// If both the grid and the inventory component are valid, initialize it
			if (Grid && Inventory)
			{
				Grid->InitializeGrid(Inventory);
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

void UPlayerHUDWidget::SetInventoryContainerVisibility(bool bIsVisible)
{
	if (InventoryContainerWidget)
	{
		InventoryContainerWidget->SetInventoryContentVisibility(bIsVisible);
	}
}

bool UPlayerHUDWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (UInventoryDragDropOperation* InventoryOperation = Cast<UInventoryDragDropOperation>(InOperation))
	{
		// Cast the generic SourceContainer to the specific InventoryComponent
		if (UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(InventoryOperation->SourceContainer))
		{
			// Get the full quantity of the item stack being dropped
			const int32 QuantityToDrop = InventoryComp->GetItems()[InventoryOperation->SourceSlotIndex].Quantity;

			// Now, call DropItem on the successfully casted InventoryComponent
			InventoryComp->DropItem(InventoryOperation->SourceSlotIndex, QuantityToDrop);
			
			// Return true to indicate we handled the drop successfully
			return true;
		}
	}

	return false;
}