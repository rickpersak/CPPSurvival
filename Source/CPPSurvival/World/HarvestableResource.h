#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "HarvestableResource.generated.h"

class UItemDataInfo;

UCLASS()
class CPPSURVIVAL_API AHarvestableResource : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	AHarvestableResource();

	virtual void OnInteract_Implementation(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Harvesting")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Harvesting")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Harvesting")
	UItemDataInfo* HarvestedItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Harvesting")
	int32 HarvestedItemQuantity;

	UFUNCTION(BlueprintCallable, Category = "Harvesting")
	void Harvest(AActor* Interactor, float Damage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Harvesting")
	void OnHarvested();

	UFUNCTION(BlueprintImplementableEvent, Category = "Harvesting")
	void OnDepleted();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;
};