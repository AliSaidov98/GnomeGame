// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/ItemProperties.h"
#include "ItemTooltipWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Inventory Widgets")
class INVENTORY_API UItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tooltip Widget")
	void InitializeTooltip(const FItemStruct DescribedItem);

	UFUNCTION(BlueprintCallable, Category = "Tooltip Widget")
	TMap<FString, float> GetStatMap(const FItemStruct InItem);

};
