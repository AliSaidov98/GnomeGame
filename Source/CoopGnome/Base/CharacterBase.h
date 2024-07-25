// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

UCLASS()
class COOPGNOME_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	class USkeletalMeshComponent* Weapon;

	UPROPERTY(BlueprintReadOnly, Category="AI")
	bool Dead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	float Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
	float BaseDamage = 0.01;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void MoveForward(float Val);
	UFUNCTION(BlueprintCallable)
	void MoveRight(float Val);
	UFUNCTION(BlueprintCallable)
	void LookUp(float Val);
	UFUNCTION(BlueprintCallable)
	void Turn(float Val);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
