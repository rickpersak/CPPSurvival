#include "UI/Game/CPPSurvivalHUD.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UI/Game/PlayerHUDWidget.h"
#include "CPPSurvival.h"

void ACPPsurvivalHUD::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogCPPSurvival, Warning, TEXT("CPPSurvivalHUD::BeginPlay - PlayerHUDWidgetClass: %s"), *GetNameSafe(PlayerHUDWidgetClass));
	
	if (PlayerHUDWidgetClass)
	{
		APlayerController* PC = GetOwningPlayerController();
		UE_LOG(LogCPPSurvival, Warning, TEXT("CPPSurvivalHUD::BeginPlay - OwningPlayerController: %s, IsLocal: %s"), 
			*GetNameSafe(PC), PC && PC->IsLocalPlayerController() ? TEXT("true") : TEXT("false"));
			
		if (PC && PC->IsLocalPlayerController())
		{
			PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(PC, PlayerHUDWidgetClass);
			UE_LOG(LogCPPSurvival, Warning, TEXT("CPPSurvivalHUD::BeginPlay - Created PlayerHUDWidget: %s"), *GetNameSafe(PlayerHUDWidget));

			if (PlayerHUDWidget)
			{
				PlayerHUDWidget->AddToViewport();
				UE_LOG(LogCPPSurvival, Warning, TEXT("CPPSurvivalHUD::BeginPlay - PlayerHUDWidget added to viewport successfully"));
			}
			else
			{
				UE_LOG(LogCPPSurvival, Error, TEXT("CPPSurvivalHUD::BeginPlay - Failed to create PlayerHUDWidget!"));
			}
		}
		else
		{
			UE_LOG(LogCPPSurvival, Error, TEXT("CPPSurvivalHUD::BeginPlay - Invalid PlayerController or not local"));
		}
	}
	else
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("CPPSurvivalHUD::BeginPlay - PlayerHUDWidgetClass is null! Make sure it's set in the HUD Blueprint."));
	}
}