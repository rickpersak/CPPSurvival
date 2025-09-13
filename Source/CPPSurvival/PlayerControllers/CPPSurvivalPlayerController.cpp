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
#include "SaveSystem/SaveLoadSubsystem.h"
#include "Characters/EnemyCharacter.h"
#include "Components/HealthComponent.h"
#include "CollisionQueryParams.h"
#include "Kismet/KismetSystemLibrary.h"

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

void ACPPSurvivalPlayerController::Server_MoveItem_Implementation(UContainerComponent* SourceContainer, int32 SourceIndex, UContainerComponent* TargetContainer, int32 TargetIndex)
{
	if (SourceContainer && TargetContainer)
	{
		SourceContainer->MoveItem(SourceIndex, TargetContainer, TargetIndex);
	}
}

void ACPPSurvivalPlayerController::Server_ShiftClickItem_Implementation(UContainerComponent* SourceContainer, int32 SourceIndex)
{
	if (!SourceContainer) return;

	ACPPSurvivalCharacter* MyCharacter = GetPawn<ACPPSurvivalCharacter>();
	if (!MyCharacter) return;

	UInventoryComponent* PlayerInventory = MyCharacter->GetInventoryComponent();
	if (!PlayerInventory) return;
	
	// If shift-clicking from player inventory, move to world container, and vice-versa
	if (SourceContainer == PlayerInventory)
	{
		// Moving from player inventory to open container
		if (CurrentOpenContainer && CurrentOpenContainer != PlayerInventory)
		{
			bool bSuccess = SourceContainer->MoveItemToFirstAvailableSlot(SourceIndex, CurrentOpenContainer);
			UE_LOG(LogCPPSurvival, Warning, TEXT("Shift-click from player inventory to container: %s"), bSuccess ? TEXT("Success") : TEXT("Failed"));
		}
	}
	else
	{
		// Moving from container to player inventory
		bool bSuccess = SourceContainer->MoveItemToFirstAvailableSlot(SourceIndex, PlayerInventory);
		UE_LOG(LogCPPSurvival, Warning, TEXT("Shift-click from container to player inventory: %s"), bSuccess ? TEXT("Success") : TEXT("Failed"));
	}
}

void ACPPSurvivalPlayerController::Server_DealDamage_Implementation(AEnemyCharacter* DamagedEnemy, float DamageAmount)
{
	UE_LOG(LogCPPSurvival, Warning, TEXT("[SERVER] Server_DealDamage_Implementation called for %s"), *GetNameSafe(DamagedEnemy));
	if (DamagedEnemy)
	{
		// The server finds the health component and applies the damage.
		if (UHealthComponent* HealthComponent = DamagedEnemy->FindComponentByClass<UHealthComponent>())
		{
			UE_LOG(LogCPPSurvival, Warning, TEXT("[SERVER] Found HealthComponent. Calling TakeDamage..."));
			HealthComponent->TakeDamage(DamageAmount);
		}
		else
		{
			UE_LOG(LogCPPSurvival, Error, TEXT("[SERVER] FAILED to find HealthComponent on %s"), *GetNameSafe(DamagedEnemy));
		}
	}
	else
	{
		UE_LOG(LogCPPSurvival, Error, TEXT("[SERVER] DamagedEnemy is NULL."));
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
	UE_LOG(LogCPPSurvival, Log, TEXT("[CLIENT] OnInteract called."));
	
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
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MyCharacter);
	
	UKismetSystemLibrary::SphereTraceSingle(
		this,
		TraceStart,
		TraceEnd,
		InteractionSphereRadius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true
	);

	if (AActor* HitActor = HitResult.GetActor())
	{
		UE_LOG(LogCPPSurvival, Log, TEXT("[CLIENT] Sphere trace HIT actor: %s at location: %s"), *GetNameSafe(HitActor), *HitResult.ImpactPoint.ToString());
		
		// Check if the actor has a container component first (for dead enemies and regular containers)
		if (UContainerComponent* Container = HitActor->FindComponentByClass<UContainerComponent>())
		{
			// If it's an enemy, check if it's dead before opening container
			if (AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(HitActor))
			{
				if (UHealthComponent* HealthComp = HitEnemy->FindComponentByClass<UHealthComponent>())
				{
					if (HealthComp->GetCurrentHealth() <= 0.0f)
					{
						UE_LOG(LogCPPSurvival, Log, TEXT("[CLIENT] Dead enemy detected. Opening loot container..."));
						OpenContainer(Container);
					}
					else
					{
						UE_LOG(LogCPPSurvival, Warning, TEXT("[CLIENT] Living enemy detected. Dealing damage..."));
						Server_DealDamage(HitEnemy, 25.0f);
					}
				}
			}
			else
			{
				// Not an enemy, just a regular container
				UE_LOG(LogCPPSurvival, Log, TEXT("[CLIENT] Regular container detected. Opening..."));
				OpenContainer(Container);
			}
		}
		// Check if the actor is a world item to pick it up
		else if (ACPPsurvival_WorldItem* HitItem = Cast<ACPPsurvival_WorldItem>(HitActor))
		{
			if (UInventoryComponent* Inventory = MyCharacter->GetInventoryComponent())
			{
				Inventory->PickupItem(HitItem);
			}
		}
		// Fallback: if it's an enemy without a container component (shouldn't happen normally)
		else if (AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(HitActor))
		{
			UE_LOG(LogCPPSurvival, Warning, TEXT("[CLIENT] Enemy without container component detected. Dealing damage..."));
			Server_DealDamage(HitEnemy, 25.0f);
		}
	}
	else
	{
		UE_LOG(LogCPPSurvival, Log, TEXT("[CLIENT] Sphere trace did NOT hit anything."));
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