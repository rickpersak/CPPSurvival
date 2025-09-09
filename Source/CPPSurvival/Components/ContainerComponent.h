// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ContainerItem.h"
#include "Net/UnrealNetwork.h"
#include "ContainerComponent.generated.h"

class UItemDataInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContainerUpdated);

/**
 * Container Component - Base class for inventory-like containers
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CPPSURVIVAL_API UContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UContainerComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Replication setup
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

protected:
	// The items stored in this container (replicated)
	UPROPERTY(ReplicatedUsing = OnRep_Items, BlueprintReadOnly, Category = "Container")
	TArray<FContainerItem> Items;

	// Maximum number of slots in this container
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Container", meta = (ClampMin = "1"))
	int32 Capacity = 20;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Container")
	FText ContainerName;

public:
	// Delegate that fires when the container is updated
	UPROPERTY(BlueprintAssignable, Category = "Container")
	FOnContainerUpdated OnContainerUpdated;

	// Add an item to the container, returns how much couldn't be added
	UFUNCTION(BlueprintCallable, Category = "Container")
	int32 AddItem(UItemDataInfo* ItemData, int32 Quantity);

	// Remove items from a specific slot
	UFUNCTION(BlueprintCallable, Category = "Container")
	void RemoveItem(int32 Index, int32 Quantity);

	// Move an item from one slot to another (can be in different containers)
	UFUNCTION(BlueprintCallable, Category = "Container")
	void MoveItem(int32 SourceIndex, UContainerComponent* TargetContainer, int32 TargetIndex);

	// Drop an item from the container into the world
	UFUNCTION(BlueprintCallable, Category = "Container")
	virtual void DropItem(int32 Index, int32 Quantity);

	// Check if the container has the specified quantity of an item
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Container")
	bool HasItems(UItemDataInfo* ItemData, int32 Quantity) const;

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Container")
	const TArray<FContainerItem>& GetItems() const { return Items; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Container")
	int32 GetCapacity() const { return Capacity; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Container")
	const FText& GetContainerName() const { return ContainerName; }

protected:
	// Replication callbacks
	UFUNCTION()
	void OnRep_Items();

	// Server RPCs
	UFUNCTION(Server, Reliable)
	void Server_AddItem(UItemDataInfo* ItemData, int32 Quantity);

	UFUNCTION(Server, Reliable)
	void Server_RemoveItem(int32 Index, int32 Quantity);

	UFUNCTION(Server, Reliable)
	void Server_MoveItem(int32 SourceIndex, UContainerComponent* TargetContainer, int32 TargetIndex);

	UFUNCTION(Server, Reliable)
	void Server_DropItem(int32 Index, int32 Quantity);
};
