// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CoopGnomeGameMode.generated.h"

namespace MatchState
{
	// Match duration has been reached. Display winner and begin cooldown timer.
	extern COOPGNOME_API const FName Cooldown;
}

UCLASS(minimalapi)
class ACoopGnomeGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACoopGnomeGameMode();

	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class ACoopGnomeCharacter* ElimmedCharacter, class ACoopGnomePlayerController* VictimController, ACoopGnomePlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	void PlayerLeftGame(class ACoopGnomePlayerState* PlayerLeaving);
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;
	
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;

};



