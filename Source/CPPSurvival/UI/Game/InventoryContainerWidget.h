// InventoryContainerWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryContainerWidget.generated.h"

class UBackgroundBlur; 
class UBorder;         
class UWidgetSwitcher; 
class UCanvasPanel;   

/**
 * Widget to contain the inventory, providing a background blur and border.
 */
UCLASS()
class CPPSURVIVAL_API UInventoryContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Set the visibility of the internal inventory grid.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetInventoryContentVisibility(bool bIsVisible);

protected:
	// The BackgroundBlur widget defined in the UMG Blueprint
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UBackgroundBlur> BackgroundBlurPanel;

	// The Border widget defined in the UMG Blueprint
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UBorder> ContentBorder;

	// This is a placeholder, you might use a UCanvasPanel or UWidgetSwitcher
	// to dynamically add your UInventoryGridWidget to this container.
	// For now, let's assume we'll simply place the InventoryGridWidget
	// directly into the Blueprint after creating it.
	// If you want to add/remove it dynamically, you might need a UPanelWidget slot.
	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	// TObjectPtr<UPanelWidget> InventoryContentSlot; // e.g., a UCanvasPanel or UScaleBox

	
};