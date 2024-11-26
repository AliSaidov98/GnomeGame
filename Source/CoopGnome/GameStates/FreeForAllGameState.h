// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FreeForAllGameState.generated.h"

/**
 * 
 */
UCLASS()
class COOPGNOME_API AFreeForAllGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class ACoopGnomePlayerState* ScoringPLayer);
	
	UPROPERTY(Replicated)
	TArray<class ACoopGnomePlayerState*> TopScoringPlayers;

private:
	float TopScore = 0;

};
