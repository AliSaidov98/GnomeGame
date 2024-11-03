// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItem.h"
#include "HealthComponent.h"

APickUpItem::APickUpItem()
{
 	PrimaryActorTick.bCanEverTick = true;

}

void APickUpItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickUpItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APickUpItem::Interact_Implementation(AActor* User)
{
	UE_LOG(LogTemp, Warning, TEXT("Interact_Implementation"));
	if (!User)
		return false;
	
	
	return Pickup(User);
}

bool APickUpItem::Pickup(AActor* User)
{
	switch (ItemType)
	{
	case EItemType::Health:
		{
			return HealthPickup(User);
		}
	case EItemType::Weapon:
		{
			//return EquipWeapon();
			break;
		}
	default:
		break;
		;
	}

	return false;
}

bool APickUpItem::HealthPickup(AActor* User)
{
	auto HealthComponent = User->FindComponentByClass<UHealthComponent>();
	if (!HealthComponent)
		return false;

	if (HealthComponent->Heal(HealValue))
	{
		Destroy();
		return true;
	}
	else
	{
		return false;
	}
}


