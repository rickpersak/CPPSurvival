#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UInventoryContainerWidget;
class UWorldContainerWidget;
class UHotbarWidget;
class UContainerComponent;

UCLASS()
class CPPSURVIVAL_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	// Shows or hides the main player inventory panel
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetInventoryContainerVisibility(bool bIsVisible);

	// Shows and initializes the world container panel
	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenWorldContainer(UContainerComponent* ContainerComponent);

	// Hides the world container panel
	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseWorldContainer();

protected:
	// This is for the player's main inventory
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UInventoryContainerWidget> InventoryContainerWidget;

	// This is the new widget for the external container (e.g., a chest)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UWorldContainerWidget> WorldContainerWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UHotbarWidget> HotbarWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UProgressBar> HungerBar;
    
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UProgressBar> ThirstBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UTextBlock> HungerText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UTextBlock> ThirstText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UTextBlock> StaminaText;

protected:
	UFUNCTION()
	void UpdateHunger(float CurrentValue, float MaxValue);
	
	UFUNCTION()
	void UpdateThirst(float CurrentValue, float MaxValue);

	UFUNCTION()
	void UpdateStamina(float CurrentValue, float MaxValue);
};