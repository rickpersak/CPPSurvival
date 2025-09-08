// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Inventory/HotbarWidget.h"
#include "Components/ContainerComponent.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Data/ItemDataInfo.h"

void UHotbarWidget::InitializeHotbar(UContainerComponent* NewContainerComponent)
{
	if (!NewContainerComponent || !SlotWidgetClass || !HotbarGrid)
	{
		return;
	}

	ContainerComponent = NewContainerComponent;

	// Bind our Refresh function to the container's update delegate.
	ContainerComponent->OnContainerUpdated.AddDynamic(this, &UHotbarWidget::Refresh);

	// Create the hotbar slots (horizontal layout)
	HotbarGrid->ClearChildren();

	// Create slots up to the configured hotbar size or container capacity (whichever is smaller)
	const int32 ContainerCapacity = ContainerComponent->GetCapacity();
	const int32 SlotsToCreate = FMath::Min(HotbarSlots, ContainerCapacity);
	
	for (int32 i = 0; i < SlotsToCreate; ++i)
	{
		UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
		if (SlotWidget)
		{
			SlotWidget->SetSlotIndex(i);
			SlotWidget->SetParentContainer(ContainerComponent);
			
			// Add slots horizontally (all in row 0, different columns)
			UUniformGridSlot* GridSlot = HotbarGrid->AddChildToUniformGrid(SlotWidget, 0, i);
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
	
	// Perform an initial refresh to populate with current data
	Refresh();
}

void UHotbarWidget::Refresh()
{
	if (!ContainerComponent || !HotbarGrid)
	{
		return;
	}

	const TArray<FContainerItem>& Items = ContainerComponent->GetItems();

	// Iterate through all the child widgets of the hotbar grid
	for (int32 i = 0; i < HotbarGrid->GetChildrenCount(); ++i)
	{
		UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(HotbarGrid->GetChildAt(i));
		if (SlotWidget)
		{
			// If there's a valid item at this index, refresh the slot with it
			if (Items.IsValidIndex(i))
			{
				SlotWidget->Refresh(Items[i]);
			}
			else
			{
				// Otherwise, refresh the slot with an empty item to clear it
				SlotWidget->Refresh(FContainerItem());
			}
		}
	}
}
