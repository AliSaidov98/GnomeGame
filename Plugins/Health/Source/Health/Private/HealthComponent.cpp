// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Health.h"


UHealthComponent::UHealthComponent(): ArmorValue(0)
{
	PrimaryComponentTick.bCanEverTick = true;

	CurrentHealth = MaxHealth;
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner())
		return;

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);

}


void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UHealthComponent::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	ReceiveDamage(Damage);
}

void UHealthComponent::ReceiveDamage(float Damage)
{
	if (bDead)
		return;

	if (Damage <= 0)
	{
		UE_LOG(LogHealth, Warning, TEXT("UHealthComponent::ReceiveDamage(): Damage < 0"));
		return;
	}

	Damage = Damage - Damage * ArmorValue;

	if (Damage <= 0)
		return;
	
	CurrentHealth = FMath::Max(0.f, (CurrentHealth - Damage));

	OnHealthChange.Broadcast(CurrentHealth, MaxHealth);

	if (FMath::IsNearlyEqual(CurrentHealth, 0.f, 0.01f))
	{
		Death();
	}
}

bool UHealthComponent::Heal(float HealValue)
{
	if (HealValue <= 0)
	{
		UE_LOG(LogHealth, Warning, TEXT("UHealthComponent::Heal(): HealValue <= 0"));
		return false;
	}
	UE_LOG(LogHealth, Warning, TEXT("HealHealValue"));
		
	if (CurrentHealth >= MaxHealth)
		return false;

	CurrentHealth = FMath::Min(MaxHealth, (CurrentHealth + HealValue));
	OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
	return true;
}

void UHealthComponent::SetMaxHealth(float NewMaxHealth)
{
	if (NewMaxHealth <= 0)
	{
		UE_LOG(LogHealth, Warning, TEXT("UHealthComponent::SetMaxHealth(): NewMaxHealth <= 0"));
		return;
	}

	MaxHealth = NewMaxHealth;
	OnHealthChange.Broadcast(CurrentHealth, MaxHealth);

}

void UHealthComponent::Death()
{
	bDead = true;

	OnDeath.Broadcast();

	if (bDestroyActorOnDeath)
		GetOwner()->Destroy();
}

