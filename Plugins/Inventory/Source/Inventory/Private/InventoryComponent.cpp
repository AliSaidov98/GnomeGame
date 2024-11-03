// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "HelperFunctionLibrary.h"
#include "Data/InventoryInfo.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, BasicSlotAmount);
	DOREPLIFETIME(UInventoryComponent, ConnectedInventory);
	DOREPLIFETIME(UInventoryComponent, CurrentItems);
	DOREPLIFETIME(UInventoryComponent, EnableAutoEquip);
	DOREPLIFETIME(UInventoryComponent, EquipmentSlotAcceptableItems);
	DOREPLIFETIME(UInventoryComponent, InventoryData);
	DOREPLIFETIME(UInventoryComponent, IsBagInventory);
	DOREPLIFETIME(UInventoryComponent, MaximumSlotsPerRow);
	DOREPLIFETIME(UInventoryComponent, PlayerOwnedInventory);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentItems.SetNum(BasicSlotAmount, false);

	EquipmentSlotAcceptableItems.Empty();

	AddStartupItems();
}

void UInventoryComponent::Server_ConnectInventory_Implementation(
	UInventoryComponent* NewConnectedInventory, const bool MutualConnection)
{
	ConnectedInventory = NewConnectedInventory;

	if(MutualConnection && NewConnectedInventory) NewConnectedInventory->ConnectedInventory = this;
}

void UInventoryComponent::Server_DecrementUsedItemStack_Implementation(
	UInventoryComponent* FromWhichInventory, const int32& ItemIndex)
{
	FromWhichInventory->CurrentItems[ItemIndex].DecrementStackWhenUsed();
}

void UInventoryComponent::Server_SetItemInUse_Implementation(
	UInventoryComponent* ForWhichInventory, const int32& ItemIndex, const bool InUse)
{
	ForWhichInventory->CurrentItems[ItemIndex].IsItemInUse = InUse;
}

void UInventoryComponent::Server_SetPlayerOwnedInventory_Implementation(UInventoryComponent* ForWhichInventory)
{
	ForWhichInventory->PlayerOwnedInventory = this;
}

void UInventoryComponent::Server_NewSlotAdded_Implementation(
	UInventoryComponent* ForWhichInventory, const EItemCategory Category, const int32& InsertPosition)
{
	if(Category != EItemCategory::Everything) ForWhichInventory->EquipmentSlotAcceptableItems.Add(Category);

	if(InsertPosition == -1)
	{
		ForWhichInventory->CurrentItems.Add(FItemStruct());
	}
	else
	{
		ForWhichInventory->CurrentItems.Insert(FItemStruct(), InsertPosition);
		ForWhichInventory->BasicSlotAmount++;
	}
}

void UInventoryComponent::Server_RemoveLastBasicSlot_Implementation(UInventoryComponent* ForWhichInventory)
{
	if(ForWhichInventory->BasicSlotAmount > 0)
	{
		ForWhichInventory->CurrentItems.RemoveAt(BasicSlotAmount - 1);
		
		ForWhichInventory->BasicSlotAmount--;		
	}
}

