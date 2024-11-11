// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoopGnome/Actions/ActionActor.h"
#include "CoopGnome/Base/SmartObject.h"
#include "AIActionPath.generated.h"

UCLASS()
class COOPGNOME_API AAIActionPath : public ASmartObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAIActionPath();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings")
	TArray<AActionActor*> ActionActors;
};
