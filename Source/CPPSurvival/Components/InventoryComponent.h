// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ContainerComponent.h"
#include "InventoryComponent.generated.h"

class ACPPsurvival_WorldItem;

/**
 * Inventory Component - Extends ContainerComponent with pickup functionality
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CPPSURVIVAL_API UInventoryComponent : public UContainerComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	// Try to pick up a world item, returns true if successful
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool PickupItem(ACPPsurvival_WorldItem* WorldItem);

	// Drop an item from the inventory into the world
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem(int32 Index, int32 Quantity);

protected:
	// Server RPCs
	UFUNCTION(Server, Reliable)
	void Server_PickupItem(ACPPsurvival_WorldItem* WorldItem);

	UFUNCTION(Server, Reliable)
	void Server_DropItem(int32 Index, int32 Quantity);
};
