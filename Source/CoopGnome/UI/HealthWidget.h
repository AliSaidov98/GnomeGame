// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class COOPGNOME_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthValueText;

private:
	UFUNCTION()
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	
};
