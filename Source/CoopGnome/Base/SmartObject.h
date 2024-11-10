// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SmartObject.generated.h"

UCLASS()
class COOPGNOME_API ASmartObject : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASmartObject();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SmartObject")
	class UBillboardComponent* Billboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SmartObject")
	class UArrowComponent* FacingDirection;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* SubTree;

};
