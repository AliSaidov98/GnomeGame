// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ORManagerComponent.generated.h"

class UORReplicatedObject;


UCLASS(ClassGroup=("Object Replication"), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType,
	DisplayName = "UObject Replication Manager")
class OBJECTREPLICATION_API UORManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UORManagerComponent();

protected:
	//UActorComponent interface implementation
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;;
	//End of implementation
	
	UPROPERTY()
	TArray<UORReplicatedObject*> ReplicatedObjects;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Object Replication",
		DisplayName = "Register Replicated UObject")
	virtual bool RegisterReplicatedObject(
		UPARAM(DisplayName = "Replicated UObject") UORReplicatedObject* ObjectToRegister);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Object Replication",
		DisplayName = "Unregister Replicated UObject")
	virtual bool UnregisterReplicatedObject(
		UPARAM(DisplayName = "Replicated UObject") UORReplicatedObject* ObjectToUnregister,
		UPARAM(DisplayName = "Destroy Replicated UObject") bool bDestroyObject = false);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Object Replication",
		DisplayName = "Get Registered Replicated UObjects")
	virtual TArray<UORReplicatedObject*> GetRegisteredReplicatedObjects()
	{
		return ReplicatedObjects;
	};
};
