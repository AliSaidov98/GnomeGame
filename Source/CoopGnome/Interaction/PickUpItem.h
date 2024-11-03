// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"

#include "PickUpItem.generated.h"


// TODO : move to inventory system
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Health,
	Weapon
};


UCLASS()
class COOPGNOME_API APickUpItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:
	APickUpItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	bool Interact_Implementation(AActor* User);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (EditCondition = "ItemType==EItemType::Health", EditConditionHides, ClampMin = 0))
	float HealValue = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (EditCondition = "ItemType==EItemType::Weapon", EditConditionHides))
	TSubclassOf<class AWeaponBase> WeaponClass;

private:
	bool Pickup(AActor* User);
	bool HealthPickup(AActor* User);

};
