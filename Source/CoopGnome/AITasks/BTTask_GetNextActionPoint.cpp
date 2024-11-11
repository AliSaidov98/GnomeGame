// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetNextActionPoint.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CoopGnome/AI/AIActionPath.h"
#include "CoopGnome/Base/CharacterBase.h"

UBTTask_GetNextActionPoint::UBTTask_GetNextActionPoint(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Get Next Action Point";
}

EBTNodeResult::Type UBTTask_GetNextActionPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* OurBB = OwnerComp.GetBlackboardComponent();
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!OurBB || !MyController)
	{
		return EBTNodeResult::Failed;
	}

	ACharacterBase* MyCharacter = Cast<ACharacterBase>(MyController->GetPawn());
	if (!MyCharacter)
	{
		return EBTNodeResult::Failed;
	}

	AAIActionPath* PathRef = Cast<AAIActionPath>(MyCharacter->SmartObject);
	if (!PathRef || PathRef->ActionActors.IsEmpty())
	{
		return EBTNodeResult::Failed;
	}

	auto ActionActor = PathRef->ActionActors[Index];
	
	if(!IsValid(ActionActor))
		return EBTNodeResult::Failed;
	
	OurBB->SetValueAsObject(cActionActorBBKey, ActionActor);

	if (Index < (PathRef->ActionActors.Num() - 1))
	{
		Index++;
		return EBTNodeResult::Succeeded;
	}
	Index = 0;
	return EBTNodeResult::Succeeded;
}
