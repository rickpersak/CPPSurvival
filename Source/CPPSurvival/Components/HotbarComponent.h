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

	// Drop an item from the hotbar into the world (overrides UContainerComponent)
	virtual void DropItem(int32 Index, int32 Quantity) override;
	
protected:
	virtual void Server_DropItem_Implementation(int32 Index, int32 Quantity) override;
};
