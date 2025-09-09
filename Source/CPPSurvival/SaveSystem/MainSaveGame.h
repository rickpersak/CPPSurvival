#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MainSaveGame.generated.h"

class UPersistentData;

/**
 * Main SaveGame object for the game
 */
UCLASS()
class CPPSURVIVAL_API UMainSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Save Game")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = "Save Game")
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = "Save Game")
	TMap<FString, UPersistentData*> PlayerData;
};