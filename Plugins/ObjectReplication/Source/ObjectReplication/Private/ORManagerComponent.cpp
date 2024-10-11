// Fill out your copyright notice in the Description page of Project Settings.
#include "ORManagerComponent.h"
#include "ORReplicatedObject.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Engine/ActorChannel.h"

UORManagerComponent::UORManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	bReplicateUsingRegisteredSubObjectList = true;
}

bool UORManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
}

bool UORManagerComponent::RegisterReplicatedObject(UORReplicatedObject* ObjectToRegister)
{
	if (IsValid(ObjectToRegister))
	{
		ReplicatedObjects.AddUnique(ObjectToRegister);
		AddReplicatedSubObject(ObjectToRegister);
		return true;
	}

	return false;
}

bool UORManagerComponent::UnregisterReplicatedObject(UORReplicatedObject* ObjectToUnregister, bool bDestroyObject)
{
	if (IsValid(ObjectToUnregister))
	{
		ReplicatedObjects.Remove(ObjectToUnregister);
		RemoveReplicatedSubObject(ObjectToUnregister);

		if (bDestroyObject)
		{
			ObjectToUnregister->DestroyObject();
		}
		
		return true;
	}

	return false;
}

