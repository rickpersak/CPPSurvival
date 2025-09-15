#include "World/HarvestableResource.h"
#include "Components/StaticMeshComponent.h"
#include "Data/ItemDataInfo.h"
#include "Components/InventoryComponent.h"

AHarvestableResource::AHarvestableResource()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Health = 100.0f;
	MaxHealth = 100.0f;
	HarvestedItemQuantity = 1;
}

void AHarvestableResource::BeginPlay()
{
	Super::BeginPlay();
}

void AHarvestableResource::OnInteract_Implementation(AActor* Interactor)
{
	Harvest(Interactor, 25.f); // Example damage value
}

void AHarvestableResource::Harvest(AActor* Interactor, float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

	OnHarvested();

	if (Health <= 0)
	{
		OnDepleted();

		if (Interactor)
		{
			if (UInventoryComponent* Inventory = Interactor->FindComponentByClass<UInventoryComponent>())
			{
				if(HarvestedItem)
				{
					Inventory->AddItem(HarvestedItem, HarvestedItemQuantity);
				}
			}
		}
		Destroy();
	}
}