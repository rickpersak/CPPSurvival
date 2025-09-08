#include "UI/Inventory/InventoryGridWidget.h"
#include "Components/ContainerComponent.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Data/ItemDataInfo.h"

void UInventoryGridWidget::InitializeGrid(UContainerComponent* NewContainerComponent)
{
	if (!NewContainerComponent || !SlotWidgetClass || !ItemGrid)
	{
		return;
	}

	ContainerComponent = NewContainerComponent;

	// Bind our Refresh function to the container's update delegate.
	// Now, whenever the container is modified, our UI will automatically update.
	ContainerComponent->OnContainerUpdated.AddDynamic(this, &UInventoryGridWidget::Refresh);

	// Create the initial set of empty slots.
	ItemGrid->ClearChildren();

	const int32 ContainerCapacity = ContainerComponent->GetCapacity();
	
	for (int32 i = 0; i < ContainerCapacity; ++i)
	{
		UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
		if (SlotWidget)
		{
			SlotWidget->SetSlotIndex(i);
			SlotWidget->SetParentContainer(ContainerComponent);
			const int32 Row = i / Columns;
			const int32 Column = i % Columns;
			UUniformGridSlot* GridSlot = ItemGrid->AddChildToUniformGrid(SlotWidget, Row, Column);
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
	// Perform an initial refresh to populate with current data.
	Refresh();
}

void UInventoryGridWidget::Refresh()
{
	if (!ContainerComponent || !ItemGrid)
	{
		return;
	}

	const TArray<FContainerItem>& Items = ContainerComponent->GetItems();

	// Iterate through all the child widgets of the grid.
	for (int32 i = 0; i < ItemGrid->GetChildrenCount(); ++i)
	{
		UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(ItemGrid->GetChildAt(i));
		if (SlotWidget)
		{
			// If there's a valid item at this index, refresh the slot with it.
			if (Items.IsValidIndex(i))
			{
				SlotWidget->Refresh(Items[i]);
			}
			else
			{
				// Otherwise, refresh the slot with an empty item to clear it.
				// This is the crucial part that was missing.
				SlotWidget->Refresh(FContainerItem());
			}
		}
	}
}