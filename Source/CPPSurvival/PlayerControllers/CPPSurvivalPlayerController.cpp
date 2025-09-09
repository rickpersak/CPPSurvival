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
#include "SaveSystem/SaveLoadSubsystem.h"

void ACPPSurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Try to get the HUD and PlayerHUDWidget
	if (ACPPsurvivalHUD* HUD = GetHUD<ACPPsurvivalHUD>())
	{
		PlayerHUD = HUD->GetPlayerHUDWidget();
		UE_LOG(LogCPPSurvival, Warning, TEXT("BeginPlay: PlayerHUD set to %s"), *GetNameSafe(PlayerHUD));
		
		// If PlayerHUD is still null, it might be a timing issue - we'll retry later
		if (!PlayerHUD)
		{
			UE_LOG(LogCPPSurvival, Warning, TEXT("BeginPlay: PlayerHUD is null, will retry during first inventory toggle"));
		}
	}
	else
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("BeginPlay: Failed to get HUD!"));
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
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACPPSurvivalPlayerController::OnSprintStarted);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACPPSurvivalPlayerController::OnSprintCompleted);
	}
	else
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void ACPPSurvivalPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (USaveLoadSubsystem* SaveLoadSubsystem = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>())
	{
		SaveLoadSubsystem->LoadPlayer(this);
	}
}

void ACPPSurvivalPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (USaveLoadSubsystem* SaveLoadSubsystem = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>())
	{
		SaveLoadSubsystem->SavePlayer(this);
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
	if (bIsInventoryOpen)
	{
		CloseContainer();
		return;
	}
	
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
			UE_LOG(LogCPPSurvival, Warning, TEXT("OnInteract: Found container component %s on actor %s"), 
				*GetNameSafe(Container), *GetNameSafe(HitResult.GetActor()));
			UE_LOG(LogCPPSurvival, Warning, TEXT("OnInteract: Container capacity: %d, Container name: %s"), 
				Container->GetCapacity(), *Container->GetContainerName().ToString());
			OpenContainer(Container);
		}
	}
}

void ACPPSurvivalPlayerController::OnToggleInventory()
{
	UE_LOG(LogCPPSurvival, Warning, TEXT("OnToggleInventory called! bIsInventoryOpen = %s"), bIsInventoryOpen ? TEXT("true") : TEXT("false"));
	
	if (bIsInventoryOpen)
	{
		UE_LOG(LogCPPSurvival, Warning, TEXT("Inventory is open, closing container"));
		CloseContainer();
	}
	else
	{
		// Open just the player's inventory
		ACPPSurvivalCharacter* MyCharacter = GetPawn<ACPPSurvivalCharacter>();
		if (!MyCharacter)
		{
			UE_LOG(LogCPPSurvival, Error, TEXT("OnToggleInventory: MyCharacter is null!"));
			return;
		}
		
		UInventoryComponent* InventoryComp = MyCharacter->GetInventoryComponent();
		if (!InventoryComp)
		{
			UE_LOG(LogCPPSurvival, Error, TEXT("OnToggleInventory: InventoryComponent is null!"));
			return;
		}
		
		UE_LOG(LogCPPSurvival, Warning, TEXT("Opening player inventory - Character: %s, InventoryComponent: %s"), 
			*GetNameSafe(MyCharacter), *GetNameSafe(InventoryComp));
		OpenContainer(InventoryComp);
	}
}

void ACPPSurvivalPlayerController::OnSprintStarted()
{
	if (ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn()))
	{
		MyCharacter->StartSprint();
	}
}

void ACPPSurvivalPlayerController::OnSprintCompleted()
{
	if (ACPPSurvivalCharacter* MyCharacter = Cast<ACPPSurvivalCharacter>(GetPawn()))
	{
		MyCharacter->StopSprint();
	}
}

void ACPPSurvivalPlayerController::OpenContainer(UContainerComponent* ContainerToOpen)
{
	UE_LOG(LogCPPSurvival, Warning, TEXT("OpenContainer called - PlayerHUD: %s, ContainerToOpen: %s"), 
		*GetNameSafe(PlayerHUD), *GetNameSafe(ContainerToOpen));
		
	// If PlayerHUD is null, try to get it again (timing issue fix)
	if (!PlayerHUD)
	{
		UE_LOG(LogCPPSurvival, Warning, TEXT("OpenContainer: PlayerHUD is null, attempting to retrieve it again..."));
		if (ACPPsurvivalHUD* HUD = GetHUD<ACPPsurvivalHUD>())
		{
			PlayerHUD = HUD->GetPlayerHUDWidget();
			UE_LOG(LogCPPSurvival, Warning, TEXT("OpenContainer: Retry - PlayerHUD now set to %s"), *GetNameSafe(PlayerHUD));
		}
		
		if (!PlayerHUD)
		{
			UE_LOG(LogCPPSurvival, Error, TEXT("OpenContainer: PlayerHUD is still null after retry!"));
			return;
		}
	}
	
	if (!ContainerToOpen)
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("OpenContainer: ContainerToOpen is null!"));
		return;
	}

	CurrentOpenContainer = ContainerToOpen;
	bIsInventoryOpen = true;
	
	UE_LOG(LogCPPSurvival, Warning, TEXT("Setting bIsInventoryOpen = true, calling SetInventoryContainerVisibility(true)"));

	// Open player inventory
	PlayerHUD->SetInventoryContainerVisibility(true);

	// If the container we're opening is NOT the player's own inventory, open the second panel
	ACPPSurvivalCharacter* MyCharacter = GetPawn<ACPPSurvivalCharacter>();
	if (MyCharacter && CurrentOpenContainer != MyCharacter->GetInventoryComponent())
	{
		UE_LOG(LogCPPSurvival, Warning, TEXT("Opening world container (not player inventory)"));
		PlayerHUD->OpenWorldContainer(CurrentOpenContainer);
	}
	else
	{
		UE_LOG(LogCPPSurvival, Warning, TEXT("Opening player inventory only"));
	}
	
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(PlayerHUD->TakeWidget());
	SetInputMode(InputMode);
	bShowMouseCursor = true;
	
	UE_LOG(LogCPPSurvival, Warning, TEXT("OpenContainer completed - bShowMouseCursor = true, InputMode set to GameAndUI"));
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