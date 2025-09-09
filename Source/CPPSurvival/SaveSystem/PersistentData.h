#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PersistentData.generated.h"

class UContainerComponent;
class USurvivalStatsComponent;

/**
 * UObject to hold all of a single player's persistent data
 */
UCLASS(Blueprintable, BlueprintType)
class CPPSURVIVAL_API UPersistentData : public UObject
{
	GENERATED_BODY()

public:
	// Player's unique network ID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save Data")
	FString UniquePlayerId;

	// Player's last known location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save Data")
	FVector PlayerLocation;

	// Player's last known rotation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save Data")
	FRotator PlayerRotation;

	// Delegate to be called when data is loaded and applied to the player
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDataLoaded);
	UPROPERTY(BlueprintAssignable, Category = "Save Data")
	FOnDataLoaded OnDataLoaded;

	// Saves data from the player character
	UFUNCTION(BlueprintCallable, Category = "Save Data")
	void SaveData(APlayerController* PlayerController);

	// Loads data to the player character
	UFUNCTION(BlueprintCallable, Category = "Save Data")
	void LoadData(APlayerController* PlayerController);
};