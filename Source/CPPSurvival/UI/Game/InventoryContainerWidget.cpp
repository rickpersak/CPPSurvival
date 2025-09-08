#include "UI/Game/InventoryContainerWidget.h"
#include "Components/BackgroundBlur.h"
#include "Components/Border.h"
#include "UI/Inventory/InventoryGridWidget.h"

void UInventoryContainerWidget::SetInventoryContentVisibility(bool bIsVisible)
{
	SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

UInventoryGridWidget* UInventoryContainerWidget::GetInventoryGridWidget() const
{
	return InventoryGridWidget;
}