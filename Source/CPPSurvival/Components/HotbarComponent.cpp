#include "Components/HotbarComponent.h"
#include "Items/WorldItemBase.h"
#include "Data/ItemDataInfo.h"
#include "GameFramework/Actor.h"

UHotbarComponent::UHotbarComponent()
{
	Capacity = 9;
}

void UHotbarComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		Items.SetNum(Capacity);
	}
}


void UHotbarComponent::DropItem(int32 Index, int32 Quantity)
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

void UHotbarComponent::Server_DropItem_Implementation(int32 Index, int32 Quantity)
{
	DropItem(Index, Quantity);
}