void UInventoryComponent::Server_AddItem_Implementation(FItemStruct InputItem)
{
	if(!InputItem.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Tried to add an invalid item in %hs"), __FUNCTION__);
		return;
	}

	if(InputItem.IsBag && InputItem.ItemBagInventory == nullptr) InitializeBagInventory(InputItem);
	
	if(EnableAutoEquip && InputItem.IsEquipment)
	{	
		if(AutoEquipItem(InputItem))
		{
			LastAddedItemStack = 0;

			if(IsOwnerOnServer()) Multicast_UpdateInventoryUI();
			
			return;
		}
	}

	for(int i = 0; i < BasicSlotAmount; i++)
	{
		if(InputItem.CurrentStack == 0)
		{
			LastAddedItemStack = 0;
			
			if(IsOwnerOnServer()) Multicast_UpdateInventoryUI();
			
			return;
		}
		
		FItemStruct& CurrentItem = CurrentItems[i];

		if(CurrentItem.IsValid() && InputItem == CurrentItem)
		{
			if(CurrentItem.CurrentStack < CurrentItem.MaximumStack)
			{
				if(CurrentItem.CurrentStack + InputItem.CurrentStack < CurrentItem.MaximumStack)
				{
					CurrentItem.CurrentStack += InputItem.CurrentStack;

					LastAddedItemStack = 0;
					
					if(IsOwnerOnServer()) Multicast_UpdateInventoryUI();
					
					return;
				}

				InputItem.CurrentStack -= (CurrentItem.MaximumStack - CurrentItem.CurrentStack);
				CurrentItem.CurrentStack = CurrentItem.MaximumStack;

				LastAddedItemStack = InputItem.CurrentStack;
			}
		}
	}

	for(int i = 0; i < BasicSlotAmount; i++)
	{
		if(!CurrentItems[i].IsValid())
		{
			if(InputItem.CurrentStack <= InputItem.MaximumStack)
			{
				CurrentItems[i] = InputItem;

				LastAddedItemStack = 0;
				
				if(GetOwner() && GetOwner()->HasAuthority()) Multicast_UpdateInventoryUI();
				
				return;
			}
			
			CurrentItems[i] = InputItem;
			CurrentItems[i].CurrentStack = InputItem.MaximumStack;

			InputItem.CurrentStack -= InputItem.MaximumStack;

			LastAddedItemStack = InputItem.CurrentStack;
		}
	}

	LastAddedItemStack = InputItem.CurrentStack;
	
	if(IsOwnerOnServer()) Multicast_UpdateInventoryUI();
}

void UInventoryComponent::Server_DropItemFromInventory_Implementation(
	UInventoryComponent* FromWhichInventory, const int32& DroppedItemIndex)
{
	if(!FromWhichInventory->CurrentItems.IsValidIndex(DroppedItemIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("Index out of bounds passed to %hs"), __FUNCTION__);
		return;
	}

	FromWhichInventory->CurrentItems[DroppedItemIndex].IsItemInUse = false;
	
	UHelperFunctionLibrary::SpawnItemActor(
		FromWhichInventory->CurrentItems[DroppedItemIndex],
		PlayerOwnedInventory->GetOwner(),
		InventoryData->ItemActorClass
	);

	FromWhichInventory->RemoveItem(DroppedItemIndex, CurrentItems[DroppedItemIndex].CurrentStack);
}

void UInventoryComponent::Server_HalveItemStack_Implementation(
	UInventoryComponent* WhereToHalve, const int32& ItemArrayIndex) 
{
	if(WhereToHalve->CurrentItems[ItemArrayIndex].CurrentStack <= 1) return;
	
	for(int i = 0; i < WhereToHalve->BasicSlotAmount; i++)
	{
		if(!WhereToHalve->CurrentItems[i].IsValid())
		{
			FItemStruct& SplitItem = WhereToHalve->CurrentItems[ItemArrayIndex];

			const int32 StackHalfSize = SplitItem.CurrentStack % 2 == 0 ?
				SplitItem.CurrentStack / 2 : SplitItem.CurrentStack / 2 + 1;

			WhereToHalve->CurrentItems[i] = WhereToHalve->CurrentItems[ItemArrayIndex];

			WhereToHalve->CurrentItems[i].CurrentStack = StackHalfSize;

			SplitItem.CurrentStack -= StackHalfSize;

			if(IsOwnerOnServer()) WhereToHalve->Multicast_UpdateInventoryUI();
			
			return;
		}
	}
}

void UInventoryComponent::Server_SplitItem_Implementation(UInventoryComponent* FromWhichInventory,
	UInventoryComponent* InventoryForSplit, const int32& FromWhereToSplit, const int32& SplitToIndex, int32 AmountToSplit)
{
	FItemStruct& BeingSplit = FromWhichInventory->CurrentItems[FromWhereToSplit];
	FItemStruct& SplitTo = InventoryForSplit->CurrentItems[SplitToIndex];
	
	if(SplitTo.IsValid())
	{
		SplitTo.CurrentStack += AmountToSplit;
	}
	else
	{
		SplitTo = BeingSplit;

		SplitTo.CurrentStack = AmountToSplit;
	}

	BeingSplit.CurrentStack -= AmountToSplit;

	BeingSplit.IsItemInUse = false;
	SplitTo.IsItemInUse = false;
	
	if(IsOwnerOnServer()) InventoryForSplit->Multicast_UpdateInventoryUI();
}

