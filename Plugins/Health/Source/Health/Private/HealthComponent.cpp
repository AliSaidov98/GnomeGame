// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Health.h"/*
#include "../../../../../Source/CoopGnome/CoopGnomeCharacter.h"
#include "../../../../../Source/CoopGnome/GameModes/CoopGnomeGameMode.h"
#include "../../../../../Source/CoopGnome/Player/CoopGnomePlayerController.h"*/
#include "Net/UnrealNetwork.h"

/*
class ACoopGnomeGameMode;
class ACoopGnomeCharacter;
class ACoopGnomePlayerController;*/

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

	if(GetOwner()->HasAuthority())
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}


void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UHealthComponent::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	/*
	CoopGnomeGameMode = CoopGnomeGameMode == nullptr ? GetWorld()->GetAuthGameMode<ACoopGnomeGameMode>() : CoopGnomeGameMode;

	if(!CoopGnomeGameMode) return;

	ACoopGnomePlayerController* CharacterPlayerController = Cast<ACoopGnomePlayerController>(Controller);
	ACoopGnomePlayerController* AttackerController = Cast<ACoopGnomePlayerController>(GetInstigatorController());
	
	CoopGnomeGameMode->PlayerEliminated(this, CharacterPlayerController, AttackerController);
	*/

	Instigator = InstigatedBy;
	
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

void UHealthComponent::OnRep_CurrentHealth()
{
	OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::Death_Implementation()
{
	bDead = true;

	OnDeath.Broadcast();

	
	/*
	ACoopGnomeGameMode* GnomeGameMode = GetWorld()->GetAuthGameMode<ACoopGnomeGameMode>();

	if(!GnomeGameMode) return;

	ACoopGnomeCharacter* Character = Cast<ACoopGnomeCharacter>(GetOwner());
	ACoopGnomePlayerController* GnomeGameController = Cast<ACoopGnomePlayerController>(Character->GetController());
	ACoopGnomePlayerController* AttackerController = Cast<ACoopGnomePlayerController>(Character->GetInstigator());
	
	GnomeGameMode->PlayerEliminated(Character, GnomeGameController, AttackerController);*/
	/*if (bDestroyActorOnDeath)
		GetOwner()->Destroy();*/
}

