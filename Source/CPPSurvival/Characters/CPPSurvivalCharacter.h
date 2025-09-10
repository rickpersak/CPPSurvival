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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

	// True if the character is sprinting
	UPROPERTY(ReplicatedUsing = OnRep_IsSprinting, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting;
	
	// Called when bIsSprinting is replicated
	UFUNCTION()
	void OnRep_IsSprinting();

	// The character's normal walking speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	// The character's sprinting speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;

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

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopSprint();

	UFUNCTION(Server, Reliable)
	void Server_StartSprint();

	UFUNCTION(Server, Reliable)
	void Server_StopSprint();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	bool IsSprinting() const { return bIsSprinting; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetSprintStateForUI(bool bNewSprintState);

	UFUNCTION(Server, Reliable)
	void Server_SetSprintStateForUI(bool bNewSprintState);

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