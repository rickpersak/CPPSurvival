#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldContainerWidget.generated.h"

class UBorder;
class UWorldContainerGridWidget;

UCLASS()
class CPPSURVIVAL_API UWorldContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetInventoryContentVisibility(bool bIsVisible);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Container")
	UWorldContainerGridWidget* GetContainerGridWidget() const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UBorder> ContentBorder;

	// The grid widget that will be inside this container
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
	TObjectPtr<UWorldContainerGridWidget> ContainerGridWidget;
};
