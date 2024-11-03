// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InventoryWidget.h"
#include "Engine/GameViewportClient.h"
#include "InventoryComponent.h"
#include "ItemUseDefinitions.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/UniformGridPanel.h"
#include "Data/InventoryInfo.h"
#include "Widget/InventoryDragOperation.h"
#include "Widget/InventorySlotWidget.h"
#include "Widget/ItemTooltipWidget.h"
#include "Widget/SplitterWidget.h"


void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryWidget::InitializeInventoryWidget(UInventoryComponent* InInventoryComp)
{
	if(InventoryComp != nullptr && InventoryComp == InInventoryComp) return;
	
	if(InInventoryComp != nullptr)
	{
		InventoryComp = InInventoryComp;

		if(!InventoryComp->OnInventoryUpdated.IsBound())
		{
			InventoryComp->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::RefreshAllSlots);
			InventoryComp->OnSplittingInitialized.AddDynamic(this, &UInventoryWidget::CreateSplitterWidget);
			InventoryComp->OnOpenedOrClosedInventory.AddDynamic(this, &UInventoryWidget::CloseAllBags);
			InventoryComp->OnBagClosed.AddDynamic(this, &UInventoryWidget::CloseSpecificBag);
		}
		
		ConstructInitialSlots();

		const AController* OwnerController = InventoryComp->GetOwner()->GetInstigatorController();

		if(OwnerController && OwnerController->IsA<APlayerController>()) ConstructUseDefinitions();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Null inventory passed into %hs"), __FUNCTION__);
		RemoveFromParent();
	}
}

void UInventoryWidget::CreateBagWidget(UInventoryComponent* InBagInventory)
{
	UInventoryWidget* BagWidget = CreateWidget<UInventoryWidget>(
		this, InventoryComp->InventoryData->BagWidgetClass);

	FVector2D MousePosition;
	GetWorld()->GetGameViewport()->GetMousePosition(MousePosition);

	BagWidget->SetAlignmentInViewport(FVector2D(0.0f, 0.0f));
	BagWidget->SetPositionInViewport(MousePosition);

	BagWidget->AddToViewport(99);

	BagWidget->ParentInventoryWidget = this;
	
	InBagInventory->PlayerOwnedInventory = InventoryComp->PlayerOwnedInventory;
	
	BagWidget->InitializeInventoryWidget(InBagInventory);

	BagWidget->AdjustBagLocation();
	
	OpenedBagWidgets.Add(BagWidget);
}

void UInventoryWidget::CloseSpecificBag(UInventoryComponent* BagToClose)
{
	for(UInventoryWidget* BagWidget : OpenedBagWidgets)
	{
		if(BagWidget->InventoryComp == BagToClose)
		{
			OpenedBagWidgets.Remove(BagWidget);
			
			BagWidget->InventoryComp->IsInventoryOpen = false;
			BagWidget->RemoveFromParent();
			BagWidget = nullptr;

			break;
		}
	}
}

void UInventoryWidget::RefreshAllSlots()
{
	for(int i = 0; i < InventorySlots.Num(); i++)
	{
		if(InventoryComp->CurrentItems.IsValidIndex(i))
		{
			InventorySlots[i]->SlotIndex = i;
			
			InventorySlots[i]->SlotItem = InventoryComp->CurrentItems[i];
        
			InventorySlots[i]->RefreshSlot();
		}
	}
}

void UInventoryWidget::AddExternalSlot(UInventorySlotWidget* SlotToAdd, UItemTooltipWidget* ItemTooltipWidget)
{
	SlotToAdd->OwningInventoryWidget = this;
	SlotToAdd->SlotTooltipWidget = ItemTooltipWidget;
	
	SlotToAdd->SetToolTip(ItemTooltipWidget);
	SlotToAdd->GetToolTip()->SetVisibility(ESlateVisibility::Collapsed);
	
	InventorySlots.Add(SlotToAdd);

	InventoryComp->Server_NewSlotAdded(InventoryComp, SlotToAdd->AcceptableItemType);
}

void UInventoryWidget::ExpandExistingInventory(int32 ExtraSlotCount)
{
	if(ExtraSlotCount <= 0) return;
	
	const TSubclassOf<UInventorySlotWidget> SlotWidgetClass = InventoryComp->InventoryData->SlotWidgetClass;

	UItemTooltipWidget* TooltipWidget = NewObject<UItemTooltipWidget>(this, InventoryComp->InventoryData->TooltipWidgetClass);

	while(ExtraSlotCount > 0)
	{
		if(UInventorySlotWidget* NewSlot = NewObject<UInventorySlotWidget>(this, SlotWidgetClass))
		{
			NewSlot->SlotTooltipWidget = TooltipWidget;
			NewSlot->SetToolTip(TooltipWidget);
			NewSlot->GetToolTip()->SetVisibility(ESlateVisibility::Collapsed);
			
			NewSlot->OwningInventoryWidget = this;

			int32 InventorySlotAmount = InventoryComp->GetBasicSlotAmount(); 
			NewSlot->SlotIndex = InventorySlotAmount;
			
			InventorySlots.Insert(NewSlot, InventorySlotAmount);
			
			const int32 RowToAddTo = InventorySlotAmount / InventoryComp->GetMaximumSlotsPerRow();
			int32 ColumnToAddTo = InventorySlotAmount;

			if(InventorySlotAmount > InventoryComp->GetMaximumSlotsPerRow())
			{
				ColumnToAddTo = InventorySlotAmount % InventoryComp->GetMaximumSlotsPerRow();
			}

			InventoryGrid->AddChildToUniformGrid(NewSlot, RowToAddTo, ColumnToAddTo);

			InventoryComp->PlayerOwnedInventory->Server_NewSlotAdded(
				InventoryComp, EItemCategory::Everything, InventorySlotAmount);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create a new slot in %hs"), __FUNCTION__);
			return;
		}
		
		ExtraSlotCount--;
	}
}

