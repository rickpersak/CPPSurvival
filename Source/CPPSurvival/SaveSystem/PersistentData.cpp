#include "SaveSystem/PersistentData.h"
#include "GameFramework/PlayerController.h"
#include "Characters/CPPSurvivalCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/HotbarComponent.h"
#include "Components/SurvivalStatsComponent.h"

void UPersistentData::SaveData(APlayerController* PlayerController)
{
	if (!PlayerController) return;

	ACPPSurvivalCharacter* Character = Cast<ACPPSurvivalCharacter>(PlayerController->GetPawn());
	if (!Character) return;
	
	PlayerLocation = Character->GetActorLocation();
	PlayerRotation = Character->GetActorRotation();
}

void UPersistentData::LoadData(APlayerController* PlayerController)
{
	if (!PlayerController) return;

	ACPPSurvivalCharacter* Character = Cast<ACPPSurvivalCharacter>(PlayerController->GetPawn());
	if (!Character) return;
	
	Character->SetActorLocation(PlayerLocation);
	PlayerController->SetControlRotation(PlayerRotation);

	OnDataLoaded.Broadcast();
}