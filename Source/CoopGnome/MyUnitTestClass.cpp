// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUnitTestClass.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#include "Tests/AutomationCommon.h"
#include "Engine.h"
#include "EngineUtils.h"

#include "TestActor.h"

UWorld* GetTestWorld() {
    const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
    for (const FWorldContext& Context : WorldContexts)
    {
        if (((Context.WorldType == EWorldType::PIE) || (Context.WorldType == EWorldType::Game))
            && (Context.World() != nullptr))
        {
            return Context.World();
        }
    }

    return nullptr;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGetLocation, "GnomeGame.GetLocation.GetLocationTest",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

bool FGetLocation::RunTest(const FString& Parameters)
{
    AutomationOpenMap(TEXT("/Game/_Project/Maps/TestMap"));

    UWorld* world = GetTestWorld();

    FVector ExpectedLocation = FVector(120, 30, 40);

    ATestActor* TestActor = world->SpawnActor<ATestActor>();
    AActor* SpawnedActor = world->SpawnActor<AActor>(FVector(120, 30, 40), FRotator(120, 30, 40));

    USceneComponent* RootComponent = NewObject<USceneComponent>(SpawnedActor, TEXT("RootComponent"));
    SpawnedActor->SetRootComponent(RootComponent);

    SpawnedActor->SetActorLocation(FVector(120, 30, 40));

    TestActor->ActorToSpawn = SpawnedActor;

    UE_LOG(LogTemp, Warning, TEXT("TestActor->ActorToSpawn  is %s"), *TestActor->ActorToSpawn->GetName());

    UE_LOG(LogTemp, Warning, TEXT("TestActor->ActorToSpawn  is %s"), *SpawnedActor->GetActorLocation().ToString());

	TestTrueExpr(TestActor->GetSelectedActorLocation() == ExpectedLocation);

	return true;
}

