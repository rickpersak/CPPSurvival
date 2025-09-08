// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGridWidget.generated.h"

class UContainerComponent;
class UInventorySlotWidget;
class UUniformGridPanel;

/**
 * Inventory Grid Widget - Displays a grid of inventory slots
 */
UCLASS()
class CPPSURVIVAL_API UInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Initialize the grid with a container component
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeGrid(UContainerComponent* NewContainerComponent);

	// Refresh the grid display
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Refresh();

protected:
	// The uniform grid panel that holds the slot widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UUniformGridPanel> ItemGrid;

	// The container component this grid displays
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UContainerComponent> ContainerComponent;

	// The widget class to use for inventory slots
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	// Number of columns in the grid
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (ClampMin = "1"))
	int32 Columns = 6;

public:
	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	UContainerComponent* GetContainerComponent() const { return ContainerComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	UUniformGridPanel* GetItemGrid() const { return ItemGrid; }
};
