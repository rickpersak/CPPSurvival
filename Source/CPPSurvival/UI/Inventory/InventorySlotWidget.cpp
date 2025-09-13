#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ContainerComponent.h"
#include "Data/ItemDataInfo.h"
#include "PlayerControllers/CPPSurvivalPlayerController.h"
#include "Characters/CPPSurvivalCharacter.h"
#include "Framework/Application/SlateApplication.h"

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

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// Handle shift-clicking for quick transfer
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && FSlateApplication::Get().GetModifierKeys().IsShiftDown())
	{
		// Only handle shift-click if there's actually an item in this slot
		if (!CurrentItem.IsEmpty() && ParentContainer && CurrentItem.ItemData)
		{
			ACPPSurvivalPlayerController* PlayerController = Cast<ACPPSurvivalPlayerController>(GetOwningPlayer());
			if (PlayerController)
			{
				PlayerController->Server_ShiftClickItem(ParentContainer, SlotIndex);
				return FReply::Handled(); // Only consume the event if we actually handled it
			}
		}
	}

	// Allow normal mouse events to proceed (for drag-and-drop)
	return FReply::Unhandled();
}