// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ContainerComponent.h"
#include "HotbarComponent.generated.h"

/**
 * A specialized container component for the player's hotbar.
 * It has a fixed capacity of 9 slots.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CPPSURVIVAL_API UHotbarComponent : public UContainerComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHotbarComponent();
};
