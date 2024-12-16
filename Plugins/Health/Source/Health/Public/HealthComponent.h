// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange, float, CurrentHealth, float, MaxHealth);

UCLASS( ClassGroup=(Health), meta=(BlueprintSpawnableComponent) )
class HEALTH_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChange OnHealthChange;

	UPROPERTY()
	AController* Instigator;
	
protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
		
	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(float Damage);

	UFUNCTION(BlueprintCallable)
	bool Heal(float HealValue);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDead() const { return bDead; }

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float NewMaxHealth);

	UFUNCTION()
	void OnRep_CurrentHealth();

private:
	UFUNCTION(NetMulticast, Reliable)
	void Death();

	UPROPERTY(EditAnywhere, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Death", meta = (AllowPrivateAccess = "true"))
	bool bDestroyActorOnDeath = false;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, VisibleAnywhere, Category = "Health")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = "Health", meta = (AllowPrivateAccess = "true", ClampMin = 0, ClampMax = 1))
	float ArmorValue;

	bool bDead = false;

};
