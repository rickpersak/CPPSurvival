// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPPSurvivalCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInventoryComponent;
class USurvivalStatsComponent;
class UHotbarComponent;

/**
 * CPP Survival Character - The main player character
 */
UCLASS(config=Game)
class CPPSURVIVAL_API ACPPSurvivalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACPPSurvivalCharacter();

protected:
	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	// Follow camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	// Inventory component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> InventoryComponent;

	// Survival stats component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USurvivalStatsComponent> SurvivalStatsComponent;

	// Hotbar component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHotbarComponent> HotbarComponent;

public:
	// Movement functions called by the player controller
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void DoJumpEnd();

	// Getters for components
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Components")
	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Components")
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Components")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Components")
	USurvivalStatsComponent* GetSurvivalStatsComponent() const { return SurvivalStatsComponent; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Components")
	UHotbarComponent* GetHotbarComponent() const { return HotbarComponent; }
};
