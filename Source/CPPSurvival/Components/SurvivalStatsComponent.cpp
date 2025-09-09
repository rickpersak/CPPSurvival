#include "Components/SurvivalStatsComponent.h"
#include "Net/UnrealNetwork.h"
#include "Characters/CPPSurvivalCharacter.h"

USurvivalStatsComponent::USurvivalStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	// So we can tick on the server to decay stats.
	SetIsReplicatedByDefault(true);
}

void USurvivalStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// Server is responsible for initializing the stats and starting the decay timer.
	if (GetOwner()->HasAuthority())
	{
		CurrentHunger = MaxHunger;
		CurrentThirst = MaxThirst;
		CurrentStamina = MaxStamina;

		// Set a repeating timer to call HandleStatDecay every 1.0 second
		GetWorld()->GetTimerManager().SetTimer(
			StatDecayTimerHandle, 
			this, 
			&USurvivalStatsComponent::HandleStatDecay, 
			1.0f, 
			true);

		// Set a repeating timer to call HandleStamina every 0.1 seconds
		GetWorld()->GetTimerManager().SetTimer(
			StaminaTimerHandle,
			this,
			&USurvivalStatsComponent::HandleStamina,
			0.1f,
			true);
	}
}

void USurvivalStatsComponent::HandleStatDecay()
{
	if (GetOwner()->HasAuthority())
	{
		// Since this runs once per second, we just subtract the decay rate directly.
		ModifyHunger(-HungerDecayRate);
		ModifyThirst(-ThirstDecayRate);
	}
}

void USurvivalStatsComponent::HandleStamina()
{
	if (GetOwner()->HasAuthority())
	{
		ACPPSurvivalCharacter* Character = Cast<ACPPSurvivalCharacter>(GetOwner());
		if (Character && Character->IsSprinting())
		{
			ModifyStamina(-StaminaDrainRate * 0.1f);
		}
		else
		{
			ModifyStamina(StaminaRegenRate * 0.1f);
		}
	}
}

void USurvivalStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USurvivalStatsComponent, CurrentHunger);
	DOREPLIFETIME(USurvivalStatsComponent, CurrentThirst);
	DOREPLIFETIME(USurvivalStatsComponent, CurrentStamina);
}

void USurvivalStatsComponent::ModifyHunger(float Amount)
{
	if (GetOwner()->HasAuthority())
	{
		const float OldHunger = CurrentHunger;
		CurrentHunger = FMath::Clamp(CurrentHunger + Amount, 0.f, MaxHunger);
		
		// If the value changed, broadcast the update on the server.
		// Clients will get the update via the OnRep function.
		if (OldHunger != CurrentHunger)
		{
			BroadcastHungerUpdate();
		}
	}
}

void USurvivalStatsComponent::ModifyThirst(float Amount)
{
	if (GetOwner()->HasAuthority())
	{
		const float OldThirst = CurrentThirst;
		CurrentThirst = FMath::Clamp(CurrentThirst + Amount, 0.f, MaxThirst);

		if (OldThirst != CurrentThirst)
		{
			BroadcastThirstUpdate();
		}
	}
}

void USurvivalStatsComponent::ModifyStamina(float Amount)
{
	if (GetOwner()->HasAuthority())
	{
		const float OldStamina = CurrentStamina;
		CurrentStamina = FMath::Clamp(CurrentStamina + Amount, 0.f, MaxStamina);

		if (OldStamina != CurrentStamina)
		{
			BroadcastStaminaUpdate();
		}
	}
}

void USurvivalStatsComponent::OnRep_CurrentHunger()
{
	BroadcastHungerUpdate();
}

void USurvivalStatsComponent::OnRep_CurrentThirst()
{
	BroadcastThirstUpdate();
}

void USurvivalStatsComponent::OnRep_CurrentStamina()
{
	BroadcastStaminaUpdate();
}

void USurvivalStatsComponent::BroadcastHungerUpdate()
{
	OnHungerUpdated.Broadcast(CurrentHunger, MaxHunger);
}

void USurvivalStatsComponent::BroadcastThirstUpdate()
{
	OnThirstUpdated.Broadcast(CurrentThirst, MaxThirst);
}

void USurvivalStatsComponent::BroadcastStaminaUpdate()
{
	OnStaminaUpdated.Broadcast(CurrentStamina, MaxStamina);
}