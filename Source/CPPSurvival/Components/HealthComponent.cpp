#include "Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize health on the server.
	if (GetOwner()->HasAuthority())
	{
		CurrentHealth = MaxHealth;
	}
}

void UHealthComponent::TakeDamage(float DamageAmount)
{
	// Ensure this logic only runs on the server.
	if (GetOwner()->HasAuthority() && CurrentHealth > 0.0f)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
		
		if (CurrentHealth <= 0.0f)
		{
			OnDeath.Broadcast();
		}
	}
}