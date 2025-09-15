#pragma once

#include "CoreMinimal.h"
#include "World/HarvestableResource.h"
#include "RockResource.generated.h"

UCLASS()
class CPPSURVIVAL_API ARockResource : public AHarvestableResource
{
	GENERATED_BODY()

public:
	ARockResource();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};