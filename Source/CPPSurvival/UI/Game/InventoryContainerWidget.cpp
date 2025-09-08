#include "UI/Game/InventoryContainerWidget.h"
#include "Components/BackgroundBlur.h" // Include BackgroundBlur
#include "Components/Border.h"         // Include Border

void UInventoryContainerWidget::SetInventoryContentVisibility(bool bIsVisible)
{
	SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}