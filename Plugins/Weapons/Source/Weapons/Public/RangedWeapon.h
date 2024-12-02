// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "RangedWeapon.generated.h"


UCLASS(Blueprintable)
class WEAPONS_API ARangedWeapon : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	virtual void Attack() override;

private:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> MuzzleFireParticles;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> ImpactParticles;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	USoundBase* MuzzleSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	USoundBase* ImpactSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Shooting", meta = (AllowPrivateAccess = "true"))
	FName ShootStartSocket;

private:
	void ShootLineTrace();
	void PlayImpactEffect(const FVector& ImpactLocation);
	void PlayMuzzleEffect();

};
