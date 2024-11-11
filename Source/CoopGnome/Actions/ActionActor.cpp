// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionActor.h"
#include "Action.h"
#include "ActionComponent.h"


// Sets default values
AActionActor::AActionActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AActionActor::BeginPlay()
{
	Super::BeginPlay();
}

void AActionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AActionActor::StartAction(AActor* User)
{
	if(!User)
		return false;

	auto ActionComponent = User->FindComponentByClass<UActionComponent>();

	if(!ActionComponent)
		return false;

	ActionComponent->AddAction(User, Action);
	return true;
}

bool AActionActor::StopAction(AActor* User)
{
	if(!User)
	return false;

	auto ActionComponent = User->FindComponentByClass<UActionComponent>();

	if(!ActionComponent)
		return false;

	FName ActionName = Cast<UAction>(Action.GetDefaultObject())->ActionName;
	return ActionComponent->StopActionByName(User, ActionName);
}
