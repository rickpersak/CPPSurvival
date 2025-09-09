#include "Characters/EnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/ContainerComponent.h"
#include "Components/HealthComponent.h"
#include "Data/ItemDataInfo.h"
#include "AIController.h"

AEnemyCharacter::AEnemyCharacter()
{
 	// This actor doesn't tick.
	PrimaryActorTick.bCanEverTick = false;

	// Create the health component.
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	// Create the container component that will hold loot.
	ContainerComponent = CreateDefaultSubobject<UContainerComponent>(TEXT("ContainerComponent"));
	ContainerComponent->SetCapacity(12);
	ContainerComponent->SetContainerName(FText::FromString("Corpse"));
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind the OnDeath event to our custom function on the server.
	if (HasAuthority())
	{
		if (HealthComponent)
		{
			HealthComponent->OnDeath.AddDynamic(this, &AEnemyCharacter::OnDeath);
		}
	}
}

void AEnemyCharacter::OnDeath()
{
	// This function is only ever called on the server because the delegate is only bound there.

	// Stop the AI controller.
	AController* AIController = GetController();
	if (AIController)
	{
		AIController->UnPossess();
	}

	// Disable the main capsule collision so players can walk through the corpse.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Populate the inventory with loot.
	PopulateFromLootTable();

	// Turn the mesh into a ragdoll. The mesh already exists as part of ACharacter.
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent)
	{
		MeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComponent->SetSimulatePhysics(true);
	}
	
	// The corpse will be removed from the world after 2 minutes.
	SetLifeSpan(120.0f);
}

void AEnemyCharacter::PopulateFromLootTable()
{
	if (!HasAuthority() || !ContainerComponent)
	{
		return;
	}

	for (const FLootTableEntry& Entry : LootTable)
	{
		// Check the spawn chance.
		if (FMath::FRand() <= Entry.SpawnChance)
		{
			// Load the item data asset if it's valid.
			UItemDataInfo* ItemData = Entry.ItemData.LoadSynchronous();
			if (ItemData)
			{
				// Determine a random quantity within the specified range.
				const int32 Quantity = FMath::RandRange(Entry.MinQuantity, Entry.MaxQuantity);
				ContainerComponent->AddItem(ItemData, Quantity);
			}
		}
	}
}