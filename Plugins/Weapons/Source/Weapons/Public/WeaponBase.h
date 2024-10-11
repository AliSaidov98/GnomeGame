// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS(Abstract, NotBlueprintable)
class WEAPONS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void ApplyDamageToActor(AActor* TargetActor);

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Attack();

	TWeakObjectPtr<ACharacter> OwnerCharacter;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	float Damage = 5.f;

	// Attacks per second
	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	float AttackSpeed = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UDamageType> DamageTypeClass;


};
