// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActionActor.generated.h"

class UAction;

UCLASS()
class COOPGNOME_API AActionActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActionActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool StartAction(AActor* User);
	bool StopAction(AActor* User);

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings")
	TSubclassOf<UAction> Action;	
};
