// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopGnomePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "CoopGnome/CoopGnomeCharacter.h"
#include "CoopGnome/Components/CombatComponent.h"
#include "CoopGnome/GameModes/CoopGnomeGameMode.h"
#include "CoopGnome/GameStates/CoopGnomePlayerState.h"
#include "CoopGnome/GameStates/FreeForAllGameState.h"
#include "CoopGnome/Types/Announcement.h"
#include "CoopGnome/UI/AnnounceWidget.h"
#include "CoopGnome/UI/GameHUD.h"
#include "CoopGnome/UI/ReturnToMainMenu.h"
#include "CoopGnome/UI/ScoreWidget.h"
#include "GameFramework/CharacterMovementComponent.h"

void ACoopGnomePlayerController::SetHUDHealth()
{
}

void ACoopGnomePlayerController::SetHUDShield(float Shield, float MaxShield)
{
}

void ACoopGnomePlayerController::SetHUDScore(float Score)
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	
	bool bHUDValid = GameHUD &&
		GameHUD->InterfaceWidget &&
		GameHUD->InterfaceWidget->ScoreWidget;

	if(!bHUDValid) return;
	
	UScoreWidget* ScoreWidget = Cast<UScoreWidget>(GameHUD->InterfaceWidget->ScoreWidget);
	
	bool bHUDPropertyValid = ScoreWidget->ScoreAmount && ScoreWidget->ScoreText;

	if(bHUDValid && bHUDPropertyValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("ScoreText"));
		
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt32(Score));
		
		ScoreWidget->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void ACoopGnomePlayerController::SetHUDDefeats(int32 Defeats)
{
}

void ACoopGnomePlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
}

void ACoopGnomePlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
}

void ACoopGnomePlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	
}

void ACoopGnomePlayerController::SetHUDAnnouncementCountdown(FString AnnounceText)
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	
	bool bHUDValid = GameHUD &&
		GameHUD->InterfaceWidget &&
		GameHUD->InterfaceWidget->AnnounceWidget;

	if(!bHUDValid) return;
	
	UAnnounceWidget* AnnounceWidget = Cast<UAnnounceWidget>(GameHUD->InterfaceWidget->AnnounceWidget);
	
	bool bHUDPropertyValid = IsValid(AnnounceWidget->AnnounceText);

	if(bHUDValid && bHUDPropertyValid)
	{
		AnnounceWidget->AnnounceText->SetText(FText::FromString(AnnounceText));
	}
}

void ACoopGnomePlayerController::SetHUDGrenades(int32 Grenades)
{
}


void ACoopGnomePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);	
}

void ACoopGnomePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	CheckPing(DeltaTime);
}

void ACoopGnomePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACoopGnomePlayerController, MatchState);
}

float ACoopGnomePlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ACoopGnomePlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ACoopGnomePlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}	
}

void ACoopGnomePlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ACoopGnomePlayerController::HandleMatchHasStarted()
{
	GnomeCharacter = GnomeCharacter ? GnomeCharacter : Cast<ACoopGnomeCharacter>(GetPawn());
	if (GnomeCharacter)
	{
		ACoopGnomePlayerState* GnomePlayerState = Cast<ACoopGnomePlayerState>(PlayerState);
		if(GnomePlayerState)
			GnomePlayerState->SetAnnouncementMessage("");
	}
}

void ACoopGnomePlayerController::HandleCooldown()
{
	GnomeCharacter = GnomeCharacter ? GnomeCharacter : Cast<ACoopGnomeCharacter>(GetPawn());
	
	if (GnomeCharacter)
	{
		GnomeCharacter->bDisableGameplay = true;
		
		GnomeCharacter->GetCharacterMovement()->StopMovementImmediately();
		GnomeCharacter->GetCharacterMovement()->DisableMovement();

		GnomeCharacter->GetCombat()->FireButtonPressed(false);

		ACoopGnomePlayerState* GnomePlayerState = Cast<ACoopGnomePlayerState>(PlayerState);
		AFreeForAllGameState* FreeForAllGameState = Cast<AFreeForAllGameState>(UGameplayStatics::GetGameState(this));

		if(!FreeForAllGameState || !GnomePlayerState) return;

		TArray<ACoopGnomePlayerState*> TopPlayers = FreeForAllGameState->TopScoringPlayers;

		
		FString TopPlayerString = GetInfoText(TopPlayers);
		
		/*
		if(TopPlayers.Num() == 0)
		{
			TopPlayerString = FString("There is no winner.");
		}
		else if(TopPlayers.Num() == 1 && TopPlayers[0] == GnomePlayerState)
		{
			TopPlayerString = FString("You are the winner!");
		}
		else if(TopPlayers.Num() == 1)
		{
			TopPlayerString = FString::Printf(TEXT("%s is the winner!"), *TopPlayers[0]->GetPlayerName());
		}
		else if(TopPlayers.Num() > 1)
		{
			TopPlayerString = FString("Players tied for the win:\n");

			for (auto TiedPlayer : TopPlayers)
			{
				TopPlayerString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
			}
		}*/
		
		GnomePlayerState->SetAnnouncementMessage(TopPlayerString);
 	}
}

