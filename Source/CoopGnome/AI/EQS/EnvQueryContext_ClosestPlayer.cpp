// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvQueryContext_ClosestPlayer.h"

#include "CoopGnome/CoopGnomeCharacter.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"


void UEnvQueryContext_ClosestPlayer::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	auto Owner = Cast<AActor>(QueryInstance.Owner.Get());

	if(!Owner)
		return;
	
	//auto Player = UGameplayStatics::GetPlayerCharacter(Owner, 0);
	
	auto PerceptionComponent = Owner->FindComponentByClass<UAIPerceptionComponent>();

	if(!PerceptionComponent)
		return;

	TArray<AActor*> PercievedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PercievedActors);

	if(PercievedActors.IsEmpty())
		return;

	const FVector OwnerLocation = Owner->GetActorLocation(); 
	AActor* ClosestPlayer = nullptr;
	double ClosestDistance = -1;
	AActor* Player = nullptr;
	
	for(auto Actor : PercievedActors)
	{
		if(!Actor->ActorHasTag("Player"))
			continue;

		Player = Actor;
		
		FVector PlayerLocation = Actor->GetActorLocation();
		double DistanceToPlayer = FVector::DistSquared(OwnerLocation, PlayerLocation);

		if(ClosestDistance < 0 || DistanceToPlayer < ClosestDistance)
		{
			ClosestDistance = DistanceToPlayer;
			ClosestPlayer = Actor;
		}
	}
	
	if(!Player)
		return;
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, ClosestPlayer);
}
