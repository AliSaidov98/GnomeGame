// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponBase::ApplyDamageToActor(AActor* TargetActor)
{
	if (auto Controller = OwnerCharacter->GetController())
	{
		UGameplayStatics::ApplyDamage(TargetActor, Damage, Controller, this, DamageTypeClass);
	}
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::Attack()
{
	// Play animation
}



