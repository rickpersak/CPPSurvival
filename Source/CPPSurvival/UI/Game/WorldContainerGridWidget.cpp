#include "UI/Game/WorldContainerGridWidget.h"
#include "Components/ContainerComponent.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Data/ItemDataInfo.h"
#include "CPPSurvival.h"

void UWorldContainerGridWidget::InitializeGrid(UContainerComponent* NewContainerComponent)
{
	UE_LOG(LogCPPSurvival, Warning, TEXT("WorldContainerGridWidget::InitializeGrid called"));
	UE_LOG(LogCPPSurvival, Warning, TEXT("  - NewContainerComponent: %s"), *GetNameSafe(NewContainerComponent));
	UE_LOG(LogCPPSurvival, Warning, TEXT("  - SlotWidgetClass: %s"), *GetNameSafe(SlotWidgetClass));
	UE_LOG(LogCPPSurvival, Warning, TEXT("  - ItemGrid: %s"), *GetNameSafe(ItemGrid));
	
	if (!NewContainerComponent)
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("InitializeGrid: NewContainerComponent is null!"));
		return;
	}
	
	if (!SlotWidgetClass)
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("InitializeGrid: SlotWidgetClass is null! Make sure it's set in the Blueprint."));
		return;
	}
	
	if (!ItemGrid)
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("InitializeGrid: ItemGrid is null! Make sure the widget is bound in Blueprint."));
		return;
	}

	ContainerComponent = NewContainerComponent;

	// Bind our Refresh function to the container's update delegate.
	// Now, whenever the container is modified, our UI will automatically update.
	ContainerComponent->OnContainerUpdated.AddDynamic(this, &UWorldContainerGridWidget::Refresh);

	// Check if the optional ContainerNameText widget exists
	if (ContainerNameText)
	{
		FText ContainerName = ContainerComponent->GetContainerName();
		UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeGrid: ContainerNameText exists, Container name: %s"), *ContainerName.ToString());
		UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeGrid: Container class: %s"), *ContainerComponent->GetClass()->GetName());
		
		// Only show the name for base UContainerComponent (i.e., storage chests)
		// and not for derived classes like UInventoryComponent or UHotbarComponent.
		if (ContainerComponent->GetClass() == UContainerComponent::StaticClass())
		{
			ContainerNameText->SetText(ContainerName);
			ContainerNameText->SetVisibility(ESlateVisibility::HitTestInvisible);
			UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeGrid: Container name set and made visible"));
		}
		else
		{
			// Hide the name text for other container types.
			ContainerNameText->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeGrid: Container name hidden (not base UContainerComponent)"));
		}
	}
	else
	{
		UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeGrid: ContainerNameText is null (optional widget)"));
	}

	// Create the initial set of empty slots.
	ItemGrid->ClearChildren();

	const int32 ContainerCapacity = ContainerComponent->GetCapacity();
	UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeGrid: Container capacity is %d, creating %d slots"), ContainerCapacity, ContainerCapacity);
	
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
			
			if (i == 0) // Log only the first slot to avoid spam
			{
				UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeGrid: Created slot %d at Row %d, Column %d"), i, Row, Column);
			}
		}
		else
		{
			UE_LOG(LogCPPSurvival, Error, TEXT("InitializeGrid: Failed to create SlotWidget for slot %d"), i);
		}
	}
	
	UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeGrid: Total slots created: %d, calling Refresh()"), ItemGrid->GetChildrenCount());
	// Perform an initial refresh to populate with current data.
	Refresh();
}

void UWorldContainerGridWidget::Refresh()
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
				SlotWidget->Refresh(FContainerItem());
			}
		}
	}
}
