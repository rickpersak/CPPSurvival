#include "SaveSystem/SaveLoadSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SaveSystem/MainSaveGame.h"
#include "SaveSystem/PersistentData.h"
#include "GameFramework/PlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void USaveLoadSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentSlotName = TEXT("MainSaveSlot");
	LoadGame();
}

void USaveLoadSubsystem::SaveGame()
{
	if (!CurrentSaveGame)
	{
		CurrentSaveGame = Cast<UMainSaveGame>(UGameplayStatics::CreateSaveGameObject(UMainSaveGame::StaticClass()));
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);
}

void USaveLoadSubsystem::LoadGame()
{
	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{
		CurrentSaveGame = Cast<UMainSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
	}
	else
	{
		SaveGame();
	}
}

void USaveLoadSubsystem::SavePlayer(APlayerController* PlayerController)
{
	if (!PlayerController || !CurrentSaveGame) return;

	APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
	if (!PlayerState) return;

	FString PlayerId = PlayerState->GetUniqueId().GetUniqueNetId()->ToString();

	UPersistentData** PlayerDataPtr = CurrentSaveGame->PlayerData.Find(PlayerId);
	UPersistentData* PlayerData = PlayerDataPtr ? *PlayerDataPtr : NewObject<UPersistentData>(CurrentSaveGame);
    
	PlayerData->SaveData(PlayerController);
	CurrentSaveGame->PlayerData.Add(PlayerId, PlayerData);

	SaveGame();
}

void USaveLoadSubsystem::LoadPlayer(APlayerController* PlayerController)
{
	if (!PlayerController || !CurrentSaveGame) return;
    
	APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
	if (!PlayerState) return;

	FString PlayerId = PlayerState->GetUniqueId().GetUniqueNetId()->ToString();

	if (UPersistentData** PlayerData = CurrentSaveGame->PlayerData.Find(PlayerId))
	{
		(*PlayerData)->LoadData(PlayerController);
	}
}