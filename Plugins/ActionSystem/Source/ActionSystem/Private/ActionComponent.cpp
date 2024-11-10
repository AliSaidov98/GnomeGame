// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionComponent.h"
#include "Action.h"

// Sets default values for this component's properties
UActionComponent::UActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UActionComponent::AddAction(AActor* Instigator, TSubclassOf<UAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}
	if (UAction* NewAction = NewObject<UAction>(this, ActionClass); ensure(NewAction))
	{
		Actions.Add(NewAction);
		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
}

void UActionComponent::RemoveAction(UAction* ActionToRemove)
{
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
	{
		return;
	}
	Actions.Remove(ActionToRemove);
}

void UActionComponent::RemoveAllActions()
{
	if (!Actions.IsEmpty())
	{
		for (UAction* Action : Actions)
		{
			Actions.Remove(Action);
		}
	}
}

UAction* UActionComponent::GetActionByName(FName ActionName)
{
	return nullptr;
}

bool UActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (UAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to start Action: %s "), *ActionName.ToString());
				continue;
			}
			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

bool UActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (UAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning())
			{
				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}


// Called when the game starts
void UActionComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const TSubclassOf<UAction> Action : DefaultActions)
	{
		AddAction(GetOwner(), Action);
	}
}


