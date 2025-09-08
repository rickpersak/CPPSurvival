#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UInventoryGridWidget;
class UProgressBar; 
class UTextBlock; 

/**
 * Player HUD Widget - The main UI widget for the player
 */
UCLASS()
class CPPSURVIVAL_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	// Reference to the inventory grid widget
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UInventoryGridWidget> InventoryGridWidget;

	// Add pointers for your new stat display widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UProgressBar> HungerBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UProgressBar> ThirstBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UTextBlock> HungerText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UTextBlock> ThirstText;

public:
	// Set the visibility of the inventory grid
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetInventoryGridVisibility(bool bIsVisible);

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	UInventoryGridWidget* GetInventoryGridWidget() const { return InventoryGridWidget; }

protected:
	// New functions to handle the stat updates
	UFUNCTION()
	void UpdateHunger(float CurrentValue, float MaxValue);
	
	UFUNCTION()
	void UpdateThirst(float CurrentValue, float MaxValue);
};