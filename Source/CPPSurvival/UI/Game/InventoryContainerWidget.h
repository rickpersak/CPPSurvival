#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryContainerWidget.generated.h"

class UBackgroundBlur;
class UBorder;
class UInventoryGridWidget; // Forward declare the grid widget

UCLASS()
class CPPSURVIVAL_API UInventoryContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetInventoryContentVisibility(bool bIsVisible);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	UInventoryGridWidget* GetInventoryGridWidget() const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UBackgroundBlur> BackgroundBlurPanel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UBorder> ContentBorder;

	// New pointer to the grid widget that will be inside this container
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UInventoryGridWidget> InventoryGridWidget;
};