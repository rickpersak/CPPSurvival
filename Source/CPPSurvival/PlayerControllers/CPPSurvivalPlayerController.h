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

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	bool IsInventoryOpen() const { return bIsInventoryOpen; }

protected:
	// UI Management Functions
	void OpenContainer(UContainerComponent* ContainerToOpen);
	void CloseContainer();
};