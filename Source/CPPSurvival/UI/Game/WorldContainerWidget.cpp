#include "UI/Game/WorldContainerWidget.h"
#include "Components/Border.h"
#include "UI/Game/WorldContainerGridWidget.h"

void UWorldContainerWidget::SetInventoryContentVisibility(bool bIsVisible)
{
	SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

UWorldContainerGridWidget* UWorldContainerWidget::GetContainerGridWidget() const
{
	return ContainerGridWidget;
}
