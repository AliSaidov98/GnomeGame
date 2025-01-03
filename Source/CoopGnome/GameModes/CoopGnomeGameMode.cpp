// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoopGnomeGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "CoopGnome/CoopGnomeCharacter.h"
#include "CoopGnome/GameStates/FreeForAllGameState.h"
#include "CoopGnome/GameStates/CoopGnomePlayerState.h"
#include "CoopGnome/Player/CoopGnomePlayerController.h"
#include "UObject/ConstructorHelpers.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ACoopGnomeGameMode::ACoopGnomeGameMode()
{
	bDelayedStart = true;
}

void ACoopGnomeGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ACoopGnomeGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			//EndMatch();
			SetMatchState(MatchState::WaitingPostMatch);
			//StartToLeaveMap();
			//RestartGame();
		}
	}
}

void ACoopGnomeGameMode::PlayerEliminated(class ACoopGnomeCharacter* ElimmedCharacter,
	class ACoopGnomePlayerController* VictimController, ACoopGnomePlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;

	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	
	ACoopGnomePlayerState* AttackerPlayerState = AttackerController ? Cast<ACoopGnomePlayerState>(AttackerController->PlayerState) : nullptr;
	ACoopGnomePlayerState* VictimPlayerState = VictimController ? Cast<ACoopGnomePlayerState>(VictimController->PlayerState) : nullptr;
	
	AFreeForAllGameState* FreeForAllGameState = GetGameState<AFreeForAllGameState>();
	
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && FreeForAllGameState)
	{
		TArray<ACoopGnomePlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : FreeForAllGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		AttackerPlayerState->AddToScore(1.f);
		FreeForAllGameState->UpdateTopScore(AttackerPlayerState);
		if (FreeForAllGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			ACoopGnomeCharacter* Leader = Cast<ACoopGnomeCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				// MulticastGainedTheLead();
			}
		}

		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!FreeForAllGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				ACoopGnomeCharacter* Loser = Cast<ACoopGnomeCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					// MulticastLostTheLead();
				}
			}
		}
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
		VictimPlayerState->SetAnnouncementMessage("You were eliminated");
	}

	if (ElimmedCharacter)
	{
		// ElimmedCharacter->Elim(false);
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACoopGnomePlayerController* PC = Cast<ACoopGnomePlayerController>(*It);
		if (PC && AttackerPlayerState && VictimPlayerState)
		{
			PC->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}
}

void ACoopGnomeGameMode::AIEliminated(ACoopGnomeCharacter* ElimmedCharacter,
	ACoopGnomePlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;

	ACoopGnomePlayerState* AttackerPlayerState = AttackerController ? Cast<ACoopGnomePlayerState>(AttackerController->PlayerState) : nullptr;
	
	AFreeForAllGameState* FreeForAllGameState = GetGameState<AFreeForAllGameState>();
	
	if (AttackerPlayerState && FreeForAllGameState)
	{
		TArray<ACoopGnomePlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : FreeForAllGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		AttackerPlayerState->AddToScore(1.f);
		FreeForAllGameState->UpdateTopScore(AttackerPlayerState);
	}
}

void ACoopGnomeGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if(GetMatchState() == MatchState::Cooldown || GetMatchState() == MatchState::WaitingPostMatch) return;
	
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

void ACoopGnomeGameMode::PlayerLeftGame(class ACoopGnomePlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	
	AFreeForAllGameState* FreeForAllGameState = GetGameState<AFreeForAllGameState>();
	if (FreeForAllGameState && FreeForAllGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		FreeForAllGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	
	ACoopGnomeCharacter* CharacterLeaving = Cast<ACoopGnomeCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->bLeftGame = true;
		CharacterLeaving->Elim_Server();
	}
}

float ACoopGnomeGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

void ACoopGnomeGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACoopGnomePlayerController* PlayerController = Cast<ACoopGnomePlayerController>(*It);
		if (PlayerController)
		{
			PlayerController->OnMatchStateSet(MatchState);
		}
	}
}
