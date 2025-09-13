// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "CPPSurvivalPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;
class UPlayerHUDWidget;
class UContainerComponent;
class AEnemyCharacter;

/**
 * CPP Survival Player Controller - Handles input and UI management
 */
UCLASS()
class CPPSURVIVAL_API ACPPSurvivalPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// Server RPC to move items between containers
	UFUNCTION(Server, BlueprintCallable, Reliable)
	void Server_MoveItem(UContainerComponent* SourceContainer, int32 SourceIndex, UContainerComponent* TargetContainer, int32 TargetIndex);

	// Server RPC to handle shift-clicking items between containers
	UFUNCTION(Server, Reliable)
	void Server_ShiftClickItem(UContainerComponent* SourceContainer, int32 SourceIndex);

	// Server RPC to deal damage to an actor.
	UFUNCTION(Server, Reliable)
	void Server_DealDamage(AEnemyCharacter* DamagedEnemy, float DamageAmount);

protected:
	// --- Input ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> MobileExcludedMappingContexts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MouseLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	// --- UI ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UUserWidget> MobileControlsWidget;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UPlayerHUDWidget> PlayerHUD;

	// --- Interaction ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionSphereRadius = 50.0f;

	// --- State ---
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsInventoryOpen = false;

	UPROPERTY()
	TObjectPtr<UContainerComponent> CurrentOpenContainer;
	
public:
	// Input callbacks
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnJumpStarted();
	void OnJumpCompleted();
	void OnInteract();
	void OnToggleInventory();
	void OnSprintStarted();
	void OnSprintCompleted();

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	bool IsInventoryOpen() const { return bIsInventoryOpen; }

protected:
	// UI Management Functions
	void OpenContainer(UContainerComponent* ContainerToOpen);
	void CloseContainer();
};