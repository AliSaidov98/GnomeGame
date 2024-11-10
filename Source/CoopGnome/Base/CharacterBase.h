// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "../CommonDefinitions.h"
#include "CharacterBase.generated.h"

USTRUCT(BlueprintType)
struct FAnimValues
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bIsCrouching = false;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bIsInCombat = false;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bIsShooting = false;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bADS = false;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bIsSitting = false;

};


UCLASS(config=Game)
class COOPGNOME_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:

	ACharacterBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	class USkeletalMeshComponent* Weapon;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "AI")
	EFaction Faction;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "AI")
	ECombatRole CombatRole;

	UPROPERTY(BlueprintReadOnly, Category="AI")
	bool Dead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	float Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	float BaseDamage = 0.01;

	UPROPERTY(BlueprintReadOnly, Category="AI")
	class AAIControllerBase* ControllerRef = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	class UBehaviorTree* BehaviorTreeAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	class ASmartObject* SmartObject = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FAnimValues AnimValues;

	UFUNCTION(BlueprintCallable)
	void PlayMontage(UAnimMontage* Montage, float Rate);

	bool IsHostile(ACharacterBase* Agent);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWidgetRef();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWidgetVis(bool Newbool);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable)
	void MoveForward(float Val);
	UFUNCTION(BlueprintCallable)
	void MoveRight(float Val);
	UFUNCTION(BlueprintCallable)
	void LookUp(float Val);
	UFUNCTION(BlueprintCallable)
	void Turn(float Val);


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SlowWalkSpeed = 94.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 375.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 110.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float CrouchedWalkSpeed = 100.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
	UAnimMontage* HitReactMontage;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
