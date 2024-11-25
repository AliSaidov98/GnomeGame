// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSoundSubsystem.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"



void UGameSoundSubsystem::PlayMusic(TSoftObjectPtr<USoundBase> MusicSound)
{
	if (MusicSound.IsNull())
	{
		//LOG
		return;
	}

	FSoftObjectPath MusicPath = MusicSound.ToSoftObjectPath();
	UAssetManager::GetStreamableManager().RequestAsyncLoad(MusicPath, FStreamableDelegate::CreateUObject(this, &UGameSoundSubsystem::OnMusicLoadSuccess, MusicSound));
}

void UGameSoundSubsystem::PlaySound(TSoftObjectPtr<USoundBase> Sound, FVector Location)
{
	if (Sound.IsNull())
	{
		//LOG
		return;
	}

	if (Sound.IsValid())
	{
		OnSoundLoadSuccess(Sound, Location);
		return;
	}

	FSoftObjectPath SoundPath = Sound.ToSoftObjectPath();
	UAssetManager::GetStreamableManager().RequestAsyncLoad(SoundPath, FStreamableDelegate::CreateUObject(this, &UGameSoundSubsystem::OnSoundLoadSuccess, Sound, Location));

}

void UGameSoundSubsystem::OnMusicLoadSuccess(TSoftObjectPtr<USoundBase> MusicSound)
{
	if (!MusicSound.IsValid())
	{
		//LOG
		return;
	}

	MusicSound->Duration = INDEFINITELY_LOOPING_DURATION;
	
	if (!IsValid(MusicAudioComponent))
	{
		MusicAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), MusicSound.Get());
	}
	else
	{
		MusicAudioComponent->SetSound(MusicSound.Get());

	}
	MusicAudioComponent->Play();
}

void UGameSoundSubsystem::OnSoundLoadSuccess(TSoftObjectPtr<USoundBase> Sound, FVector Location)
{
	Sound->AttenuationSettings = AttenuationSettings;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound.Get(), Location);
	/*if (!IsValid(SoundAudioComponent))
	{
		SoundAudioComponent = UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound.Get(), Location);
	}
	else
	{
		SoundAudioComponent->SetSound(Sound.Get());
	}
	SoundAudioComponent->Play();*/
}

