#include "UI/Game/InventoryContainerWidget.h"
#include "Components/BackgroundBlur.h"
#include "Components/Border.h"
#include "UI/Inventory/InventoryGridWidget.h"
#include "CPPSurvival.h"

void UInventoryContainerWidget::SetInventoryContentVisibility(bool bIsVisible)
{
	ESlateVisibility NewVisibility = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	ESlateVisibility CurrentVisibility = GetVisibility();
	
	UE_LOG(LogCPPSurvival, Warning, TEXT("InventoryContainerWidget::SetInventoryContentVisibility called - bIsVisible: %s, Current Visibility: %d, New Visibility: %d"), 
		bIsVisible ? TEXT("true") : TEXT("false"), 
		(int32)CurrentVisibility,
		(int32)NewVisibility);
		
	SetVisibility(NewVisibility);
	
	UE_LOG(LogCPPSurvival, Warning, TEXT("InventoryContainerWidget visibility set. Final visibility: %d"), (int32)GetVisibility());
}

UInventoryGridWidget* UInventoryContainerWidget::GetInventoryGridWidget() const
{
	return InventoryGridWidget;
}