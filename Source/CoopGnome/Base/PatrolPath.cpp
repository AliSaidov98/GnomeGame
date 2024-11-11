// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolPath.h"

#include "Components/SplineComponent.h"
#include "Components/BillboardComponent.h"


// Sets default values
APatrolPath::APatrolPath()
{
	PrimaryActorTick.bCanEverTick = false;
	Path = CreateDefaultSubobject<USplineComponent>(TEXT("Path"));
	
	Path->SetupAttachment(Billboard);
	Path->SetDrawDebug(true);
}

// Called when the game starts or when spawned
void APatrolPath::BeginPlay()
{
	Super::BeginPlay();
	GetPathPoints();
}

void APatrolPath::GetPathPoints()
{
	for (int i = 0; i <= Path->GetNumberOfSplinePoints(); i++)
	{
		PathPoints.Add(Path->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
	}
}


