#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveLoadSubsystem.generated.h"

class UMainSaveGame;
class UPersistentData;

/**
 * Subsystem to manage saving and loading the game
 */
UCLASS()
class CPPSURVIVAL_API USaveLoadSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void LoadGame();
    
	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void SavePlayer(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void LoadPlayer(APlayerController* PlayerController);

protected:
	UPROPERTY()
	TObjectPtr<UMainSaveGame> CurrentSaveGame;
    
	FString CurrentSlotName;
};