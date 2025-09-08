#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ContainerComponent.h"
#include "Data/ItemDataInfo.h"

void UInventorySlotWidget::Refresh(const FContainerItem& ItemData)
{
	// Store the current item data
	CurrentItem = ItemData;

	if (ItemData.IsEmpty())
	{
		// Clear the slot if it's empty
		if (ItemImage)
		{
			ItemImage->SetBrushFromTexture(nullptr);
			ItemImage->SetVisibility(ESlateVisibility::Hidden);
		}
		
		if (QuantityText)
		{
			QuantityText->SetText(FText::GetEmpty());
			QuantityText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		// Update the slot with item data
		if (ItemImage && ItemData.ItemData && ItemData.ItemData->Icon)
		{
			ItemImage->SetBrushFromTexture(ItemData.ItemData->Icon);
			ItemImage->SetVisibility(ESlateVisibility::Visible);
		}
		
		if (QuantityText)
		{
			if (ItemData.Quantity > 1)
			{
				// Only show quantity text if there's more than 1 item
				QuantityText->SetText(FText::AsNumber(ItemData.Quantity));
				QuantityText->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				// Hide quantity text for single items
				QuantityText->SetText(FText::GetEmpty());
				QuantityText->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}
