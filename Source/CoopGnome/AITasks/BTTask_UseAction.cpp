// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_UseAction.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CoopGnome/Actions/ActionActor.h"
#include "CoopGnome/Base/CharacterBase.h"

UBTTask_UseAction::UBTTask_UseAction(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Use Action";
}

EBTNodeResult::Type UBTTask_UseAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* OurBB = OwnerComp.GetBlackboardComponent();
	AAIController* Controller = OwnerComp.GetAIOwner();

	if(!OurBB || !Controller)
	{
		return EBTNodeResult::Failed;
	}

	ACharacterBase* Character = Cast<ACharacterBase>(Controller->GetPawn());

	if(!Character)
	{
		return EBTNodeResult::Failed;
	}

	auto ActionActor = Cast<AActionActor>(OurBB->GetValueAsObject(cActionActorBBKey));

	if(!ActionActor)
		return EBTNodeResult::Failed;

	if(ActionActor->StartAction(Character))
		return EBTNodeResult::Succeeded;
	else
		return EBTNodeResult::Failed;
}
