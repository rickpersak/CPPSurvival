#pragma once

#include "CoreMinimal.h"
#include "World/HarvestableResource.h"
#include "BushResource.generated.h"

UCLASS()
class CPPSURVIVAL_API ABushResource : public AHarvestableResource
{
	GENERATED_BODY()

public:
	ABushResource();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};