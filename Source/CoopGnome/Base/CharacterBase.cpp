﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "AIControllerBase.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "WeaponBase.h"
#include "CoopGnome/Player/CoopGnomePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	ConstantWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
}


void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACharacterBase, Dead);
}

void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	/*if (LagCompensation)
	{
		LagCompensation->OwnerCharacter = this;
		if (Controller)
		{
			LagCompensation->OwnerController = Cast<ACoopGnomePlayerController>(Controller);
		}
	}*/
}

void ACharacterBase::PlayMontage(UAnimMontage* Montage, float Rate)
{
	if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{GetMesh()->GetAnimInstance()->Montage_Play(Montage, Rate);}
}

bool ACharacterBase::IsHostile(ACharacterBase* Agent)
{
	return (Faction != Agent->Faction && Agent->Faction != EFaction::Neutral);
}



// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	UpdateWidgetRef();
}

/*
float ACharacterBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (ControllerRef)
	{
		ACharacterBase* Damager = Cast<ACharacterBase>(DamageCauser);
		if (Damager && Damager->Faction == Faction)
		{
			return 0.f;
		}
	}

	if (Health <= 0.f) return 0.f;
		
	if(ControllerRef) ControllerRef->BBComponent->SetValueAsBool("Damaged", true);

	FHitResult HitInfo;
	FVector ImpulseDir;
	DamageEvent.GetBestHitInfo(this, DamageCauser, HitInfo, ImpulseDir);

	ACharacterBase* Chr = Cast<ACharacterBase>(HitInfo.GetActor());
	if (!HitInfo.bBlockingHit || Chr == nullptr) return 0.f;

	Chr->Health = (HitInfo.BoneName == "head") ? 0.f : Health - Damage;

	if (Chr->Health <= 0.f)
	{
		//PlayMontage(DeathMontage, 1.f);
		GetMesh()->bIgnoreRadialForce = true;
		//PlayMontage(DeathMontage, 2.f);
		GetMesh()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Dead = true;
		GetCharacterMovement()->bUseRVOAvoidance = false;

		if (ControllerRef)
		{
			ControllerRef->GetBrainComponent()->StopLogic("Agent Is Dead");
			ControllerRef->ClearFocus(EAIFocusPriority::LastFocusPriority);
			ControllerRef->BBComponent->SetValueAsBool("Damaged", false);
		}

		return 0.f;
	}

	PlayMontage(HitReactMontage, 1.f);
	return 0.f;
}*/


void ACharacterBase::MoveForward(float Val)
{
	if(Controller == nullptr || Val == 0.f)
	{
		return;
	}

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	AddMovementInput(Direction, Val);
}

void ACharacterBase::MoveRight(float Val)
{
	if(Controller == nullptr || Val == 0.f)
	{
		return;
	}

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

}

void ACharacterBase::LookUp(float Val)
{
	if(Val == 0.f)
	{
		return;
	}

	AddControllerPitchInput(Val);
}

void ACharacterBase::Turn(float Val)
{
	if(Val == 0.f)
	{
		return;
	}

	AddControllerYawInput(Val);
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

