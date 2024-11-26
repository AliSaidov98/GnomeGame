// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyMode.h"
#include "GameFramework/GameState.h"
#include "MultiplayerSessionsSubsystem.h"

void ALobbyMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check (Subsystem);
		if (NumberOfPlayers == Subsystem->DesireNumberConnections)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				bUseSeamlessTravel = true;

				FString MatchType = Subsystem->DesireMatchType;
				if (MatchType == "FreeForAll")
				{
					World->SeamlessTravel(FString("/Game/Maps/FreeForAll?listen"));
				}
				else if (MatchType == "Teams")
				{
					World->SeamlessTravel(FString("/Game/Maps/Teams?listen"));
				}
				else if (MatchType == "CaptureFlag")
				{
					World->SeamlessTravel(FString("/Game/Maps/CaptureFlag?listen"));
				}
			}
		}
	}
}
