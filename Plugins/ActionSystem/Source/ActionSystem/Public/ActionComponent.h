// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "ActionComponent.generated.h"


class UAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionStateChange, UActionComponent*, OwningComp, UAction*, Action);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONSYSTEM_API UActionComponent : public UActorComponent
{
	
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UActionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag")
	FGameplayTagContainer ActiveTags;

	UPROPERTY(BlueprintAssignable, Category="Action")
	FOnActionStateChange OnActionStart;

	UPROPERTY(BlueprintAssignable, Category="Action")
	FOnActionStateChange OnActionStop;

	UFUNCTION(BlueprintCallable, Category="Action")
	void AddAction(AActor* Instigator, TSubclassOf<UAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category="Action")
	void RemoveAction(UAction* ActionToRemove);

	UFUNCTION(BlueprintCallable, Category="Action")
	void RemoveAllActions();

	UFUNCTION(BlueprintCallable, Category="Action")
	UAction* GetActionByName(FName ActionName);

	UFUNCTION(BlueprintCallable, Category="Action")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category="Action")
	bool StopActionByName(AActor* Instigator, FName ActionName);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	TArray<UAction*> Actions;

	UPROPERTY(EditAnywhere, Category="Action")
	TArray<TSubclassOf<UAction>> DefaultActions;

};
