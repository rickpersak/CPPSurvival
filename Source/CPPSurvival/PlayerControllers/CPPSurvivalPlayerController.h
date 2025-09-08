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

protected:
	// Input mapping contexts
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> MobileExcludedMappingContexts;

	// Input actions
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

	// UI References
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UUserWidget> MobileControlsWidget;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UPlayerHUDWidget> PlayerHUD;

	// Interaction settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance = 500.0f;

	// Inventory state
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bIsInventoryOpen = false;

public:
	// Input callback functions
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnJumpStarted();
	void OnJumpCompleted();
	void OnInteract();
	void OnToggleInventory();

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	bool IsInventoryOpen() const { return bIsInventoryOpen; }
};
