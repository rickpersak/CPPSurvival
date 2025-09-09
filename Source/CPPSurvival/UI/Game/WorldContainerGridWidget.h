#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "WorldContainerGridWidget.generated.h"

class UContainerComponent;
class UInventorySlotWidget;
class UUniformGridPanel;

/**
 * World Container Grid Widget - Displays a grid of container slots for world containers (chests, etc.)
 * Same functionality as InventoryGridWidget but allows for different visual styling
 */
UCLASS()
class CPPSURVIVAL_API UWorldContainerGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Initialize the grid with a container component
	UFUNCTION(BlueprintCallable, Category = "Container")
	void InitializeGrid(UContainerComponent* NewContainerComponent);

	// Refresh the grid display
	UFUNCTION(BlueprintCallable, Category = "Container")
	void Refresh();

protected:
	// The uniform grid panel that holds the slot widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UUniformGridPanel> ItemGrid;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "UI")
	TObjectPtr<UTextBlock> ContainerNameText;

	// The container component this grid displays
	UPROPERTY(BlueprintReadOnly, Category = "Container")
	TObjectPtr<UContainerComponent> ContainerComponent;

	// The widget class to use for container slots
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	// Number of columns in the grid
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (ClampMin = "1"))
	int32 Columns = 6;

public:
	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Container")
	UContainerComponent* GetContainerComponent() const { return ContainerComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	UUniformGridPanel* GetItemGrid() const { return ItemGrid; }
};
