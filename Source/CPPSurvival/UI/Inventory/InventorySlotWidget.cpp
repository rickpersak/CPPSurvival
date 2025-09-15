#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/ContainerComponent.h"
#include "Data/ItemDataInfo.h"
#include "PlayerControllers/CPPSurvivalPlayerController.h"
#include "Characters/CPPSurvivalCharacter.h"
#include "Framework/Application/SlateApplication.h"
#include "UI/Inventory/InventoryDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"

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
	// Handle shift-click for moving entire stacks
	if (InMouseEvent.IsShiftDown() && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		HandleShiftClick();
		return FReply::Handled();
	}

	// Handle left-click drag detection
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && !CurrentItem.IsEmpty())
	{
		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return FReply::Unhandled();
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!CurrentItem.IsEmpty() && ParentContainer)
	{
		OutOperation = CreateDragDropOperation();
	}
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (UInventoryDragDropOperation* InventoryOperation = Cast<UInventoryDragDropOperation>(InOperation))
	{
		// Don't allow dropping on the same slot
		if (InventoryOperation->SourceContainer == ParentContainer && InventoryOperation->SourceSlotIndex == SlotIndex)
		{
			return false;
		}

		// Get the player controller for the RPC call
		if (ACPPSurvivalPlayerController* PlayerController = Cast<ACPPSurvivalPlayerController>(GetOwningPlayer()))
		{
			PlayerController->Server_MoveItem(
				InventoryOperation->SourceContainer, 
				InventoryOperation->SourceSlotIndex, 
				ParentContainer, 
				SlotIndex
			);
			return true;
		}
	}

	return false;
}

void UInventorySlotWidget::HandleShiftClick()
{
	if (!ParentContainer || CurrentItem.IsEmpty())
	{
		return;
	}

	// Get the player controller for the RPC call
	if (ACPPSurvivalPlayerController* PlayerController = Cast<ACPPSurvivalPlayerController>(GetOwningPlayer()))
	{
		PlayerController->Server_ShiftClickItem(ParentContainer, SlotIndex);
	}
}

UDragDropOperation* UInventorySlotWidget::CreateDragDropOperation()
{
	UInventoryDragDropOperation* DragOperation = NewObject<UInventoryDragDropOperation>();
	if (DragOperation)
	{
		DragOperation->SourceContainer = ParentContainer;
		DragOperation->SourceSlotIndex = SlotIndex;
		DragOperation->SourceSlotWidget = this;
		
		if (CurrentItem.ItemData)
		{
			DragOperation->DraggedItemName = CurrentItem.ItemData->DisplayName.ToString();
		}

		// Always create a simple item icon drag visual
		if (CurrentItem.ItemData && CurrentItem.ItemData->Icon)
		{
			// Create a SizeBox to control the dimensions of the drag visual
			USizeBox* SizeBox = NewObject<USizeBox>(this);
			if (SizeBox)
			{
				SizeBox->SetWidthOverride(DragIconSize.X * DragVisualScale);
				SizeBox->SetHeightOverride(DragIconSize.Y * DragVisualScale);

				// Create the image widget and add it to the SizeBox
				UImage* DragImage = NewObject<UImage>(SizeBox);
				if (DragImage)
				{
					DragImage->SetBrushFromTexture(CurrentItem.ItemData->Icon);
					SizeBox->AddChild(DragImage);
				}

				// Use the SizeBox as the drag visual
				DragOperation->DefaultDragVisual = SizeBox;
			}
		}

		DragOperation->Pivot = EDragPivot::MouseDown;
	}

	return DragOperation;
}