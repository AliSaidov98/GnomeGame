// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_UseAction.generated.h"

/**
 * 
 */
UCLASS()
class COOPGNOME_API UBTTask_UseAction : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_UseAction(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
