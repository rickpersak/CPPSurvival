#include "Components/ContainerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Data/ItemDataInfo.h"
#include "Items/WorldItemBase.h"
#include "CPPSurvival.h"

UContainerComponent::UContainerComponent()
{
	// This makes the component replicate by default.
	SetIsReplicatedByDefault(true);
}

void UContainerComponent::BeginPlay()
{
	Super::BeginPlay();

	// The server is responsible for initializing the container's state.
	if (GetOwner()->HasAuthority())
	{
		// If capacity is > 0, resize the array to that size.
		// This fills the array with default-constructed FContainerItems, which are our "empty" slots.
		if (Capacity > 0)
		{
			Items.SetNum(Capacity);
		}
	}
}

void UContainerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UContainerComponent, Items);
}

void UContainerComponent::OnRep_Items()
{
	OnContainerUpdated.Broadcast();
}

int32 UContainerComponent::AddItem(UItemDataInfo* ItemData, int32 Quantity)
{
	if (GetOwner()->HasAuthority())
	{
		if (!ItemData || Quantity <= 0)
		{
			return Quantity;
		}
		
		int32 QuantityToAdd = Quantity;

		// 1. First pass: Try to stack with existing items.
		for (FContainerItem& Item : Items)
		{
			if (QuantityToAdd > 0 && Item.ItemData == ItemData && Item.Quantity < ItemData->MaxStackSize)
			{
				const int32 CanAdd = FMath::Min(QuantityToAdd, ItemData->MaxStackSize - Item.Quantity);
				Item.Quantity += CanAdd;
				QuantityToAdd -= CanAdd;
			}
		}

		// 2. Second pass: If items remain, find empty slots and create new stacks.
		if (QuantityToAdd > 0)
		{
			for (FContainerItem& Item : Items)
			{
				if (QuantityToAdd > 0 && !Item.ItemData) // Check for an empty slot (!Item.ItemData is the same as Item.ItemData == nullptr)
				{
					const int32 AddAmount = FMath::Min(QuantityToAdd, ItemData->MaxStackSize);
					Item.ItemData = ItemData;
					Item.Quantity = AddAmount;
					QuantityToAdd -= AddAmount;
				}
			}
		}
		
		// After any modification on the server, manually broadcast the update.
		// This ensures the server's UI also updates. OnRep_Items only fires for clients.
		OnContainerUpdated.Broadcast();
		
		return QuantityToAdd; // Return what was left over (e.g., if inventory was full).
	}
	else
	{
		Server_AddItem(ItemData, Quantity);
		return 0;
	}
}

void UContainerComponent::Server_AddItem_Implementation(UItemDataInfo* ItemData, int32 Quantity)
{
	// The RPC calls the public function on the server.
	AddItem(ItemData, Quantity);
}

void UContainerComponent::RemoveItem(int32 Index, int32 Quantity)
{
	if (GetOwner()->HasAuthority())
	{
		if (!Items.IsValidIndex(Index) || !Items[Index].ItemData || Quantity <= 0)
		{
			return;
		}
		
		Items[Index].Quantity -= Quantity;

		// If the stack is now empty, reset the struct to its default "empty" state.
		// DO NOT remove from the array, as that would break the fixed-size slot system.
		if (Items[Index].Quantity <= 0)
		{
			Items[Index] = FContainerItem(); // Resets ItemData to nullptr and Quantity to 0
		}
		
		OnContainerUpdated.Broadcast();
	}
	else
	{
		Server_RemoveItem(Index, Quantity);
	}
}

void UContainerComponent::Server_RemoveItem_Implementation(int32 Index, int32 Quantity)
{
	RemoveItem(Index, Quantity);
}

