// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase.h"
#include "CharacterBase.h"
#include "SmartObject.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


// Sets default values
AAIControllerBase::AAIControllerBase()
{
	BBComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	BTComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ACharacterBase* CharacterBase = Cast<ACharacterBase>(InPawn);
	if (CharacterBase != nullptr && CharacterBase->BehaviorTreeAsset != nullptr)
	{
		Agent = CharacterBase;
		Agent->ControllerRef = this;

		BBComponent->InitializeBlackboard(*CharacterBase->BehaviorTreeAsset->BlackboardAsset);
		BTComponent->StartTree(*CharacterBase->BehaviorTreeAsset);
	}
}

// Called when the game starts or when spawned
void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (!Agent)
	{
		ACharacterBase* CharacterBase = Cast<ACharacterBase>(GetPawn());
		if (!CharacterBase) return;

		Agent = CharacterBase;
		Agent->ControllerRef = this;
	}

	if (Agent->SmartObject)
	{
		FGameplayTag SubTag;
		BTComponent->SetDynamicSubtree(SubTag, Agent->SmartObject->SubTree);
	}

	BBComponent->SetValueAsEnum("CombatRole", (uint8)Agent->CombatRole);
}

