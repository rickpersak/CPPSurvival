// PlayerHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UInventoryContainerWidget;
class UHotbarWidget;

UCLASS()
class CPPSURVIVAL_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UInventoryContainerWidget> InventoryContainerWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UHotbarWidget> HotbarWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UProgressBar> HungerBar;
    
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UProgressBar> ThirstBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UTextBlock> HungerText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UTextBlock> ThirstText;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetInventoryContainerVisibility(bool bIsVisible);

protected:
	UFUNCTION()
	void UpdateHunger(float CurrentValue, float MaxValue);
	
	UFUNCTION()
	void UpdateThirst(float CurrentValue, float MaxValue);
};
