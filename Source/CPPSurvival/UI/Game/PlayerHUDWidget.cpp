#include "UI/Game/PlayerHUDWidget.h"
#include "UI/Game/InventoryContainerWidget.h"
#include "UI/Game/WorldContainerWidget.h"
#include "UI/Inventory/InventoryGridWidget.h"
#include "UI/Game/WorldContainerGridWidget.h"
#include "UI/Inventory/HotbarWidget.h"
#include "Components/InventoryComponent.h"
#include "Components/HotbarComponent.h"
#include "Components/SurvivalStatsComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "Characters/CPPSurvivalCharacter.h"
#include "Blueprint/DragDropOperation.h"
#include "UI/Inventory/InventoryDragDropOperation.h"

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Hide both container widgets by default
	if (InventoryContainerWidget)
	{
		InventoryContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (WorldContainerWidget)
	{
		WorldContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	ACPPSurvivalCharacter* PlayerCharacter = Cast<ACPPSurvivalCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		// --- Initialize Survival Stats ---
		USurvivalStatsComponent* StatsComponent = PlayerCharacter->GetSurvivalStatsComponent();
		if (StatsComponent)
		{
			StatsComponent->OnHungerUpdated.AddDynamic(this, &UPlayerHUDWidget::UpdateHunger);
			StatsComponent->OnThirstUpdated.AddDynamic(this, &UPlayerHUDWidget::UpdateThirst);

			UpdateHunger(StatsComponent->GetCurrentHunger(), StatsComponent->GetMaxHunger());
			UpdateThirst(StatsComponent->GetCurrentThirst(), StatsComponent->GetMaxThirst());
		}
		
		// --- Initialize Player Inventory ---
		if (InventoryContainerWidget)
		{
			UInventoryGridWidget* Grid = InventoryContainerWidget->GetInventoryGridWidget();
			UInventoryComponent* Inventory = PlayerCharacter->GetInventoryComponent();

			if (Grid && Inventory)
			{
				Grid->InitializeGrid(Inventory);
			}
		}
		
		// --- Initialize Hotbar ---
		if(HotbarWidget)
		{
			UHotbarComponent* HotbarComponent = PlayerCharacter->GetHotbarComponent();

			if(HotbarComponent)
			{
				HotbarWidget->InitializeHotbar(HotbarComponent);
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

void UPlayerHUDWidget::OpenWorldContainer(UContainerComponent* ContainerComponent)
{
	if (WorldContainerWidget && ContainerComponent)
	{
		UWorldContainerGridWidget* Grid = WorldContainerWidget->GetContainerGridWidget();
		if (Grid)
		{
			Grid->InitializeGrid(ContainerComponent);
			WorldContainerWidget->SetInventoryContentVisibility(true);
		}
	}
}

void UPlayerHUDWidget::CloseWorldContainer()
{
	if (WorldContainerWidget)
	{
		WorldContainerWidget->SetInventoryContentVisibility(false);
	}
}


bool UPlayerHUDWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (UInventoryDragDropOperation* InventoryOperation = Cast<UInventoryDragDropOperation>(InOperation))
	{
		if (UContainerComponent* ContainerComp = InventoryOperation->SourceContainer)
		{
			const int32 QuantityToDrop = ContainerComp->GetItems()[InventoryOperation->SourceSlotIndex].Quantity;
			
			if (UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(ContainerComp))
			{
				InventoryComp->DropItem(InventoryOperation->SourceSlotIndex, QuantityToDrop);
				return true;
			}
			
			if (UHotbarComponent* HotbarComp = Cast<UHotbarComponent>(ContainerComp))
			{
				HotbarComp->DropItem(InventoryOperation->SourceSlotIndex, QuantityToDrop);
				return true;
			}
		}
	}

	return false;
}