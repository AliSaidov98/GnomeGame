#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Unoccupied UMETA(DisplayName = "Unoccupied"),
	Reloading UMETA(DisplayName = "Reloading"),
	ThrowingGrenade UMETA(DisplayName = "Throwing Grenade"),
	SwappingWeapons UMETA(DisplayName = "Swapping Weapons"),
	SoftCover UMETA(DisplayName = "Soft Cover"),
	ApproachingCover UMETA(DisplayName = "Approaching Cover"),
	Flank UMETA(DisplayName = "Flank"),
	HoldCover UMETA(DisplayName = "HoldCover"),
	RetreatingCover UMETA(DisplayName = "RetreatingCover"),
	Evade UMETA(DisplayName = "Evade")
};

