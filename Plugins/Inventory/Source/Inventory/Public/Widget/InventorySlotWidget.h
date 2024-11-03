// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemProperties.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UInventoryWidget;
class UItemTooltipWidget;
class UImage;
class UTextBlock;	
class UInventoryComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Inventory Widgets")
class INVENTORY_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UInventoryWidget;
	
public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	void RefreshSlot() const;
	
protected:
	bool CanSlotAcceptItem(const FItemStruct& InItem) const;

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;	

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemThumbnailImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemStackText;	

	UPROPERTY(EditInstanceOnly, Category = "Inventory Slot")
	TObjectPtr<UTexture2D> CustomEquipmentImage;

	UPROPERTY()
	TObjectPtr<UInventoryWidget> OwningInventoryWidget;
	
	UPROPERTY()
	TObjectPtr<UItemTooltipWidget> SlotTooltipWidget;

	UPROPERTY()
	int32 SlotIndex = -1;

	UPROPERTY(EditInstanceOnly, Category = "Inventory Slot")
	EItemCategory AcceptableItemType = EItemCategory::Everything;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Slot", meta = (AllowPrivateAccess = true))
	FItemStruct SlotItem;

};
