// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UOverlay;
enum class EItemCategory : uint8;
class UContainerInventoryWidget;
class UInventoryComponent;
class UInventorySlotWidget;
class USplitterWidget;
class UItemTooltipWidget;
class UItemUseDefinitions;
class UUniformGridPanel;

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Inventory Widgets")

class INVENTORY_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UInventorySlotWidget;
	
public:

	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
	void InitializeInventoryWidget(UInventoryComponent* InInventoryComp);

	UFUNCTION(Category = "Inventory Widget")
	void CreateBagWidget(UInventoryComponent* InBagInventory);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Widget")
	void AdjustBagLocation();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
	void CloseSpecificBag(UInventoryComponent* BagToClose);
	
protected:
	UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
	void RefreshAllSlots();

	UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
	void AddExternalSlot(UInventorySlotWidget* SlotToAdd, UItemTooltipWidget* ItemTooltipWidget);

	UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
	void ExpandExistingInventory(int32 ExtraSlotCount);

	UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
	void ShrinkExistingInventory(int32 ShrinkingAmount);
	
	UFUNCTION(Category = "Inventory Widget")
	void CreateSplitterWidget(UInventoryComponent* DraggedFromInventory, int32 DraggedSlotIndex, int32 DroppedSlotIndex);

	UFUNCTION(Category = "Inventory Widget")
	void ConstructInitialSlots();

	UFUNCTION(Category = "Inventory Widget")
	void ConstructUseDefinitions();

	UFUNCTION(Category = "Inventory Widget")
	void SplitterVisibilityChanged(ESlateVisibility InVisibility);
	
	UFUNCTION(Category = "Inventory Widget")
	void CloseAllBags();

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
private:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> RootOverlay;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory Widget", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UUniformGridPanel> InventoryGrid;
	
	UPROPERTY(meta = (BindWidget))
	TArray<UInventorySlotWidget*> InventorySlots;

	UPROPERTY()
	TArray<TObjectPtr<UInventoryWidget>> OpenedBagWidgets;
	
public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Widget")
	TObjectPtr<UInventoryComponent> InventoryComp;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Widget")
	TObjectPtr<UInventoryWidget> ParentInventoryWidget;

};
