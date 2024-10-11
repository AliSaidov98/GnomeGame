// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedWeapon.h"


#include "Kismet/GameplayStatics.h"

void ARangedWeapon::Attack()
{
	AWeaponBase::Attack();

	PlayMuzzleEffect();
	ShootLineTrace();
}

void ARangedWeapon::ShootLineTrace()
{
	if (ShootStartSocket.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("ARangedWeapon::ShootLineTrace(): ShootStartSocket.IsNone()"));
		return;
	}

	FHitResult HitResult;
	FVector Start = Mesh->GetSocketLocation(ShootStartSocket);
	// TODO: replace hardcoded value with variable
	FVector End = Start + GetActorRightVector() * 10000.f;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1);

	if (!HitResult.IsValidBlockingHit())
		return;

	PlayImpactEffect(HitResult.Location);

	if (AActor* TargetActor = HitResult.GetActor())
		ApplyDamageToActor(TargetActor);
}

void ARangedWeapon::PlayImpactEffect(const FVector& ImpactLocation)
{
	FTransform ParticleTransform;
	ParticleTransform.SetLocation(ImpactLocation);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, ParticleTransform);

}

void ARangedWeapon::PlayMuzzleEffect()
{	
	FTransform ParticleTransform;
	ParticleTransform.SetLocation(Mesh->GetSocketLocation(ShootStartSocket));
	ParticleTransform.SetRotation(FQuat::MakeFromRotator(Mesh->GetSocketRotation(ShootStartSocket)));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFireParticles, ParticleTransform);

}
