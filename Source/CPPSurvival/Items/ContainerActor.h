#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ContainerActor.generated.h"

class UContainerComponent;
class UItemDataInfo;
class USceneComponent;

// Struct to define an entry in a loot table.
// This allows designers to easily create lists of items that can spawn in a container.
USTRUCT(BlueprintType)
struct FLootTableEntry
{
    GENERATED_BODY()

    // The item to potentially spawn.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
    TSoftObjectPtr<UItemDataInfo> ItemData;

    // The minimum quantity of the item to spawn.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = "1"))
    int32 MinQuantity = 1;

    // The maximum quantity of the item to spawn.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = "1"))
    int32 MaxQuantity = 1;

    // The chance (from 0.0 to 1.0) that this item will spawn at all.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SpawnChance = 1.0f;
};


UCLASS(Blueprintable, BlueprintType)
class CPPSURVIVAL_API AContainerActor : public AActor
{
    GENERATED_BODY()

public:
    AContainerActor();

    // This function is called in the editor or on spawn when properties are changed.
    virtual void OnConstruction(const FTransform& Transform) override;

protected:
    // Called when the game starts or when spawned.
    virtual void BeginPlay() override;

    // A generic root component to which we can attach any visual representation in Blueprints.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> RootSceneComponent;

    // The component that actually holds the items.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UContainerComponent> ContainerComponent;

    // --- CONFIGURATION PROPERTIES ---
    // These properties are exposed to the editor for easy configuration.

    // The name that will be displayed in the UI when this container is opened.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container Settings")
    FText ContainerName;

    // The number of slots this container will have.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container Settings", meta = (ClampMin = "1", UIMin = "1"))
    int32 Capacity = 20;

    // If true, the container will be empty when it spawns, ignoring the loot table.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Settings")
    bool bIsEmptyOnSpawn = false;
    
    // If true, the container will attempt to fill itself with items from the LootTable when the game starts.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Settings", meta = (EditCondition = "!bIsEmptyOnSpawn"))
    bool bSpawnItemsOnBeginPlay = true;
    
    // The list of possible items that can spawn in this container.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Settings", meta = (EditCondition = "!bIsEmptyOnSpawn"))
    TArray<FLootTableEntry> LootTable;

private:
    // Populates the container's inventory based on the LootTable.
    // This is only ever called on the server.
    void PopulateFromLootTable();
};