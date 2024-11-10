// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "Action.generated.h"

class UActionComponent;

UCLASS(Blueprintable)
class ACTIONSYSTEM_API UAction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Action")
	bool bAutoStart;

	UPROPERTY(EditDefaultsOnly, Category="Action")
	FName ActionName;

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category="Action")
	bool IsRunning() const;

	virtual UWorld* GetWorld() const override;
	
protected:
	bool bIsRunning;
	double TimeStart;

	UFUNCTION(BlueprintCallable, Category="Action")
	UActionComponent* GetOwningComponent();

	UPROPERTY(EditDefaultsOnly, Category="Tag")
	FGameplayTagContainer GrantTags;

	UPROPERTY(EditDefaultsOnly, Category="Tag")
	FGameplayTagContainer BlockTags;

};
