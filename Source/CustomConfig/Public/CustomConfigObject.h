// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CustomConfigObject.generated.h"

/**
 * 
*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCustomConfigObjectChangedSignature);

UCLASS(Config=CustomConfig, Blueprintable)
class CUSTOMCONFIG_API UCustomConfigObject : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="CustomConfig")
	void LoadSettings(bool bBroadcastSettingsChanged = true);

	UFUNCTION(BlueprintCallable, Category="CustomConfig")
	void SaveSettings(bool bBroadcastSettingsChanged = true);

	UFUNCTION(BlueprintCallable, Category="CustomConfig")
	void BroadcastSettingsChanged();

	UPROPERTY(BlueprintAssignable, Category="CustomConfig")
	FCustomConfigObjectChangedSignature OnSettingsChanged;

};
