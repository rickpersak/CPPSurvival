// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "ItemDataInfo.generated.h"

/**
 * Item Data Info - defines the properties of an item type
 */
UCLASS(BlueprintType)
class CPPSURVIVAL_API UItemDataInfo : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// The display name of the item
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Item Info")
	FText DisplayName;

	// Description of the item
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Item Info")
	FText Description;

	// The icon to show in the inventory UI
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Item Info")
	TObjectPtr<UTexture2D> Icon = nullptr;

	// The mesh to show when this item is dropped in the world
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Item Info")
	TObjectPtr<UStaticMesh> WorldMesh = nullptr;

	// Maximum number of this item that can be stacked in one slot
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Item Info", meta = (ClampMin = "1"))
	int32 MaxStackSize = 1;

	// Override to provide a custom Primary Asset ID
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
