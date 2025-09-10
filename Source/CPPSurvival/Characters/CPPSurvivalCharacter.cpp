#include "CPPSurvivalCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Components/InventoryComponent.h"
#include "Components/SurvivalStatsComponent.h"
#include "Components/HotbarComponent.h"
#include "CPPSurvival.h"
#include "Net/UnrealNetwork.h"

ACPPSurvivalCharacter::ACPPSurvivalCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	SurvivalStatsComponent = CreateDefaultSubobject<USurvivalStatsComponent>(TEXT("SurvivalStatsComponent"));
	HotbarComponent = CreateDefaultSubobject<UHotbarComponent>(TEXT("HotbarComponent"));

	bIsSprinting = false;
	WalkSpeed = 500.f;
	SprintSpeed = 800.f;
}

void ACPPSurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ACPPSurvivalCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPPSurvivalCharacter, bIsSprinting);
}


void ACPPSurvivalCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ACPPSurvivalCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ACPPSurvivalCharacter::DoJumpStart()
{
	Jump();
}

void ACPPSurvivalCharacter::DoJumpEnd()
{
	StopJumping();
}

void ACPPSurvivalCharacter::StartSprint()
{
	if (HasAuthority())
	{
		bIsSprinting = true;
		OnRep_IsSprinting();
	}
	else
	{
		Server_StartSprint();
	}
}

void ACPPSurvivalCharacter::StopSprint()
{
	if (HasAuthority())
	{
		bIsSprinting = false;
		OnRep_IsSprinting();
	}
	else
	{
		Server_StopSprint();
	}
}

void ACPPSurvivalCharacter::Server_StartSprint_Implementation()
{
	StartSprint();
}

void ACPPSurvivalCharacter::Server_StopSprint_Implementation()
{
	StopSprint();
}

void ACPPSurvivalCharacter::OnRep_IsSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void ACPPSurvivalCharacter::SetSprintStateForUI(bool bNewSprintState)
{
	if (HasAuthority())
	{
		bIsSprinting = bNewSprintState;
		// Don't call OnRep_IsSprinting() since Blueprint handles movement
		// Just replicate the value for stamina tracking
	}
	else
	{
		// Call server RPC if needed
		Server_SetSprintStateForUI(bNewSprintState);
	}
}

void ACPPSurvivalCharacter::Server_SetSprintStateForUI_Implementation(bool bNewSprintState)
{
	SetSprintStateForUI(bNewSprintState);
}