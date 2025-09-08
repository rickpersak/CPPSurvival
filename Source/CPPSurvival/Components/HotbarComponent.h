#pragma once

#include "CoreMinimal.h"
#include "Components/ContainerComponent.h"
#include "HotbarComponent.generated.h"

/**
 * A specialized container component for the player's hotbar.
 * It has a fixed capacity of 9 slots.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CPPSURVIVAL_API UHotbarComponent : public UContainerComponent
{
	GENERATED_BODY()

public:

	UHotbarComponent();
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Hotbar")
	void DropItem(int32 Index, int32 Quantity);
	
protected:
	UFUNCTION(Server, Reliable)
	void Server_DropItem(int32 Index, int32 Quantity);
};
