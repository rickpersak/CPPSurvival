#include "UI/Game/WorldContainerWidget.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Components/ContainerComponent.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Data/ItemDataInfo.h"
#include "CPPSurvival.h"

void UWorldContainerWidget::SetInventoryContentVisibility(bool bIsVisible)
{
	SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UWorldContainerWidget::InitializeContainer(UContainerComponent* NewContainerComponent)
{
	UE_LOG(LogCPPSurvival, Warning, TEXT("WorldContainerWidget::InitializeContainer called"));
	UE_LOG(LogCPPSurvival, Warning, TEXT("  - NewContainerComponent: %s"), *GetNameSafe(NewContainerComponent));
	UE_LOG(LogCPPSurvival, Warning, TEXT("  - SlotWidgetClass: %s"), *GetNameSafe(SlotWidgetClass));
	UE_LOG(LogCPPSurvival, Warning, TEXT("  - ItemGrid: %s"), *GetNameSafe(ItemGrid));
	
	if (!NewContainerComponent)
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("InitializeContainer: NewContainerComponent is null!"));
		return;
	}
	
	if (!SlotWidgetClass)
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("InitializeContainer: SlotWidgetClass is null! Make sure it's set in the Blueprint."));
		return;
	}
	
	if (!ItemGrid)
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("InitializeContainer: ItemGrid is null! Make sure the widget is bound in Blueprint."));
		return;
	}

	ContainerComponent = NewContainerComponent;

	// Bind our Refresh function to the container's update delegate.
	// Now, whenever the container is modified, our UI will automatically update.
	ContainerComponent->OnContainerUpdated.AddDynamic(this, &UWorldContainerWidget::Refresh);

	// Check if the optional ContainerNameText widget exists
	if (ContainerNameText)
	{
		FText ContainerName = ContainerComponent->GetContainerName();
		UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeContainer: ContainerNameText exists, Container name: %s"), *ContainerName.ToString());
		UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeContainer: Container class: %s"), *ContainerComponent->GetClass()->GetName());
		
		// Only show the name for base UContainerComponent (i.e., storage chests)
		// and not for derived classes like UInventoryComponent or UHotbarComponent.
		if (ContainerComponent->GetClass() == UContainerComponent::StaticClass())
		{
			ContainerNameText->SetText(ContainerName);
			ContainerNameText->SetVisibility(ESlateVisibility::HitTestInvisible);
			UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeContainer: Container name set and made visible"));
		}
		else
		{
			// Hide the name text for other container types.
			ContainerNameText->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeContainer: Container name hidden (not base UContainerComponent)"));
		}
	}
	else
	{
		UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeContainer: ContainerNameText is null (optional widget)"));
	}

	// Create the initial set of empty slots.
	ItemGrid->ClearChildren();

	const int32 ContainerCapacity = ContainerComponent->GetCapacity();
	UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeContainer: Container capacity is %d, creating %d slots"), ContainerCapacity, ContainerCapacity);
	
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
			
			// Set fixed size for each slot instead of filling
			GridSlot->SetHorizontalAlignment(HAlign_Center);
			GridSlot->SetVerticalAlignment(VAlign_Center);
			
			// Force each slot to be exactly the slot size
			if (SlotWidget)
			{
				SlotWidget->SetDesiredSizeInViewport(SlotSize);
			}
			
			if (i == 0) // Log only the first slot to avoid spam
			{
				UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeContainer: Created slot %d at Row %d, Column %d"), i, Row, Column);
			}
		}
		else
		{
			UE_LOG(LogCPPSurvival, Error, TEXT("InitializeContainer: Failed to create SlotWidget for slot %d"), i);
		}
	}
	
	UE_LOG(LogCPPSurvival, Warning, TEXT("InitializeContainer: Total slots created: %d, calling Refresh()"), ItemGrid->GetChildrenCount());
	
	// Update the container size to fit the content
	UpdateContainerSize();
	
	// Perform an initial refresh to populate with current data.
	Refresh();
}

void UWorldContainerWidget::Refresh()
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

void UWorldContainerWidget::UpdateContainerSize()
{
	if (!ContainerComponent)
	{
		UE_LOG(LogCPPSurvival, Warning, TEXT("UpdateContainerSize: ContainerComponent is null"));
		return;
	}

	const int32 ContainerCapacity = ContainerComponent->GetCapacity();
	
	// Calculate the number of rows needed
	const int32 Rows = (ContainerCapacity + Columns - 1) / Columns; // Ceiling division
	
	// Calculate required dimensions including spacing between slots
	const float RequiredWidth = (Columns * SlotSize.X) + ((Columns - 1) * SlotSpacing.X) + (ContentPadding.X * 2);
	
	// Add header height if container name is visible
	float RequiredHeight = (Rows * SlotSize.Y) + ((Rows - 1) * SlotSpacing.Y) + (ContentPadding.Y * 2);
	if (ContainerNameText && ContainerNameText->GetVisibility() != ESlateVisibility::Collapsed)
	{
		RequiredHeight += HeaderHeight;
	}

	UE_LOG(LogCPPSurvival, Warning, TEXT("UpdateContainerSize: Capacity=%d, Rows=%d, Columns=%d"), 
		ContainerCapacity, Rows, Columns);
	UE_LOG(LogCPPSurvival, Warning, TEXT("UpdateContainerSize: Calculated size: %fx%f"), 
		RequiredWidth, RequiredHeight);

	// Method 1: Try to use SizeBox if it exists (this is the most reliable method)
	if (ContainerSizeBox)
	{
		UE_LOG(LogCPPSurvival, Warning, TEXT("UpdateContainerSize: Using SizeBox to set exact size"));
		
		// Now that slots are fixed size, we can set the exact container size
		ContainerSizeBox->SetWidthOverride(RequiredWidth);
		ContainerSizeBox->SetHeightOverride(RequiredHeight);
		
		UE_LOG(LogCPPSurvival, Warning, TEXT("UpdateContainerSize: Set container size to %fx%f"), RequiredWidth, RequiredHeight);
		
		return;
	}

	// Method 2: If no SizeBox, log a warning that dynamic sizing won't work properly
	UE_LOG(LogCPPSurvival, Warning, TEXT("UpdateContainerSize: No SizeBox found! Add a SizeBox named 'ContainerSizeBox' to your widget hierarchy for proper dynamic sizing."));
	UE_LOG(LogCPPSurvival, Warning, TEXT("UpdateContainerSize: Widget hierarchy should be: Root -> ContainerSizeBox -> ContentBorder -> [rest]"));

	// Method 3: Force layout invalidation
	InvalidateLayoutAndVolatility();
	if (GetParent())
	{
		GetParent()->InvalidateLayoutAndVolatility();
	}
	
	UE_LOG(LogCPPSurvival, Warning, TEXT("UpdateContainerSize: Completed size update"));
}
