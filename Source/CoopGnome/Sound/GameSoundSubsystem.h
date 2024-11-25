// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameSoundSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class COOPGNOME_API UGameSoundSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
		
public:
	UFUNCTION(BlueprintCallable)
	void PlayMusic(TSoftObjectPtr<USoundBase> MusicSound);

	UFUNCTION(BlueprintCallable)
	void PlaySound(TSoftObjectPtr<USoundBase> Sound, FVector Location);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	USoundAttenuation* AttenuationSettings;
private:
	UFUNCTION()
	void OnMusicLoadSuccess(TSoftObjectPtr<USoundBase> MusicSound);
	UFUNCTION()
	void OnSoundLoadSuccess(TSoftObjectPtr<USoundBase> Sound, FVector Location);

	TObjectPtr<UAudioComponent> MusicAudioComponent;
	TObjectPtr<UAudioComponent> SoundAudioComponent;

	
};
