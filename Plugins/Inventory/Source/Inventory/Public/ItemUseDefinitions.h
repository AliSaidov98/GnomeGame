// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemProperties.h"
#include "ItemUseDefinitions.generated.h"

/**
 * 
 */

class UInventoryComponent;

UCLASS(BlueprintType, Blueprintable, ClassGroup = "Inventory")
class INVENTORY_API UItemUseDefinitions : public UObject
{
	GENERATED_BODY()
	
public:

	UItemUseDefinitions() = default;

	UFUNCTION(BlueprintCallable, Category = "Item Use Definitions")
	void MasterFunction(UInventoryComponent* OwningInventory, FItemStruct ItemToBeUsed, int32 ItemArrayIndex);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Item Use Definitions")
	void BlueprintMasterFunction(UInventoryComponent* OwningInventory, FItemStruct ItemToBeUsed, int32 ItemArrayIndex);
	
protected:

	void EquipmentItems(UInventoryComponent* OwningInventory, int32 ItemIndex) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Item Use Definitions")
	void UseSmallPotion(FItemStruct PotionToBeUsed);

public:

	UPROPERTY(BlueprintReadOnly, Category = "Item Use Definitions", meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> OwnerActor;
	

};
