// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CPPSurvivalHUD.generated.h"

class UUserWidget;
class UPlayerHUDWidget;

/**
 * CPP Survival HUD - Manages the main game HUD
 */
UCLASS()
class CPPSURVIVAL_API ACPPsurvivalHUD : public AHUD
{
	GENERATED_BODY()

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	// The widget class to use for the player HUD
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	// Reference to the created player HUD widget
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidget;

public:
	// Getter for the player HUD widget
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	UPlayerHUDWidget* GetPlayerHUDWidget() const { return PlayerHUDWidget; }
};
