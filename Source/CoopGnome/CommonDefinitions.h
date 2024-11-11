#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EFaction : uint8
{
	Enemy            UMETA(DisplayName = "Enemy"),
	Friendly         UMETA(DisplayName = "Friendly"),
	Neutral          UMETA(DisplayName = "Neutral")
};

UENUM(BlueprintType)
enum class ECombatRole : uint8
{
	Engager           UMETA(DisplayName = "Engager"),
	Defender          UMETA(DisplayName = "Defender"),
	Sniper            UMETA(DisplayName = "Sniper")
};

const FName cActionActorBBKey = "ActionActor";