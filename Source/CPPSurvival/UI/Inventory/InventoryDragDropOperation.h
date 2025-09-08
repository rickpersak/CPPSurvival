// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryDragDropOperation.generated.h"

class UContainerComponent;
class UInventorySlotWidget;

/**
 * Inventory Drag Drop Operation - Handles dragging items in the inventory
 */
UCLASS()
class CPPSURVIVAL_API UInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	// The container that the item is being dragged from
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	TObjectPtr<UContainerComponent> SourceContainer;

	// The slot index of the item being dragged
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	int32 SourceSlotIndex = -1;

	// Reference to the slot widget that initiated the drag
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	TObjectPtr<UInventorySlotWidget> SourceSlotWidget;

	// Payload data for the drag operation
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	FString DraggedItemName;
};
