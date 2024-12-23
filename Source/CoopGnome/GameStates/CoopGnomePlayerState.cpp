// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopGnomePlayerState.h"
#include "CoopGnome/CoopGnomeCharacter.h"
#include "CoopGnome/Player/CoopGnomePlayerController.h"
#include "Net/UnrealNetwork.h"

void ACoopGnomePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACoopGnomePlayerState, Defeats);
	DOREPLIFETIME(ACoopGnomePlayerState, AnnouncementMessage);
}

void ACoopGnomePlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ACoopGnomeCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACoopGnomePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ACoopGnomePlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<ACoopGnomeCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACoopGnomePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ACoopGnomePlayerState::OnRep_AnnouncementMessage()
{
	Character = Character == nullptr ? Cast<ACoopGnomeCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACoopGnomePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDAnnouncementCountdown(AnnouncementMessage);
		}
	}
}

void ACoopGnomePlayerState::SetAnnouncementMessage(FString Announcement)
{
	AnnouncementMessage = Announcement;
	
	Character = Character == nullptr ? Cast<ACoopGnomeCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACoopGnomePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDAnnouncementCountdown(Announcement);
		}
	}
}

FString ACoopGnomePlayerState::GetAnnouncementMessage()
{
	return AnnouncementMessage;
}


void ACoopGnomePlayerState::ClearAnnouncement()
{
	AnnouncementMessage = "";
}

void ACoopGnomePlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<ACoopGnomeCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACoopGnomePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ACoopGnomePlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<ACoopGnomeCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACoopGnomePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}
