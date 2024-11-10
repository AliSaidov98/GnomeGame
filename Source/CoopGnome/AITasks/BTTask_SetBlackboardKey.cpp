// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetBlackboardKey.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CoopGnome/Base/AIControllerBase.h"

UBTTask_SetBlackboardKey::UBTTask_SetBlackboardKey()
{
	bCreateNodeInstance = true;
	NodeName = "Set Blackboard Key";
}

EBTNodeResult::Type UBTTask_SetBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* MyController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (MyController != nullptr)
	{
		MyController->BBComponent->SetValueAsBool("Damaged", Damage);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

