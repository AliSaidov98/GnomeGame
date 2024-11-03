// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemUseDefinitions.h"
#include "InventoryComponent.h"

void UItemUseDefinitions::MasterFunction(
	UInventoryComponent* OwningInventory, FItemStruct ItemToBeUsed, const int32 ItemArrayIndex)
{
	if(ItemToBeUsed.IsEquipment) return EquipmentItems(OwningInventory, ItemArrayIndex);

	if(ItemToBeUsed.Name == "Minor Healing Potion")
	{
		OwningInventory->CurrentItems[ItemArrayIndex].CurrentStack--;
		UseSmallPotion(ItemToBeUsed);
	}
}

void UItemUseDefinitions::EquipmentItems(UInventoryComponent* OwningInventory, int32 ItemIndex) const
{
	if(OwningInventory->GetBasicSlotAmount() >= OwningInventory->CurrentItems.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Player Inventory has no Equipment slots, error in %hs"), __FUNCTION__);
		return;
	}

	const FItemStruct ItemToBeUsed = OwningInventory->CurrentItems[ItemIndex];

	const int32 EquipmentSlotIndex = OwningInventory->GetValidSlotForAutoEquip(ItemToBeUsed.Category, false);
	if(EquipmentSlotIndex == -1) return;
    
	OwningInventory->Server_OnItemDropped(
		OwningInventory,
		OwningInventory,
		ItemToBeUsed.Category,
		ItemIndex,
		EquipmentSlotIndex,
		false
	);
}
