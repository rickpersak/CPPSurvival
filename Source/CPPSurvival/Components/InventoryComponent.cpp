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
	// On both client and server, we can do an early check.
	if (!WorldItem || !WorldItem->GetItemData())
	{
		return false;
	}

	if (GetOwner()->HasAuthority())
	{
		// We are the server, execute the logic.
		// We attempt to add the item using the base class function.
		// Let's assume world items are always quantity 1 for simplicity.
		const int32 QuantityRemaining = AddItem(WorldItem->GetItemData(), 1);

		if (QuantityRemaining == 0)
		{
			// If AddItem returned 0, it means the item was successfully added.
			// Now we can safely destroy the world actor.
			WorldItem->Destroy();
			return true;
		}
		
		// If QuantityRemaining > 0, the inventory was full, so the pickup fails.
		// We don't destroy the actor and return false.
		return false;
	}
	else
	{
		// We are a client, so we send an RPC to the server to do the work.
		Server_PickupItem(WorldItem);
		
		// We return true for responsiveness, but the server has the final say.
		// This is called "optimistic prediction."
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
			// 3. Set the item data on the new actor
			// This will replicate to all clients and make it appear correctly.
			NewWorldItem->SetItemData(ItemToDrop.ItemData);

			// 4. Remove the item from this inventory
			RemoveItem(Index, QuantityToDrop);
		}
	}
	else
	{
		// We are a client, send an RPC to the server.
		Server_DropItem(Index, Quantity);
	}
}

void UInventoryComponent::Server_DropItem_Implementation(int32 Index, int32 Quantity)
{
	DropItem(Index, Quantity);
}