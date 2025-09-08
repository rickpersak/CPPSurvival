// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "WorldItemBase.generated.h"

class UItemDataInfo;
class UStaticMeshComponent;
class USphereComponent;

/**
 * World Item Base - Represents an item that exists in the world
 */
UCLASS()
class CPPSURVIVAL_API ACPPsurvival_WorldItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ACPPsurvival_WorldItem();

	// Called when the actor is constructed
	virtual void OnConstruction(const FTransform& Transform) override;

	// Replication setup
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// The mesh component that renders the item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	// Overlap sphere for interaction detection
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> OverlapSphere;

	// The data that defines what item this is (replicated)
	UPROPERTY(ReplicatedUsing = OnRep_ItemData, BlueprintReadWrite, EditAnywhere, Category = "Item")
	TObjectPtr<UItemDataInfo> ItemData;

public:
	// Getter for item data
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Item")
	UItemDataInfo* GetItemData() const { return ItemData; }

	// Setter for item data
	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetItemData(UItemDataInfo* NewItemData) { ItemData = NewItemData; InitializeFromItemData(); }

protected:
	// Called when ItemData is replicated to clients
	UFUNCTION()
	void OnRep_ItemData();

	// Initialize the visual representation from the item data
	void InitializeFromItemData();
};
