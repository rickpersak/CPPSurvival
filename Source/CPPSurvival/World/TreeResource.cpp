#include "World/TreeResource.h"


// Sets default values
ATreeResource::ATreeResource()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATreeResource::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATreeResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

