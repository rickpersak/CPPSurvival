#include "UI/Game/CPPSurvivalHUD.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UI/Game/PlayerHUDWidget.h"

void ACPPsurvivalHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (PlayerHUDWidgetClass)
	{
		APlayerController* PC = GetOwningPlayerController();
		if (PC && PC->IsLocalPlayerController())
		{
			PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(PC, PlayerHUDWidgetClass);

			if (PlayerHUDWidget)
			{
				PlayerHUDWidget->AddToViewport();
			}
		}
	}
}