// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/ItemTooltipWidget.h"

TMap<FString, float> UItemTooltipWidget::GetStatMap(const FItemStruct InItem)
{
	TMap<FString, float> ResultingMap = {

		{" Maximum Health", InItem.Health},
		{" Armor", InItem.Armor},
		{"% Critical Chance", InItem.CriticalChance}
	};

	return ResultingMap;
}

