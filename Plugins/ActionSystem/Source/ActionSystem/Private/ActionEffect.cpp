// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionEffect.h"
#include "ActionComponent.h"

UActionEffect::UActionEffect() : Duration(0), Period(0)
{
	bAutoStart = true;
}

void UActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.f)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UActionEffect::StopAction, Instigator);
		GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, TimerDelegate, Duration, false);
	}

	if (Period > 0.f)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UActionEffect::ExecutePeriodEffect, Instigator);
		GetWorld()->GetTimerManager().SetTimer(PeriodTimerHandle, TimerDelegate, Period, true);
	}
}

void UActionEffect::StopAction_Implementation(AActor* Instigator)
{
	if (GetWorld()->GetTimerManager().GetTimerRemaining(PeriodTimerHandle) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodEffect(Instigator);
	}
	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(PeriodTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);

	if (UActionComponent* Comp = GetOwningComponent())
	{
		Comp->RemoveAction(this);
	}
}

double UActionEffect::GetTimeRemaining() const
{
	const double EndTime = TimeStart - Duration;
	return EndTime - GetWorld()->GetTimeSeconds();
}

/*void UActionEffect::ExecutePeriodEffect_Implementation(AActor* Instigator)
{
}*/


