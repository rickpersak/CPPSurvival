// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/ContainerItem.h"
#include "InventorySlotWidget.generated.h"

class UContainerComponent;
class UImage;
class UTextBlock;

/**
 * Inventory Slot Widget - Represents a single slot in an inventory grid
 */
UCLASS()
class CPPSURVIVAL_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Refresh the slot display with new item data
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Refresh(const FContainerItem& ItemData);

	// Set the slot index
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetSlotIndex(int32 NewSlotIndex) { SlotIndex = NewSlotIndex; }

	// Set the parent container
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetParentContainer(UContainerComponent* NewParentContainer) { ParentContainer = NewParentContainer; }

protected:
	// UI components
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UImage> ItemImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UTextBlock> QuantityText;

	// The slot index in the container
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 SlotIndex = -1;

	// Reference to the parent container
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UContainerComponent> ParentContainer;

	// Current item data
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FContainerItem CurrentItem;

public:
	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetSlotIndex() const { return SlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	UContainerComponent* GetParentContainer() const { return ParentContainer; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	const FContainerItem& GetCurrentItem() const { return CurrentItem; }

protected:
	// Mouse event overrides for drag and drop functionality
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	// Helper function to handle shift-click functionality
	void HandleShiftClick();

	// Helper function to create drag operation
	UDragDropOperation* CreateDragDropOperation();

	// Size of the drag icon in pixels
	UPROPERTY(EditAnywhere, Category = "Drag Drop", meta = (ClampMin = "16.0", ClampMax = "256.0"))
	FVector2D DragIconSize = FVector2D(64.0f, 64.0f);

	// Scale multiplier for the drag visual (1.0 = same size as slot, 1.2 = 20% larger)
	UPROPERTY(EditAnywhere, Category = "Drag Drop", meta = (ClampMin = "0.5", ClampMax = "3.0"))
	float DragVisualScale = 1.2f;
};