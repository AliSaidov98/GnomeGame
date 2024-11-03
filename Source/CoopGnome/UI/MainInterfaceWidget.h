// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainInterfaceWidget.generated.h"

/**
 * 
 */
UCLASS()
class COOPGNOME_API UMainInterfaceWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> HealthWidget;

};
