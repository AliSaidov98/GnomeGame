// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomConfigObject.h"
#include "Misc/ConfigCacheIni.h"
#include "UObject/UObjectIterator.h"



void UCustomConfigObject::LoadSettings(bool bBroadcastSettingsChanged)
{
	FString CustomConfigIni;
	FConfigCacheIni::LoadGlobalIniFile(CustomConfigIni, *GetConfigFilename(this), nullptr, true);
	LoadConfig(NULL, NULL, UE::LCPF_PropagateToChildDefaultObjects, NULL);
	if (bBroadcastSettingsChanged)
	{
		BroadcastSettingsChanged();
	}
}

void UCustomConfigObject::SaveSettings(bool bBroadcastSettingsChanged)
{
	SaveConfig();
	if (bBroadcastSettingsChanged)
	{
		BroadcastSettingsChanged();
	}
}

void UCustomConfigObject::BroadcastSettingsChanged()
{
	UClass* CurrentClass = GetClass();
	for (TObjectIterator<UCustomConfigObject> it; it; ++it)
	{
		if (UCustomConfigObject* CustomConfigObj = *it)
		{
			if (IsValid(CustomConfigObj) && CustomConfigObj->GetClass() == CurrentClass)
			{
				CustomConfigObj->LoadConfig();
				CustomConfigObj->OnSettingsChanged.Broadcast();
			}
		}
	}
}
