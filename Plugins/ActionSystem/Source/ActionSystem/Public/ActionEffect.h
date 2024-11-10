// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "ActionEffect.generated.h"

UCLASS()
class ACTIONSYSTEM_API UActionEffect : public UAction
{
	GENERATED_BODY()

public:
	UActionEffect();

	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;

	UFUNCTION(BlueprintCallable, Category = "Action")
	double GetTimeRemaining() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Effect")
	float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Effect")
	float Period;

	FTimerHandle DurationTimerHandle;
	FTimerHandle PeriodTimerHandle;

	UFUNCTION(BlueprintNativeEvent, Category = "Action Effect")
	void ExecutePeriodEffect(AActor* Instigator);

};