void UInventoryWidget::ShrinkExistingInventory(int32 ShrinkingAmount)
{
	if(ShrinkingAmount <= 0) return;
	
	while(ShrinkingAmount > 0)
	{
		InventorySlots.RemoveAt(InventorySlots.Num() - 1);

		InventoryGrid->RemoveChildAt(InventoryComp->GetBasicSlotAmount() - 1);
		
		InventoryComp->PlayerOwnedInventory->Server_RemoveLastBasicSlot(InventoryComp);
		
		ShrinkingAmount--;
	}
}

void UInventoryWidget::CreateSplitterWidget(
	UInventoryComponent* DraggedFromInventory, const int32 DraggedSlotIndex, const int32 DroppedSlotIndex)
{
	USplitterWidget* SplitterWidget = CreateWidget<USplitterWidget>(
		this, InventoryComp->InventoryData->SplitterWidgetClass);

	SplitterWidget->OnVisibilityChanged.AddDynamic(this, &UInventoryWidget::SplitterVisibilityChanged);
	
	RootOverlay->AddChildToOverlay(SplitterWidget);

	if(UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(SplitterWidget->Slot))
	{
		OverlaySlot->SetHorizontalAlignment(HAlign_Center);
		OverlaySlot->SetVerticalAlignment(VAlign_Center);
	}

	InventoryComp->OnOpenedOrClosedInventory.AddDynamic(SplitterWidget, &USplitterWidget::CloseSplitterWidget);
	
	SplitterWidget->InitializeSplitter(
		DraggedFromInventory, InventoryComp, DraggedSlotIndex, DroppedSlotIndex);
}

void UInventoryWidget::ConstructInitialSlots()
{
	int32 CurrentRow = 0, CurrentColumn = 0;
	
	InventoryGrid->ClearChildren();
	InventorySlots.Empty();

	//UItemTooltipWidget* TooltipWidget = NewObject<UItemTooltipWidget>(this, InventoryComp->InventoryData->TooltipWidgetClass);
	
	for(int i = 0; i < InventoryComp->GetBasicSlotAmount(); i++)
	{
		if(CurrentColumn == InventoryComp->GetMaximumSlotsPerRow())
		{
			CurrentColumn = 0;
			CurrentRow++;
		}

		if(UInventorySlotWidget* CurrentSlot =
			NewObject<UInventorySlotWidget>(this, InventoryComp->InventoryData->SlotWidgetClass))
		{
			CurrentSlot->OwningInventoryWidget = this;
			CurrentSlot->SlotItem = InventoryComp->CurrentItems[i];
			CurrentSlot->SlotIndex = i;
			//CurrentSlot->SlotTooltipWidget = TooltipWidget;
			//CurrentSlot->SetToolTip(TooltipWidget);
			//CurrentSlot->GetToolTip()->SetVisibility(ESlateVisibility::Collapsed);

			InventorySlots.Add(CurrentSlot);
			InventoryGrid->AddChildToUniformGrid(CurrentSlot, CurrentRow, CurrentColumn);
			
			CurrentColumn++;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to create Inventory Slot in %hs"), __FUNCTION__);
			return;
		}
	}
}

void UInventoryWidget::ConstructUseDefinitions()
{
	InventoryComp->InventoryUseDefinitions =
		NewObject<UItemUseDefinitions>(this, InventoryComp->InventoryData->ItemUseDefinitionsClass);

	InventoryComp->InventoryUseDefinitions->OwnerActor = InventoryComp->GetOwner();
}

void UInventoryWidget::SplitterVisibilityChanged(ESlateVisibility InVisibility)
{
	if(InVisibility == ESlateVisibility::Collapsed || InVisibility == ESlateVisibility::Hidden)
	{
		InventoryGrid->SetIsEnabled(true);
	}
	else InventoryGrid->SetIsEnabled(false);
}

void UInventoryWidget::CloseAllBags()
{
	for(UInventoryWidget* BagWidget : OpenedBagWidgets)
	{
		BagWidget->InventoryComp->IsInventoryOpen = false;
		BagWidget->RemoveFromParent();
		BagWidget = nullptr;	
	}

	OpenedBagWidgets.Empty();
}

bool UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if(const UInventoryDragOperation* DragOperation = Cast<UInventoryDragOperation>(InOperation))
	{
		DragOperation->WhereItemCameFrom->Multicast_UpdateInventoryUI();
	}

	return true;
}

