// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetPathPoints.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "CoopGnome/Base/CharacterBase.h"
#include "CoopGnome/Base/PatrolPath.h"


UBTTask_GetPathPoints::UBTTask_GetPathPoints(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Get Path Points";
}

EBTNodeResult::Type UBTTask_GetPathPoints::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* OurBB = OwnerComp.GetBlackboardComponent();
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

	APatrolPath* PathRef = Cast<APatrolPath>(MyCharacter->SmartObject);
	if (!PathRef || PathRef->PathPoints.Num() < 1)
	{
		return EBTNodeResult::Succeeded;
	}

	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>("MoveToLocation", PathRef->PathPoints[Index]);

	if (Index < (PathRef->PathPoints.Num() - 1))
	{
		Index++;
		return EBTNodeResult::Succeeded;
	}
	Index = 0;
	return EBTNodeResult::Succeeded;

}

