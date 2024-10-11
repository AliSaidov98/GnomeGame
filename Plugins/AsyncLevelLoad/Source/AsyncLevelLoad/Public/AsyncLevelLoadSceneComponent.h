// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AsyncLevelLoadSceneComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ASYNCLEVELLOAD_API UAsyncLevelLoadSceneComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAsyncLevelLoadSceneComponent();

	UFUNCTION(BlueprintCallable, Category="AsyncLevelLoad")
	void AsyncLevelLoad(const FString& LevelDir, const FString& LevelName);

	UFUNCTION(BlueprintCallable, Category="AsyncLevelLoad")
	void AsyncLevelLoadByObjRef(TSoftObjectPtr<UWorld> WorldSoftObject);

};
