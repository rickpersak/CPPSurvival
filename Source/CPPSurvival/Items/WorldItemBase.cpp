#include "Items/WorldItemBase.h"
#include "Data/ItemDataInfo.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

ACPPsurvival_WorldItem::ACPPsurvival_WorldItem()
{
	bReplicates = true;
	SetReplicatingMovement(true);
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMeshComponent;
	StaticMeshComponent->SetSimulatePhysics(true);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(250.f);
	
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ACPPsurvival_WorldItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACPPsurvival_WorldItem, ItemData);
	DOREPLIFETIME(ACPPsurvival_WorldItem, Quantity);
}

void ACPPsurvival_WorldItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	// Initialize visuals from ItemData. This is great for seeing the mesh in the editor
	// when you assign the data asset.
	InitializeFromItemData();
}

void ACPPsurvival_WorldItem::OnRep_ItemData()
{
	// This function is called on clients when the ItemData variable is updated from the server.
	// We update the visuals here to ensure clients see the correct item.
	InitializeFromItemData();
}

void ACPPsurvival_WorldItem::InitializeFromItemData()
{
	if (ItemData)
	{
		if (ItemData->WorldMesh)
		{
			StaticMeshComponent->SetStaticMesh(ItemData->WorldMesh);
		}
		else
		{
			StaticMeshComponent->SetStaticMesh(nullptr);
		}
	}
	else
	{
		StaticMeshComponent->SetStaticMesh(nullptr);
	}
}

void ACPPsurvival_WorldItem::SetItemData(UItemDataInfo* NewItemData)
{
	ItemData = NewItemData;
	InitializeFromItemData();
}

void ACPPsurvival_WorldItem::SetQuantity(int32 NewQuantity)
{
	if (HasAuthority())
	{
		Quantity = NewQuantity;
	}
}