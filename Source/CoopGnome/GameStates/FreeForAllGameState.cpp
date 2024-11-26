// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeForAllGameState.h"
#include "CoopGnomePlayerState.h"
#include "Net/UnrealNetwork.h"

void AFreeForAllGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFreeForAllGameState, TopScoringPlayers);
}

void AFreeForAllGameState::UpdateTopScore(class ACoopGnomePlayerState* ScoringPLayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPLayer);
		TopScore = ScoringPLayer->GetScore();
	}
	else if (TopScore == ScoringPLayer->GetScore())
	{
		TopScoringPlayers.AddUnique(ScoringPLayer);
	}
	else if (ScoringPLayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPLayer);
		TopScore = ScoringPLayer->GetScore();
	}
}
