// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ORReplicatedObject.generated.h"

UENUM(BlueprintType)
enum EORReplicationState
{
	//This UObject is considered for replication.
	Replicates,
	//This UObject is not considered for replication.
	DoNotReplicate,
};

UCLASS(Blueprintable, BlueprintType, DisplayName = "Replicated UObject", Abstract)
class OBJECTREPLICATION_API UORReplicatedObject : public UObject
{
	GENERATED_BODY()

public:

	//UObject interface implementation
	virtual UWorld* GetWorld() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;
	//End of implementation
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Replicated UObject", DisplayName = "Destroy Replicated UObject")
	void DestroyObject();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Replicated UObject")
	AActor* GetOwningActor() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replicated UObject")
	TEnumAsByte<EORReplicationState> ReplicationState = EORReplicationState::Replicates;

protected:
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Replicated UObject", DisplayName = "On Replicated UObject Destroyed")
	void OnDestroyed();
};
