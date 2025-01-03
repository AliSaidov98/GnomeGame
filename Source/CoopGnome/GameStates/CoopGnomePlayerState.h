﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CoopGnomePlayerState.generated.h"

UCLASS()
class COOPGNOME_API ACoopGnomePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();
	
	UFUNCTION()
	virtual void OnRep_AnnouncementMessage();

	UFUNCTION(BlueprintCallable)
	void SetAnnouncementMessage(FString AnnouncementMessage);

	UFUNCTION(BlueprintCallable)
	FString GetAnnouncementMessage();
	
	void ClearAnnouncement();
	
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);
	
private:
	UPROPERTY()
	class ACoopGnomeCharacter* Character;
	UPROPERTY()
	class ACoopGnomePlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	UPROPERTY(ReplicatedUsing = OnRep_AnnouncementMessage)
	FString AnnouncementMessage;
	
};
