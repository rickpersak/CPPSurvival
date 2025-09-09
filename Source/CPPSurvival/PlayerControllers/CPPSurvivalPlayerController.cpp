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
#include "Components/ContainerComponent.h"

void ACPPSurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ACPPsurvivalHUD* HUD = GetHUD<ACPPsurvivalHUD>())
	{
		PlayerHUD = HUD->GetPlayerHUDWidget();
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
		}
		else 
		{
			UE_LOG(LogCPPSurvival, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}
}

void ACPPSurvivalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACPPSurvivalPlayerController::OnJumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACPPSurvivalPlayerController::OnJumpCompleted);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACPPSurvivalPlayerController::OnMove);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACPPSurvivalPlayerController::OnLook);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACPPSurvivalPlayerController::OnLook);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACPPSurvivalPlayerController::OnInteract);
		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ACPPSurvivalPlayerController::OnToggleInventory);
	}
	else
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void ACPPSurvivalPlayerController::OnMove(const FInputActionValue& Value)
{
	if (bIsInventoryOpen) return;
	
	if (ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn()))
	{
		FVector2D MovementVector = Value.Get<FVector2D>();
		MyCharacter->DoMove(MovementVector.X, MovementVector.Y);
	}
}

void ACPPSurvivalPlayerController::OnLook(const FInputActionValue& Value)
{
	if (bIsInventoryOpen) return;
	
	if (ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn()))
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();
		MyCharacter->DoLook(LookAxisVector.X, LookAxisVector.Y);
	}
}

void ACPPSurvivalPlayerController::OnJumpStarted()
{
	if (bIsInventoryOpen) return;
	
	if (ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn()))
	{
		MyCharacter->DoJumpStart();
	}
}

void ACPPSurvivalPlayerController::OnJumpCompleted()
{
	if (ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn()))
	{
		MyCharacter->DoJumpEnd();
	}
}

void ACPPSurvivalPlayerController::OnInteract()
{
	if (bIsInventoryOpen) return;
	
	ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn());
	if (!MyCharacter) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector TraceStart = CameraLocation;
	const FVector TraceEnd = TraceStart + (CameraRotation.Vector() * InteractionDistance);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility);

	if (HitResult.GetActor())
	{
		// Case 1: Interacting with a world item to pick it up
		if (ACPPsurvival_WorldItem* HitItem = Cast<ACPPsurvival_WorldItem>(HitResult.GetActor()))
		{
			if (UInventoryComponent* Inventory = MyCharacter->GetInventoryComponent())
			{
				Inventory->PickupItem(HitItem);
			}
		}
		// Case 2: Interacting with an actor that has a container component (e.g., a chest)
		else if (UContainerComponent* Container = HitResult.GetActor()->FindComponentByClass<UContainerComponent>())
		{
			OpenContainer(Container);
		}
	}
}

void ACPPSurvivalPlayerController::OnToggleInventory()
{
	if (bIsInventoryOpen)
	{
		CloseContainer();
	}
	else
	{
		// Open just the player's inventory
		ACPPSurvivalCharacter* MyCharacter = GetPawn<ACPPSurvivalCharacter>();
		if (MyCharacter && MyCharacter->GetInventoryComponent())
		{
			OpenContainer(MyCharacter->GetInventoryComponent());
		}
	}
}

void ACPPSurvivalPlayerController::OpenContainer(UContainerComponent* ContainerToOpen)
{
	if (!PlayerHUD || !ContainerToOpen)
	{
		return;
	}

	CurrentOpenContainer = ContainerToOpen;
	bIsInventoryOpen = true;

	// Open player inventory
	PlayerHUD->SetInventoryContainerVisibility(true);

	// If the container we're opening is NOT the player's own inventory, open the second panel
	ACPPSurvivalCharacter* MyCharacter = GetPawn<ACPPSurvivalCharacter>();
	if (MyCharacter && CurrentOpenContainer != MyCharacter->GetInventoryComponent())
	{
		PlayerHUD->OpenWorldContainer(CurrentOpenContainer);
	}
	
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(PlayerHUD->TakeWidget());
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void ACPPSurvivalPlayerController::CloseContainer()
{
	if (!PlayerHUD)
	{
		return;
	}

	bIsInventoryOpen = false;
	CurrentOpenContainer = nullptr;
	
	PlayerHUD->SetInventoryContainerVisibility(false);
	PlayerHUD->CloseWorldContainer();
	
	const FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}