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
};
