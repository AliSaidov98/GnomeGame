// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/SplitterWidget.h"
#include "InventoryComponent.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "Widget/InventoryWidget.h"

void USplitterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ConfirmButton->OnClicked.AddDynamic(this, &USplitterWidget::ConfirmSplitting);
	CancelButton->OnClicked.AddDynamic(this, &USplitterWidget::CloseSplitterWidget);

	SplitStackSlider->OnValueChanged.AddDynamic(this, &USplitterWidget::SplitSliderValueChanged);
}

void USplitterWidget::InitializeSplitter(UInventoryComponent* DraggedFromInventory,
	UInventoryComponent* SplittingInventory, int32 DraggedSlotIndex, int32 DroppedSlotIndex)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	SplitFromInventory = DraggedFromInventory;
	SplitToInventory = SplittingInventory;

	FromWhereToSplit = DraggedSlotIndex;
	ToWhereToSplit = DroppedSlotIndex;
	
	SplitItem = DraggedFromInventory->CurrentItems[FromWhereToSplit];
	
	LeftOverText->SetText(UKismetTextLibrary::Conv_IntToText(SplitItem.CurrentStack - 1));

	ToWhereSlotItem = SplitToInventory->CurrentItems[ToWhereToSplit];

	SplitStackSlider->SetValue(0.0f);
	SetMaximumSplitAmount();
	SetTransferAmountText(0.0f);

	SplitItemThumbnail->SetBrushFromTexture(SplitItem.Thumbnail);
}

void USplitterWidget::CloseSplitterWidget()
{
	if(IsVisible())
	{
		SetVisibility(ESlateVisibility::Hidden);

		SplitToInventory->Multicast_UpdateInventoryUI();

		if(SplitFromInventory != nullptr && SplitFromInventory != SplitToInventory)
		{
			SplitFromInventory->Multicast_UpdateInventoryUI();
		}
		
		RemoveFromParent();
	}
}

void USplitterWidget::SetMaximumSplitAmount()
{
	if(ToWhereSlotItem.IsValid() && ToWhereSlotItem.MaximumStack - ToWhereSlotItem.CurrentStack < SplitItem.CurrentStack - 1)
	{
		MaximumSplitAmount = ToWhereSlotItem.MaximumStack - ToWhereSlotItem.CurrentStack;
	}
	else MaximumSplitAmount = SplitItem.CurrentStack - 1;
}

void USplitterWidget::SplitSliderValueChanged(float Value)
{
	const float SnappedValue = UKismetMathLibrary::GridSnap_Float(
		Value, 1.0f / (MaximumSplitAmount - 1));
	
	SplitStackSlider->SetValue(SnappedValue);
	SetTransferAmountText(SnappedValue);
}

void USplitterWidget::SetTransferAmountText(const float InSliderValue) const
{
	const float AmountToSplit = UKismetMathLibrary::MapRangeClamped(
		InSliderValue, 0.0f, 1.0f,1.0f, MaximumSplitAmount);
	
	LeftOverText->SetText(
		UKismetTextLibrary::Conv_IntToText(
			SplitItem.CurrentStack - UKismetMathLibrary::FTrunc(AmountToSplit)));

	if(ToWhereSlotItem.IsValid())
	{
		SplitDestinationText->SetText(
			UKismetTextLibrary::Conv_IntToText(
				ToWhereSlotItem.CurrentStack + UKismetMathLibrary::FTrunc(AmountToSplit)));		
	}
	else SplitDestinationText->SetText(UKismetTextLibrary::Conv_IntToText(AmountToSplit));
}

void USplitterWidget::ConfirmSplitting()
{
	const int32 SplitAmount = UKismetMathLibrary::MapRangeClamped(
		SplitStackSlider->GetValue(), 0.0f, 1.0f, 1.0f, MaximumSplitAmount);
	
	SplitToInventory->PlayerOwnedInventory->Server_SplitItem(
		SplitFromInventory,
		SplitToInventory,
		FromWhereToSplit,
		ToWhereToSplit,
		SplitAmount
	);

	CloseSplitterWidget();
}

