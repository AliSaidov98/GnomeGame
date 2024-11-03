// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/ItemProperties.h"
#include "InventoryComponent.h"
#include "SplitterWidget.generated.h"

class USlider;
class UTextBlock;
class UButton;
class UImage;
class UInventoryComponent;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Inventory Widgets")
class INVENTORY_API USplitterWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UFUNCTION(Category = "Splitter Widget Functions")
	void InitializeSplitter(UInventoryComponent* DraggedFromInventory, UInventoryComponent* SplittingInventory,
		int32 DraggedSlotIndex, int32 DroppedSlotIndex);

	UFUNCTION(Category = "Splitter Widget Functions")
	void CloseSplitterWidget();
	
protected:
	UFUNCTION(Category = "Splitter Widget Functions")
	void SetMaximumSplitAmount();

	UFUNCTION(Category = "Splitter Widget Functions")
	void SplitSliderValueChanged(float Value);

	UFUNCTION(Category = "Splitter Widget Functions")
	void SetTransferAmountText(const float InSliderValue) const;

	UFUNCTION(Category = "Splitter Widget Functions")
	void ConfirmSplitting();
	
private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SplitItemThumbnail;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> SplitStackSlider;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LeftOverText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SplitDestinationText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> SplitFromInventory;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> SplitToInventory;
	
	UPROPERTY()
	int32 FromWhereToSplit = -1;

	UPROPERTY()
	int32 ToWhereToSplit = -1;

	UPROPERTY()
	int32 MaximumSplitAmount = 1;

	UPROPERTY()
	FItemStruct SplitItem;

	UPROPERTY()
	FItemStruct ToWhereSlotItem;

};
