// Copyright Epic Games, Inc. All Rights Reserved.


#include "CPPSurvivalPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "CPPSurvival.h"
#include "Items/WorldItemBase.h"
#include "Components/InventoryComponent.h"
#include "Characters/CPPSurvivalCharacter.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "UI/Game/PlayerHUDWidget.h"
#include "UI/Game/CPPSurvivalHUD.h"

void ACPPSurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ACPPsurvivalHUD* HUD = GetHUD<ACPPsurvivalHUD>())
	{
		// We'll need to expose the widget from the HUD class later
		// PlayerHUD = HUD->GetPlayerHUDWidget(); 
	}

	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogCPPSurvival, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void ACPPSurvivalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
			
		}
	}

	// Set up enhanced input action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACPPSurvivalPlayerController::OnJumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACPPSurvivalPlayerController::OnJumpCompleted);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACPPSurvivalPlayerController::OnMove);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACPPSurvivalPlayerController::OnLook);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACPPSurvivalPlayerController::OnLook);

		// Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACPPSurvivalPlayerController::OnInteract);

		// Inventory
		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ACPPSurvivalPlayerController::OnToggleInventory);
	}
	else
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACPPSurvivalPlayerController::OnMove(const FInputActionValue& Value)
{
	if (bIsInventoryOpen)
	{
		return;
	}
	
	ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn());
	if (!MyCharacter)
	{
		return;
	}
	
	FVector2D MovementVector = Value.Get<FVector2D>();
	MyCharacter->DoMove(MovementVector.X, MovementVector.Y);
}

void ACPPSurvivalPlayerController::OnLook(const FInputActionValue& Value)
{
	if (bIsInventoryOpen)
	{
		return;
	}
	
	ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn());
	if (!MyCharacter)
	{
		return;
	}
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	MyCharacter->DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ACPPSurvivalPlayerController::OnJumpStarted()
{
	if (bIsInventoryOpen)
	{
		return;
	}
	
	ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn());
	if (!MyCharacter)
	{
		return;
	}
	MyCharacter->DoJumpStart();
}

void ACPPSurvivalPlayerController::OnJumpCompleted()
{
	ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn());
	if (!MyCharacter)
	{
		return;
	}
	MyCharacter->DoJumpEnd();
}

void ACPPSurvivalPlayerController::OnInteract()
{
	if (bIsInventoryOpen)
	{
		return;
	}
	
	ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn());
	if (!MyCharacter)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector TraceStart = CameraLocation;
	const FVector TraceEnd = TraceStart + (CameraRotation.Vector() * InteractionDistance);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility);

	if (ACPPsurvival_WorldItem* HitItem = Cast<ACPPsurvival_WorldItem>(HitResult.GetActor()))
	{
		if (UInventoryComponent* Inventory = MyCharacter->FindComponentByClass<UInventoryComponent>())
		{
			Inventory->PickupItem(HitItem);
		}
	}
}

void ACPPSurvivalPlayerController::OnToggleInventory()
{
	// Flip the state
	bIsInventoryOpen = !bIsInventoryOpen;
	
	// Lazily get the HUD widget if we don't have it yet
	if (!PlayerHUD)
	{
		if (ACPPsurvivalHUD* HUD = GetHUD<ACPPsurvivalHUD>())
		{
			// We need to create a getter for this in the HUD class.
			PlayerHUD = Cast<UPlayerHUDWidget>(HUD->GetPlayerHUDWidget()); 
		}
	}
	
	if (!PlayerHUD)
	{
		UE_LOG(LogCPPSurvival, Warning, TEXT("Cannot toggle inventory, PlayerHUD is not valid."));
		return;
	}

	if (bIsInventoryOpen)
	{
		PlayerHUD->SetInventoryGridVisibility(true);
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(PlayerHUD->TakeWidget());
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
	else
	{
		PlayerHUD->SetInventoryGridVisibility(false);
		const FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
}