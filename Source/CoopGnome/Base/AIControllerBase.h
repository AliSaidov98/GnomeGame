// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class COOPGNOME_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	AAIControllerBase();

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(Transient)
	class UBehaviorTreeComponent* BTComponent;

	UPROPERTY(Transient)
	class UBlackboardComponent* BBComponent;

	UPROPERTY(BlueprintReadWrite)
	class ACharacterBase* Agent = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float DetectionLevel = 0.f;

protected:
	virtual void BeginPlay() override;

};