void UInventoryComponent::Server_TransferToConnectedInventory_Implementation(
	UInventoryComponent* FromWhichInventory, const bool TransferJustOne, const int32 SpecificTransferIndex) 
{
	const bool PreviousAutoEquip = FromWhichInventory->ConnectedInventory->EnableAutoEquip;

	FromWhichInventory->ConnectedInventory->EnableAutoEquip = false;

	if(SpecificTransferIndex == -1)
	{
		for(int i = FromWhichInventory->CurrentItems.Num() - 1; i >= 0; i--)
		{
			FItemStruct CurrentItem = FromWhichInventory->CurrentItems[i];
			int32 FullItemStack = CurrentItem.CurrentStack;

			if(CurrentItem.IsValid() && !CurrentItem.IsItemInUse)
			{
				FromWhichInventory->ConnectedInventory->Server_AddItem(CurrentItem);
				
				if(LastAddedItemStack == 0)
				{
					FromWhichInventory->RemoveItem(i, FullItemStack);
				}
				else
				{
					FromWhichInventory->RemoveItem(i, FullItemStack - LastAddedItemStack);
					break;
				}
				if(TransferJustOne) break;
			}
		}
	}
	else if(!FromWhichInventory->CurrentItems[SpecificTransferIndex].IsItemInUse)
	{
		FItemStruct SpecificItem = FromWhichInventory->CurrentItems[SpecificTransferIndex];
		int32 FullItemStack = SpecificItem.CurrentStack;

		FromWhichInventory->ConnectedInventory->Server_AddItem(SpecificItem);
		
		if(LastAddedItemStack == 0)
		{
			FromWhichInventory->RemoveItem(SpecificTransferIndex, FullItemStack);
		}
		else FromWhichInventory->RemoveItem(SpecificTransferIndex, FullItemStack - LastAddedItemStack);
	}

	FromWhichInventory->ConnectedInventory->EnableAutoEquip = PreviousAutoEquip;
	
	if(IsOwnerOnServer()) FromWhichInventory->Multicast_UpdateInventoryUI();
}

void UInventoryComponent::Server_OnItemDropped_Implementation(UInventoryComponent* DraggedFromInventory,
	UInventoryComponent* DroppedOnInventory, const EItemCategory& SlotAcceptableCategory,
	const int32& DraggedSlotIndex, const int32& DroppedSlotIndex, const bool ShouldSplitItem)
{
	if(!DraggedFromInventory->CurrentItems.IsValidIndex(DraggedSlotIndex) ||
	   !DroppedOnInventory->CurrentItems.IsValidIndex(DroppedSlotIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("Index out of bounds passed to %hs"), __FUNCTION__);
		return;
	}		

	if(DraggedSlotIndex == DroppedSlotIndex && DraggedFromInventory == DroppedOnInventory)
	{
		DraggedFromInventory->CurrentItems[DraggedSlotIndex].IsItemInUse = false;
		DroppedOnInventory->CurrentItems[DroppedSlotIndex].IsItemInUse = false;
		
		DroppedOnInventory->Multicast_UpdateInventoryUI();
		return;
	}

	const FItemStruct& DraggedItem = DraggedFromInventory->CurrentItems[DraggedSlotIndex];
	const FItemStruct& DroppedSlotItem = DroppedOnInventory->CurrentItems[DroppedSlotIndex];

	if(ShouldSplitItem && CanSplitItem(DraggedItem, DroppedSlotItem, SlotAcceptableCategory))
	{
		DroppedOnInventory->Multicast_CallSplitterDelegate(DraggedFromInventory, DraggedSlotIndex, DroppedSlotIndex);
		return;
	}
	
	if(DroppedSlotItem.IsValid())
	{
		if(DraggedItem == DroppedSlotItem)
		{
			DroppedOnInventory->MergeItems(DraggedFromInventory, DraggedSlotIndex, DroppedSlotIndex);
		}
		else DroppedOnInventory->SwapItems(DraggedFromInventory, DraggedSlotIndex, DroppedSlotIndex);
	}
	else DroppedOnInventory->MoveItemToEmptySlot(DraggedFromInventory, DraggedSlotIndex, DroppedSlotIndex);

	DraggedFromInventory->CurrentItems[DraggedSlotIndex].IsItemInUse = false;
	DroppedOnInventory->CurrentItems[DroppedSlotIndex].IsItemInUse = false;
	
	if(IsOwnerOnServer() || DroppedOnInventory->IsBagInventory) DroppedOnInventory->Multicast_UpdateInventoryUI();
}

