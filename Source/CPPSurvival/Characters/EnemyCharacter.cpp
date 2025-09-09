#include "Characters/EnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/ContainerComponent.h"
#include "Components/HealthComponent.h"
#include "Data/ItemDataInfo.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
 	// This actor doesn't tick.
	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

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
	PopulateFromLootTable();
	Multicast_OnDeath();
	SetLifeSpan(120.0f);
}

void AEnemyCharacter::Multicast_OnDeath_Implementation()
{
	// Stop the AI controller.
	AController* AIController = GetController();
	if (AIController)
	{
		AIController->UnPossess();
	}

	// Disable character movement to prevent interference with ragdoll
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->DisableMovement();
		MovementComp->StopMovementImmediately();
	}

	// Disable the capsule component's collision entirely.
	// This is what allows you to walk through the corpse.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Turn the mesh into a ragdoll. The mesh already exists as part of ACharacter.
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent)
	{
		// Detach the mesh from the capsule so it can fall freely
		MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		
		// Set collision for ragdoll physics
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));
		
		// Enable physics simulation for ragdoll effect
		MeshComponent->SetSimulatePhysics(true);

		// Allow the mesh to be hit by the visibility trace for looting
		MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		
		UE_LOG(LogTemp, Warning, TEXT("Ragdoll setup complete for %s"), *GetName());
	}
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