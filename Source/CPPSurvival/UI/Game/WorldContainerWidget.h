#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "WorldContainerWidget.generated.h"

class UBorder;
class UContainerComponent;
class UInventorySlotWidget;
class UUniformGridPanel;
class USizeBox;

/**
 * World Container Widget - Displays a container interface for world containers (chests, etc.)
 * Combines both styling/layout and grid functionality in a single widget
 */
UCLASS()
class CPPSURVIVAL_API UWorldContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Initialize the container with a container component
	UFUNCTION(BlueprintCallable, Category = "Container")
	void InitializeContainer(UContainerComponent* NewContainerComponent);

	// Refresh the container display
	UFUNCTION(BlueprintCallable, Category = "Container")
	void Refresh();

	// Shows or hides the container
	void SetInventoryContentVisibility(bool bIsVisible);

	// Manually update the container size (useful if container capacity changes)
	UFUNCTION(BlueprintCallable, Category = "Container")
	void UpdateContainerSize();

	// Clean up delegate bindings
	UFUNCTION(BlueprintCallable, Category = "Container")
	void CleanupContainer();

protected:
	// Override BeginDestroy to ensure proper cleanup
	virtual void BeginDestroy() override;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UBorder> ContentBorder;

	// Optional SizeBox for explicit size control (bind this if you want dynamic sizing)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "UI")
	TObjectPtr<USizeBox> ContainerSizeBox;

	// The uniform grid panel that holds the slot widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UUniformGridPanel> ItemGrid;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "UI")
	TObjectPtr<UTextBlock> ContainerNameText;

	// The container component this widget displays
	UPROPERTY(BlueprintReadOnly, Category = "Container")
	TObjectPtr<UContainerComponent> ContainerComponent;

	// The widget class to use for container slots
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	// Number of columns in the grid
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (ClampMin = "1"))
	int32 Columns = 6;

	// Size of each inventory slot (used for dynamic resizing)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FVector2D SlotSize = FVector2D(64.0f, 64.0f);

	// Padding around the grid content
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FVector2D ContentPadding = FVector2D(20.0f, 20.0f);

	// Additional height for container name/header
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float HeaderHeight = 40.0f;

	// Spacing between slots in the grid
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FVector2D SlotSpacing = FVector2D(2.0f, 2.0f);

protected:

public:
	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Container")
	UContainerComponent* GetContainerComponent() const { return ContainerComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	UUniformGridPanel* GetItemGrid() const { return ItemGrid; }
};