bool UInventoryComponent::CanSplitItem(const FItemStruct& SplitItem,
	const FItemStruct& DestinationItem, const EItemCategory& SlotAcceptableCategory) const
{
	if(SlotAcceptableCategory != EItemCategory::Everything) return false;

	if(!DestinationItem.IsValid() || (SplitItem == DestinationItem &&
		DestinationItem.CurrentStack < DestinationItem.MaximumStack))
	{
		return true;
	}

	return false; 
}

void UInventoryComponent::Multicast_UpdateInventoryUI_Implementation() const
{
	OnInventoryUpdated.Broadcast();
}

int32 UInventoryComponent::GetValidSlotForAutoEquip(const EItemCategory& InItemCategory, const bool EmptySlotsOnly)
{
	for(int32 i = BasicSlotAmount; i < CurrentItems.Num(); i++)
	{
		if(EquipmentSlotAcceptableItems[i - BasicSlotAmount] == InItemCategory)
		{
			if(EmptySlotsOnly == false || !CurrentItems[i].IsValid()) return i;
		}
	}

	return -1;
}

bool UInventoryComponent::GetItemByName(FString ItemName, FItemStruct& OutItem)
{
	OutItem.Name = ItemName;
	int32 FoundIndex = CurrentItems.Find(OutItem);

	if (FoundIndex != INDEX_NONE)
	{
		OutItem = CurrentItems[FoundIndex];
		return true;
	}
	else
		return false;
}

void UInventoryComponent::AddStartupItems()
{
	const bool PreviousAutoEquip = EnableAutoEquip;
	EnableAutoEquip = false;

	for(FItemRowStruct& ItemRowStruct : DefaultItems)
	{
		FItemStruct ItemToBeAdded =
			UHelperFunctionLibrary::GetItemFromDataTable(InventoryData->ItemDataTable, ItemRowStruct.ItemRowName);

		if(ItemToBeAdded.IsValid() && GetOwner()->HasAuthority())
		{
			ItemToBeAdded.CurrentStack = ItemRowStruct.Amount;

			Server_AddItem(ItemToBeAdded);
		}
	}
	
	EnableAutoEquip = PreviousAutoEquip;
}

void UInventoryComponent::InitializeBagInventory(FItemStruct& BagItem) const
{
	UInventoryComponent* BagInventory = NewObject<UInventoryComponent>(GetOwner());
	
	BagInventory->MaximumSlotsPerRow = BagItem.BagMaximalSlotPerRow;
	BagInventory->BasicSlotAmount = BagItem.BagSlotAmount;
	BagInventory->InventoryData = InventoryData;
	BagInventory->PlayerOwnedInventory = PlayerOwnedInventory;
	BagInventory->CurrentItems.SetNum(BagInventory->BasicSlotAmount);
	BagInventory->IsBagInventory = true;
	
	BagItem.ItemBagInventory = BagInventory;
}

bool UInventoryComponent::AutoEquipItem(FItemStruct& InputItem)
{
	const int32 AutoEquipIndex = GetValidSlotForAutoEquip(InputItem.Category);

	if(AutoEquipIndex != -1)
	{
		CurrentItems[AutoEquipIndex] = InputItem;

		CurrentItems[AutoEquipIndex].CurrentStack = 1;

		InputItem.CurrentStack--;

		ApplyEquipmentStats(CurrentItems[AutoEquipIndex]);

		if(InputItem.CurrentStack == 0) return true;
	}
	
	return false;
}

bool UInventoryComponent::IsOwnerOnServer() const
{
	if(GetOwner() != nullptr)
	{
		const AController* OwnerController = GetOwner()->GetInstigatorController();

		if(OwnerController && OwnerController->IsLocalPlayerController()) return true;
	}
	
	return false;
}

