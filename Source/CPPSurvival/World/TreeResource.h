#pragma once

#include "CoreMinimal.h"
#include "World/HarvestableResource.h"
#include "TreeResource.generated.h"

UCLASS()
class CPPSURVIVAL_API ATreeResource : public AHarvestableResource
{
	GENERATED_BODY()

public:
	ATreeResource();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};