void UContainerComponent::MoveItem(int32 SourceIndex, UContainerComponent* TargetContainer, int32 TargetIndex)
{
	UE_LOG(LogCPPSurvival, Warning, TEXT("MoveItem called: SourceIndex=%d, TargetIndex=%d, TargetContainer=%s"), 
		SourceIndex, TargetIndex, TargetContainer ? TEXT("Valid") : TEXT("NULL"));

	if (GetOwner()->HasAuthority())
	{
		UE_LOG(LogCPPSurvival, Warning, TEXT("MoveItem: Running on server/authority"));
		
		// Basic validation
		if (!TargetContainer)
		{
			UE_LOG(LogCPPSurvival, Error, TEXT("MoveItem: TargetContainer is NULL!"));
			return;
		}
		
		if (!Items.IsValidIndex(SourceIndex))
		{
			UE_LOG(LogCPPSurvival, Error, TEXT("MoveItem: SourceIndex %d is invalid! Items array size: %d"), SourceIndex, Items.Num());
			return;
		}
		
		if (!TargetContainer->Items.IsValidIndex(TargetIndex))
		{
			UE_LOG(LogCPPSurvival, Error, TEXT("MoveItem: TargetIndex %d is invalid! Target array size: %d"), TargetIndex, TargetContainer->Items.Num());
			return;
		}

		// Log what we're swapping
		FString SourceItemName = Items[SourceIndex].ItemData ? Items[SourceIndex].ItemData->DisplayName.ToString() : TEXT("Empty");
		FString TargetItemName = TargetContainer->Items[TargetIndex].ItemData ? TargetContainer->Items[TargetIndex].ItemData->DisplayName.ToString() : TEXT("Empty");
		
		UE_LOG(LogCPPSurvival, Warning, TEXT("MoveItem: Swapping '%s' (index %d) with '%s' (index %d)"), 
			*SourceItemName, SourceIndex, *TargetItemName, TargetIndex);

		// Simply swap the item structs. FContainerItem is small, so this is efficient.
		// This logic handles moving within the same inventory and moving between two different inventories.
		Swap(Items[SourceIndex], TargetContainer->Items[TargetIndex]);

		UE_LOG(LogCPPSurvival, Warning, TEXT("MoveItem: Swap completed successfully"));

		// Broadcast updates for both containers so their UIs refresh.
		OnContainerUpdated.Broadcast();
		if (this != TargetContainer)
		{
			UE_LOG(LogCPPSurvival, Warning, TEXT("MoveItem: Broadcasting update for different target container"));
			TargetContainer->OnContainerUpdated.Broadcast();
		}
		else
		{
			UE_LOG(LogCPPSurvival, Warning, TEXT("MoveItem: Moving within same container"));
		}
	}
	else
	{
		UE_LOG(LogCPPSurvival, Warning, TEXT("MoveItem: Running on client, sending RPC to server"));
		// We are a client, send an RPC to the server.
		Server_MoveItem(SourceIndex, TargetContainer, TargetIndex);
	}
}

void UContainerComponent::Server_MoveItem_Implementation(int32 SourceIndex, UContainerComponent* TargetContainer, int32 TargetIndex)
{
	UE_LOG(LogCPPSurvival, Warning, TEXT("Server_MoveItem_Implementation: Received RPC call with SourceIndex=%d, TargetIndex=%d"), SourceIndex, TargetIndex);
	
	// The RPC just calls the public function on the server.
	MoveItem(SourceIndex, TargetContainer, TargetIndex);
}

bool UContainerComponent::HasItems(UItemDataInfo* ItemData, int32 Quantity) const
{
	if (!ItemData) return false;
	
	int32 TotalQuantity = 0;
	for (const FContainerItem& Item : Items)
	{
		if (Item.ItemData == ItemData)
		{
			TotalQuantity += Item.Quantity;
		}
	}
	return TotalQuantity >= Quantity;
}

void UContainerComponent::DropItem(int32 Index, int32 Quantity)
{
	if (GetOwner()->HasAuthority())
	{
		if (!Items.IsValidIndex(Index) || Quantity <= 0)
		{
			return;
		}

		const FContainerItem& ItemToDrop = Items[Index];
		const int32 QuantityToDrop = FMath::Min(Quantity, ItemToDrop.Quantity);

		AActor* Owner = GetOwner();
		FVector SpawnLocation = Owner->GetActorLocation() + (Owner->GetActorForwardVector() * 150.f);
		FRotator SpawnRotation = Owner->GetActorRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

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

void UContainerComponent::Server_DropItem_Implementation(int32 Index, int32 Quantity)
{
	DropItem(Index, Quantity);
}

bool UContainerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (const FContainerItem& Item : Items)
	{
		if (Item.ItemData)
		{
			bWroteSomething |= Channel->ReplicateSubobject(Item.ItemData, *Bunch, *RepFlags);
		}
	}
	return bWroteSomething;
}


