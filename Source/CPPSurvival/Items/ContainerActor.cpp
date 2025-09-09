#include "Items/ContainerActor.h"
#include "Components/SceneComponent.h"
#include "Components/ContainerComponent.h"
#include "Data/ItemDataInfo.h"

AContainerActor::AContainerActor()
{
    // Enable replication for this actor. This is crucial for multiplayer.
    bReplicates = true;
    SetReplicatingMovement(true);

    PrimaryActorTick.bCanEverTick = false;

    // Create a generic scene component and set it as the root.
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    RootComponent = RootSceneComponent;

    // Create the container component. It doesn't need to be attached as it has no transform.
    ContainerComponent = CreateDefaultSubobject<UContainerComponent>(TEXT("ContainerComponent"));
}

void AContainerActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // This code runs in the editor. When a designer changes the 'Capacity' or 'ContainerName'
    // on this actor, we immediately update the actual values in the ContainerComponent.
    // This provides instant feedback and keeps the data in sync.
    if (ContainerComponent)
    {
        ContainerComponent->SetCapacity(Capacity);
        ContainerComponent->SetContainerName(ContainerName);
    }
}

void AContainerActor::BeginPlay()
{
    Super::BeginPlay();

    // Only the server should be responsible for spawning loot.
    if (HasAuthority())
    {
        // Sync properties one last time to ensure everything is correct at runtime.
        if (ContainerComponent)
        {
            ContainerComponent->SetCapacity(Capacity);
            ContainerComponent->SetContainerName(ContainerName);
        }
        
        if (bSpawnItemsOnBeginPlay && !bIsEmptyOnSpawn)
        {
            PopulateFromLootTable();
        }
    }
}

void AContainerActor::PopulateFromLootTable()
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