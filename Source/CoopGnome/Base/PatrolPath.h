// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SmartObject.h"
#include "PatrolPath.generated.h"

UCLASS()
class COOPGNOME_API APatrolPath : public ASmartObject
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	APatrolPath();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PatrolPath")
	class USplineComponent* Path;
	
	void GetPathPoints();

	TArray<FVector> PathPoints;

};
