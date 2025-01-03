﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetPathPoints.generated.h"

/**
 * 
 */
UCLASS()
class COOPGNOME_API UBTTask_GetPathPoints : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_GetPathPoints(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	int32 Index = 0;
};
