// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HotbarWidget.generated.h"

class UContainerComponent;
class UInventorySlotWidget;
class UUniformGridPanel;

/**
 * Widget that displays a horizontal hotbar with inventory slots.
 */
UCLASS()
class CPPSURVIVAL_API UHotbarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Initialize the hotbar with a container component
	UFUNCTION(BlueprintCallable, Category = "Hotbar")
	void InitializeHotbar(UContainerComponent* NewContainerComponent);

	// Refresh the hotbar display
	UFUNCTION(BlueprintCallable, Category = "Hotbar")
	void Refresh();

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Hotbar")
	UContainerComponent* GetContainerComponent() const { return ContainerComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	UUniformGridPanel* GetHotbarGrid() const { return HotbarGrid; }

protected:
	// The uniform grid panel that holds the hotbar slot widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UUniformGridPanel> HotbarGrid;

	// The container component this hotbar displays
	UPROPERTY(BlueprintReadOnly, Category = "Hotbar")
	TObjectPtr<UContainerComponent> ContainerComponent;

	// The widget class to use for hotbar slots
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	// Number of slots in the hotbar (typically 10 or 12)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (ClampMin = "1"))
	int32 HotbarSlots = 10;
};
