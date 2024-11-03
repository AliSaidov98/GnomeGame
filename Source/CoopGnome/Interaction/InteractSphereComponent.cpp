// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractSphereComponent.h"

#include "InteractionInterface.h"

UInteractSphereComponent::UInteractSphereComponent()
{
}

void UInteractSphereComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UInteractSphereComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UInteractSphereComponent::OnEndOverlap);
}

void UInteractSphereComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UInteractSphereComponent::Interact()
{
	if (!IsValid(GetOwner()))
		return false;

	if (CurrentInteractActor.IsValid())
		return IInteractionInterface::Execute_Interact(CurrentInteractActor.Get(), GetOwner());
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT(":: NOT CurrentInteractActor.IsValid() ::")));
	return false;
}

void UInteractSphereComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UInteractionInterface>())
		return;
	
	OverlappedActors.Add(OtherActor);
	CurrentInteractActor = OtherActor;
}

void UInteractSphereComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->Implements<UInteractionInterface>())
		return;

	OverlappedActors.Remove(OtherActor);
	int32 ActorsNum = OverlappedActors.Num();
	if (ActorsNum > 0)
	{
		CurrentInteractActor = OverlappedActors[ActorsNum - 1];
	}
	else
	{
		CurrentInteractActor.Reset();
	}

}

