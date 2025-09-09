#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Items/ContainerActor.h"
#include "EnemyCharacter.generated.h"

class UContainerComponent;
class UHealthComponent;

/**
 * Base class for all enemy characters in the game.
 * Inherits from ACharacter for movement and AI, and contains a UContainerComponent for loot.
 */
UCLASS()
class CPPSURVIVAL_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

	// The component that manages the enemy's health.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHealthComponent> HealthComponent;
	
	// The component that will hold the enemy's loot upon death.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UContainerComponent> ContainerComponent;

	// The list of possible items that this enemy can drop.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Settings")
	TArray<FLootTableEntry> LootTable;

	// This function is bound to the HealthComponent's OnDeath event.
	UFUNCTION()
	void OnDeath();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath();

private:
	// Populates the container's inventory based on the LootTable.
	void PopulateFromLootTable();
};