void UInventoryComponent::MoveItemToEmptySlot(
	UInventoryComponent* FromWhichInventory, const int32& FromWhereIndex, const int32& IndexToMoveTo)
{
	if(IndexToMoveTo >= BasicSlotAmount)
	{
		ApplyEquipmentStats(CurrentItems[FromWhereIndex]);
	}
	else if(FromWhereIndex >= FromWhichInventory->BasicSlotAmount)
	{
		FromWhichInventory->ApplyEquipmentStats(FromWhichInventory->CurrentItems[FromWhereIndex], true);
	}

	CurrentItems[IndexToMoveTo] = FromWhichInventory->CurrentItems[FromWhereIndex];

	FromWhichInventory->CurrentItems[FromWhereIndex].Clear();
}

void UInventoryComponent::SwapItems(
	UInventoryComponent* FromWhichInventory, const int32& FromWhereIndex, const int32& ToWhereIndex)
{
	const FItemStruct SwappedItem = CurrentItems[ToWhereIndex];

	if(ToWhereIndex >= BasicSlotAmount)
	{
		ApplyEquipmentStats(SwappedItem, true);

		ApplyEquipmentStats(FromWhichInventory->CurrentItems[FromWhereIndex]);
	}

	CurrentItems[ToWhereIndex] = FromWhichInventory->CurrentItems[FromWhereIndex];
	FromWhichInventory->CurrentItems[FromWhereIndex] = SwappedItem;
}

void UInventoryComponent::MergeItems(
	UInventoryComponent* FromWhichInventory, const int32& FromWhereToMerge, const int32& WhereToMerge)
{
	FItemStruct& MergingItem = FromWhichInventory->CurrentItems[FromWhereToMerge];
	FItemStruct& MergedToItem = CurrentItems[WhereToMerge];
	
	if(MergedToItem.CurrentStack + MergingItem.CurrentStack <= MergedToItem.MaximumStack)
	{
		MergedToItem.CurrentStack += MergingItem.CurrentStack;

		FromWhichInventory->RemoveItem(FromWhereToMerge, MergingItem.CurrentStack);
		return;
	}

	MergingItem.CurrentStack -= (MergedToItem.MaximumStack - MergedToItem.CurrentStack);

	MergedToItem.CurrentStack = MergedToItem.MaximumStack;
}

bool UInventoryComponent::RemoveItem(const int32& ArrayIndex, const int32& AmountToDrop)
{
	FItemStruct& ItemToRemove = CurrentItems[ArrayIndex];

	if(!ItemToRemove.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Tried removing an item that doesn't exist in %hs"), __FUNCTION__);
		return false;
	}

	ItemToRemove.CurrentStack -= AmountToDrop;

	if(ItemToRemove.CurrentStack <= 0)
	{
		if(ArrayIndex >= BasicSlotAmount) ApplyEquipmentStats(CurrentItems[ArrayIndex], true);

		CurrentItems[ArrayIndex].Clear();
	}

	if(ItemToRemove.IsBag) Multicast_CallBagClosedDelegate(ItemToRemove.ItemBagInventory);
	
	return true;
}

void UInventoryComponent::ApplyEquipmentStats(const FItemStruct& EquipmentItem, const bool RemoveStats) const
{
	if(RemoveStats)
	{
		UE_LOG(LogTemp, Warning, TEXT("Removed Stats for item: %s"), *EquipmentItem.Name);
		if (EquipmentItem.Category == EItemCategory::Weapon)
			OnWeaponUnequipped.Broadcast(EquipmentItem.Name);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Added Stats for item: %s"), *EquipmentItem.Name);
		if (EquipmentItem.Category == EItemCategory::Weapon)
			OnWeaponEquipped.Broadcast(EquipmentItem.Name);
	}
}

void UInventoryComponent::Multicast_CallBagClosedDelegate_Implementation(UInventoryComponent* DroppedBag)
{
	OnBagClosed.Broadcast(DroppedBag);
}

void UInventoryComponent::Multicast_CallSplitterDelegate_Implementation(
	UInventoryComponent* DraggedFromInventory, int32 DraggedSlotIndex, int32 DroppedSlotIndex)
{
	OnSplittingInitialized.Broadcast(DraggedFromInventory, DraggedSlotIndex, DroppedSlotIndex);
}

void UInventoryComponent::OnRep_CurrentItems() const
{
	Multicast_UpdateInventoryUI();
}
