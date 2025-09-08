// PlayerHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UInventoryContainerWidget; // Forward declare the new container

UCLASS()
class CPPSURVIVAL_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
	// This should be your new container widget
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UInventoryContainerWidget> InventoryContainerWidget;

	// Stat UI elements...
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UProgressBar> HungerBar;
    
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UProgressBar> ThirstBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UTextBlock> HungerText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UTextBlock> ThirstText;

public:
	// This is the new function name
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetInventoryContainerVisibility(bool bIsVisible);

protected:
	UFUNCTION()
	void UpdateHunger(float CurrentValue, float MaxValue);
	
	UFUNCTION()
	void UpdateThirst(float CurrentValue, float MaxValue);
};