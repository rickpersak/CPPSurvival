// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "SurvivalStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatsUpdated, float, CurrentValue, float, MaxValue);

/**
 * Survival Stats Component - Manages hunger, thirst, and stamina
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CPPSURVIVAL_API USurvivalStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USurvivalStatsComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Replication setup
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Current hunger value (replicated)
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHunger, BlueprintReadOnly, Category = "Survival Stats")
	float CurrentHunger = 100.0f;

	// Current thirst value (replicated)
	UPROPERTY(ReplicatedUsing = OnRep_CurrentThirst, BlueprintReadOnly, Category = "Survival Stats")
	float CurrentThirst = 100.0f;

	// Current stamina value (replicated)
	UPROPERTY(ReplicatedUsing = OnRep_CurrentStamina, BlueprintReadOnly, Category = "Survival Stats")
	float CurrentStamina = 100.0f;

	// Maximum hunger value
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Survival Stats", meta = (ClampMin = "1.0"))
	float MaxHunger = 100.0f;

	// Maximum thirst value
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Survival Stats", meta = (ClampMin = "1.0"))
	float MaxThirst = 100.0f;

	// Maximum stamina value
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Survival Stats", meta = (ClampMin = "1.0"))
	float MaxStamina = 100.0f;

	// Rate at which hunger decreases per second
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Survival Stats", meta = (ClampMin = "0.0"))
	float HungerDecayRate = 1.0f;

	// Rate at which thirst decreases per second
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Survival Stats", meta = (ClampMin = "0.0"))
	float ThirstDecayRate = 1.5f;

	// Rate at which stamina regenerates per second
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Survival Stats", meta = (ClampMin = "0.0"))
	float StaminaRegenRate = 5.0f;
	
	// Rate at which stamina drains per second
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Survival Stats", meta = (ClampMin = "0.0"))
	float StaminaDrainRate = 10.0f;

	void HandleStatDecay();
	void HandleStamina();

public:
	// Events that fire when stats change
	UPROPERTY(BlueprintAssignable, Category = "Survival Stats")
	FOnStatsUpdated OnHungerUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Survival Stats")
	FOnStatsUpdated OnThirstUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Survival Stats")
	FOnStatsUpdated OnStaminaUpdated;

	// Modify hunger (positive to increase, negative to decrease)
	UFUNCTION(BlueprintCallable, Category = "Survival Stats")
	void ModifyHunger(float Amount);

	// Modify thirst (positive to increase, negative to decrease)
	UFUNCTION(BlueprintCallable, Category = "Survival Stats")
	void ModifyThirst(float Amount);

	// Modify stamina (positive to increase, negative to decrease)
	UFUNCTION(BlueprintCallable, Category = "Survival Stats")
	void ModifyStamina(float Amount);

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Survival Stats")
	float GetCurrentHunger() const { return CurrentHunger; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Survival Stats")
	float GetCurrentThirst() const { return CurrentThirst; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Survival Stats")
	float GetCurrentStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Survival Stats")
	float GetMaxHunger() const { return MaxHunger; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Survival Stats")
	float GetMaxThirst() const { return MaxThirst; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Survival Stats")
	float GetMaxStamina() const { return MaxStamina; }

protected:
	// Replication callbacks
	UFUNCTION()
	void OnRep_CurrentHunger();

	UFUNCTION()
	void OnRep_CurrentThirst();

	UFUNCTION()
	void OnRep_CurrentStamina();

	// Helper functions to broadcast updates
	void BroadcastHungerUpdate();
	void BroadcastThirstUpdate();
	void BroadcastStaminaUpdate();

private:
	FTimerHandle StatDecayTimerHandle;
	FTimerHandle StaminaTimerHandle;
};