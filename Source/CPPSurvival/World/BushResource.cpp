// Fill out your copyright notice in the Description page of Project Settings.


#include "BushResource.h"


// Sets default values
ABushResource::ABushResource()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABushResource::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABushResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

