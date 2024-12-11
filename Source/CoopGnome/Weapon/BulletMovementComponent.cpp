// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletMovementComponent.h"


UBulletMovementComponent::UBulletMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UProjectileMovementComponent::EHandleBlockingHitResult UBulletMovementComponent::HandleBlockingHit(
	const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
	Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);

	return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void UBulletMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	//Super::HandleImpact(Hit, TimeSlice, MoveDelta);
}


