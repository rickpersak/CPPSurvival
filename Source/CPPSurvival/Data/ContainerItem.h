// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ContainerItem.generated.h"

class UItemDataInfo;

/**
 * Container Item Struct - represents an item stack in a container/inventory
 */
USTRUCT(BlueprintType)
struct CPPSURVIVAL_API FContainerItem
{
	GENERATED_BODY()

public:
	// The item data (what kind of item this is)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UItemDataInfo> ItemData = nullptr;

	// How many of this item we have in this stack
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Quantity = 0;

	// Default constructor creates an empty slot
	FContainerItem()
	{
		ItemData = nullptr;
		Quantity = 0;
	}

	// Constructor with parameters
	FContainerItem(UItemDataInfo* InItemData, int32 InQuantity)
		: ItemData(InItemData), Quantity(InQuantity)
	{
	}

	// Check if this slot is empty
	bool IsEmpty() const
	{
		return ItemData == nullptr || Quantity <= 0;
	}
};