void ACoopGnomePlayerController::BroadcastElim(APlayerState* Attacker, APlayerState* Victim)
{
	ClientElimAnnouncement(Attacker, Victim);
}

void ACoopGnomePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	GameHUD = Cast<AGameHUD>(GetHUD());

	ServerCheckMatchState();
}

void ACoopGnomePlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	
	if (HasAuthority())
	{
		if (CoopGnomeGameMode == nullptr)
		{
			CoopGnomeGameMode = Cast<ACoopGnomeGameMode>(UGameplayStatics::GetGameMode(this));
			LevelStartingTime = CoopGnomeGameMode->LevelStartingTime;
		}
		CoopGnomeGameMode = CoopGnomeGameMode == nullptr ? Cast<ACoopGnomeGameMode>(UGameplayStatics::GetGameMode(this)) : CoopGnomeGameMode;
		if (CoopGnomeGameMode)
		{
			SecondsLeft = FMath::CeilToInt(CoopGnomeGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart/* || MatchState == MatchState::Cooldown*/)
		{
			SetHUDAnnouncementCountdown("Waiting");
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}

	CountdownInt = SecondsLeft;
}

void ACoopGnomePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;

	InputComponent->BindAction("Quit", IE_Pressed, this, &ACoopGnomePlayerController::ShowReturnToMainMenu);
}


void ACoopGnomePlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ACoopGnomePlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void ACoopGnomePlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ACoopGnomePlayerController::ServerCheckMatchState_Implementation()
{
	if (ACoopGnomeGameMode* GameMode = Cast<ACoopGnomeGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

void ACoopGnomePlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match,
	float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
}

void ACoopGnomePlayerController::HighPingWarning()
{
}

void ACoopGnomePlayerController::StopHighPingWarning()
{
}

void ACoopGnomePlayerController::CheckPing(float DeltaTime)
{
	if (HasAuthority()) return;
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		TObjectPtr<APlayerState> Self = GetPlayerState<APlayerState>();
		PlayerState = PlayerState == nullptr ? Self : PlayerState;
		if (PlayerState)
		{
			if (PlayerState->GetCompressedPing() * 4 > HighPingThreshold) // ping is compressed; it's actually ping / 4
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
				ServerReportPingStatus(true);
			}
			else
			{
				ServerReportPingStatus(false);
			}
		}
		HighPingRunningTime = 0.f;
	}
	bool bHighPingAnimationPlaying = true;
	if (bHighPingAnimationPlaying)
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

void ACoopGnomePlayerController::ShowReturnToMainMenu()
{
	if (ReturnToMainMenuWidget == nullptr) return;
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UReturnToMainMenu>(this, ReturnToMainMenuWidget);
	}
	if (ReturnToMainMenu)
	{
		bReturnToMainMenuOpen = !bReturnToMainMenuOpen;
		if (bReturnToMainMenuOpen)
		{
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}
}

FString ACoopGnomePlayerController::GetInfoText(const TArray<ACoopGnomePlayerState*>& Players)
{
	ACoopGnomePlayerState* CoopGnomePlayerState = GetPlayerState<ACoopGnomePlayerState>();
	if (CoopGnomePlayerState == nullptr) return FString();
	FString InfoTextString;
	if (Players.Num() == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;
	}
	else if (Players.Num() == 1 && Players[0] == CoopGnomePlayerState)
	{
		InfoTextString = Announcement::YouAreTheWinner;
	}
	else if (Players.Num() == 1)
	{
		InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *Players[0]->GetPlayerName());
	}
	else if (Players.Num() > 1)
	{
		InfoTextString = Announcement::PlayersTiedForTheWin;
		InfoTextString.Append(FString("\n"));
		for (auto TiedPlayer : Players)
		{
			InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
		}
	}

	return InfoTextString;
}

void ACoopGnomePlayerController::ClientElimAnnouncement_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	APlayerState* Self = GetPlayerState<APlayerState>();
	if (Attacker && Victim && Self)
	{
		if (Attacker == Self && Victim != Self)
		{
			return;
		}
		if (Victim == Self && Attacker != Self)
		{
			return;
		}
		if (Attacker == Victim && Attacker == Self)
		{
			return;
		}
		if (Attacker == Victim && Attacker != Self)
		{
			return;
		}
	}
}

void ACoopGnomePlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}
