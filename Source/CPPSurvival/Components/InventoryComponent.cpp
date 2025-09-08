#include "Components/InventoryComponent.h"
#include "Items/WorldItemBase.h"
#include "Data/ItemDataInfo.h"
#include "GameFramework/Actor.h"

UInventoryComponent::UInventoryComponent()
{
	// Set a default capacity that makes sense for a player's main inventory.
	Capacity = 30;
}

bool UInventoryComponent::PickupItem(ACPPsurvival_WorldItem* WorldItem)
{
	if (!WorldItem || !WorldItem->GetItemData())
	{
		return false;
	}

	if (GetOwner()->HasAuthority())
	{
		const int32 QuantityToPickup = WorldItem->GetQuantity();
		const int32 QuantityRemaining = AddItem(WorldItem->GetItemData(), QuantityToPickup);

		if (QuantityRemaining == 0)
		{
			WorldItem->Destroy();
			return true;
		}
		
		WorldItem->SetQuantity(QuantityRemaining);
		return false;
	}
	else
	{
		Server_PickupItem(WorldItem);
		return true;
	}
}

void UInventoryComponent::Server_PickupItem_Implementation(ACPPsurvival_WorldItem* WorldItem)
{
	// This RPC simply calls the main function on the server.
	PickupItem(WorldItem);
}

void UInventoryComponent::DropItem(int32 Index, int32 Quantity)
{
	if (GetOwner()->HasAuthority())
	{
		// We are the server.
		if (!Items.IsValidIndex(Index) || Quantity <= 0)
		{
			return;
		}

		const FContainerItem& ItemToDrop = Items[Index];
		const int32 QuantityToDrop = FMath::Min(Quantity, ItemToDrop.Quantity);
		
		// 1. Define where to spawn the item
		AActor* Owner = GetOwner();
		FVector SpawnLocation = Owner->GetActorLocation() + (Owner->GetActorForwardVector() * 150.f);
		FRotator SpawnRotation = Owner->GetActorRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// 2. Spawn the world item actor
		ACPPsurvival_WorldItem* NewWorldItem = GetWorld()->SpawnActor<ACPPsurvival_WorldItem>(SpawnLocation, SpawnRotation, SpawnParams);
		if (NewWorldItem)
		{
			NewWorldItem->SetItemData(ItemToDrop.ItemData);
			NewWorldItem->SetQuantity(QuantityToDrop);

			RemoveItem(Index, QuantityToDrop);
		}
	}
	else
	{
		Server_DropItem(Index, Quantity);
	}
}

void UInventoryComponent::Server_DropItem_Implementation(int32 Index, int32 Quantity)
{
	DropItem(Index, Quantity);
}