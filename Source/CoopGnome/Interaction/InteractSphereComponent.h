// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "InteractSphereComponent.generated.h"

/**
 * 
 */

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class COOPGNOME_API UInteractSphereComponent : public USphereComponent
{
	GENERATED_BODY()
		
public:
	UInteractSphereComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool Interact();

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TArray<TWeakObjectPtr<AActor>> OverlappedActors;
	TWeakObjectPtr<AActor> CurrentInteractActor;

};
