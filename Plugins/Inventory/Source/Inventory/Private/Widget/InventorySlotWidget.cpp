// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/InventorySlotWidget.h"
#include "InventoryComponent.h"
#include "ItemUseDefinitions.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/InventoryInfo.h"
#include "Widget/InventoryDragOperation.h"
#include "Widget/InventoryWidget.h"
#include "Widget/ItemTooltipWidget.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshSlot();
}

void UInventorySlotWidget::RefreshSlot() const
{
	if(SlotItem.IsValid() && !SlotItem.IsItemInUse)
	{
		ItemThumbnailImage->SetVisibility(ESlateVisibility::Visible);
		ItemThumbnailImage->SetBrushFromTexture(SlotItem.Thumbnail);

		const FString CurrentStackString = SlotItem.CurrentStack > 1 ?
			 FString::FromInt(SlotItem.CurrentStack) : "";
		
		ItemStackText->SetText(FText::FromString(CurrentStackString));
		ItemStackText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		ItemStackText->SetVisibility(ESlateVisibility::Hidden);

		if(AcceptableItemType != EItemCategory::Everything && CustomEquipmentImage != nullptr && !SlotItem.IsItemInUse)
		{
			ItemThumbnailImage->SetBrushFromTexture(CustomEquipmentImage);
		}
		else ItemThumbnailImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UInventorySlotWidget::CanSlotAcceptItem(const FItemStruct& InItem) const
{
	if(AcceptableItemType == EItemCategory::Everything || AcceptableItemType == InItem.Category)
	{
		return true;
	}

	return false;
}

FReply UInventorySlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(SlotItem.IsValid() == false) return FReply::Unhandled();

	if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && SlotItem.IsValid())
	{
		FEventReply Reply;
		Reply.NativeReply = FReply::Unhandled();

		if(const TSharedPtr<SWidget> SlateWidgetDetectingDrag = GetCachedWidget(); SlateWidgetDetectingDrag.IsValid())
		{
			Reply.NativeReply = Reply.NativeReply.DetectDrag(
				SlateWidgetDetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
		}
		
		return Reply.NativeReply;
	}

	if(InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if(SlotItem.IsBag && SlotItem.ItemBagInventory != nullptr)
		{
			if(!SlotItem.ItemBagInventory->IsInventoryOpen)
			{
				OwningInventoryWidget->CreateBagWidget(SlotItem.ItemBagInventory);

				SlotItem.ItemBagInventory->IsInventoryOpen = true;
			}
		}
		else
		{
			OwningInventoryWidget->InventoryComp->PlayerOwnedInventory->Server_DropItemFromInventory(
				OwningInventoryWidget->InventoryComp, SlotIndex
			);

			SlotItem.Clear();
			RefreshSlot();
		}
	}
	
	return FReply::Handled();
}

FReply UInventorySlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if(!SlotItem.IsValid()) return FReply::Unhandled();
	
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);

	if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		FReply CurrentReply = FReply::Handled();

		UInventoryComponent* WidgetsInventoryComponent = OwningInventoryWidget->InventoryComp;
		
		if(WidgetsInventoryComponent->PlayerOwnedInventory->GetHalveStackKeyPressed())
		{
			WidgetsInventoryComponent->PlayerOwnedInventory->Server_HalveItemStack(WidgetsInventoryComponent, SlotIndex);
		}
		else if(WidgetsInventoryComponent->PlayerOwnedInventory->ConnectedInventory)
		{
			WidgetsInventoryComponent->PlayerOwnedInventory->Server_TransferToConnectedInventory(
				WidgetsInventoryComponent, true, SlotIndex
			);
		}
		else if(WidgetsInventoryComponent->InventoryUseDefinitions != nullptr 
			&& SlotItem.CurrentStack > SlotItem.UseDecrementStackAmount)
		{
			WidgetsInventoryComponent->PlayerOwnedInventory->InventoryUseDefinitions->MasterFunction(
				WidgetsInventoryComponent, SlotItem, SlotIndex
			);

			WidgetsInventoryComponent->PlayerOwnedInventory->Server_DecrementUsedItemStack(
				WidgetsInventoryComponent, SlotIndex
			);
		}
		else CurrentReply = FReply::Unhandled();

		return CurrentReply;
	}
	
	return FReply::Handled();
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if(SlotItem.IsItemInUse) return;

	OwningInventoryWidget->InventoryComp->PlayerOwnedInventory->Server_SetItemInUse(
		OwningInventoryWidget->InventoryComp, SlotIndex);
	
	if(SlotItem.IsBag && SlotItem.ItemBagInventory && SlotItem.ItemBagInventory->IsInventoryOpen)
	{
		OwningInventoryWidget->InventoryComp->OnBagClosed.Broadcast(SlotItem.ItemBagInventory);
	}
	
	if(UInventoryDragOperation* DragOperation = NewObject<UInventoryDragOperation>(this))
	{
		DragOperation->DefaultDragVisual = NewObject<UUserWidget>(
			this, OwningInventoryWidget->InventoryComp->InventoryData->DragVisualClass);
		
		DragOperation->WhereItemCameFrom = OwningInventoryWidget->InventoryComp;
		DragOperation->DraggedItem = SlotItem;
		DragOperation->DraggedSlotIndex = SlotIndex;
		
		SlotItem.Clear();
		RefreshSlot();
		
		OutOperation = DragOperation;		
	}
	else UE_LOG(LogTemp, Error, TEXT("Unable to create Drag Operation in %hs"), __FUNCTION__);
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	const UInventoryDragOperation* CurrentDragOperation = Cast<UInventoryDragOperation>(InOperation);
	
	if(!SlotItem.IsItemInUse && CanSlotAcceptItem(CurrentDragOperation->DraggedItem))
	{
		UInventoryComponent* WidgetsInventoryComponent = OwningInventoryWidget->InventoryComp;
		
		WidgetsInventoryComponent->PlayerOwnedInventory->Server_SetItemInUse(
			WidgetsInventoryComponent, SlotIndex);
		
		WidgetsInventoryComponent->Server_OnItemDropped(
            CurrentDragOperation->WhereItemCameFrom,
            WidgetsInventoryComponent,
            AcceptableItemType,
            CurrentDragOperation->DraggedSlotIndex,
            SlotIndex,
            WidgetsInventoryComponent->PlayerOwnedInventory->GetSplitKeyPressed()
        );	
        
		return true;
	}

	OwningInventoryWidget->InventoryComp->PlayerOwnedInventory->Server_SetItemInUse(
		CurrentDragOperation->WhereItemCameFrom,
		CurrentDragOperation->DraggedSlotIndex,
		false
	);
	
	RefreshSlot();
	OwningInventoryWidget->InventorySlots[CurrentDragOperation->DraggedSlotIndex]->RefreshSlot();
	
	return false;
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if(SlotItem.IsValid() && !SlotItem.IsBag && SlotTooltipWidget != nullptr)
	{
		GetToolTip()->SetVisibility(ESlateVisibility::HitTestInvisible);

		SlotTooltipWidget->InitializeTooltip(SlotItem);
	}
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if(SlotTooltipWidget != nullptr && GetToolTip()->IsVisible())
	{
		GetToolTip()->SetVisibility(ESlateVisibility::Collapsed);
	}
}


