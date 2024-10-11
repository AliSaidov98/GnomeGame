// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncLevelLoadSceneComponent.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"



// Sets default values for this component's properties
UAsyncLevelLoadSceneComponent::UAsyncLevelLoadSceneComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UAsyncLevelLoadSceneComponent::AsyncLevelLoad(const FString& LevelDir, const FString& LevelName)
{
	LoadPackageAsync(LevelDir+LevelName,
		FLoadPackageAsyncDelegate::CreateLambda([this, LevelDir, LevelName](const FName& PackageName, UPackage* 
		LoadPackage, EAsyncLoadingResult::Type Result)
		{
			if (Result == EAsyncLoadingResult::Succeeded)
			{
				UGameplayStatics::OpenLevel(this, FName(*(LevelDir+LevelName)));
			}
		}
		),
		0, PKG_ContainsMap);
}

void UAsyncLevelLoadSceneComponent::AsyncLevelLoadByObjRef(TSoftObjectPtr<UWorld> WorldSoftObject)
{
	FString LevelPath = WorldSoftObject.ToString();
	LoadPackageAsync(LevelPath,
		FLoadPackageAsyncDelegate::CreateLambda([this, WorldSoftObject](const FName& PackageName, UPackage* 
		LoadPackage, EAsyncLoadingResult::Type Result)
		{
			if (Result == EAsyncLoadingResult::Succeeded)
			{
				UGameplayStatics::OpenLevelBySoftObjectPtr(this, WorldSoftObject);
			}
		}
		),
		0, PKG_ContainsMap);
}


