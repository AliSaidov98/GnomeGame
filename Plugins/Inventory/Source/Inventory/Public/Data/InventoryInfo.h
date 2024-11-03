#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InventoryInfo.generated.h"

class AItemActor;
class UDataTable;
class UInventorySlotWidget;
class UItemTooltipWidget;
class UInventoryWidget;
class UItemUseDefinitions;
class USplitterWidget;
class UWidget;

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = "Inventory")
class INVENTORY_API UInventoryInfo : public UDataAsset
{
	GENERATED_BODY()

	friend class UInventoryComponent;
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory Defaults")
	UDataTable* ItemDataTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Defaults")
	TSubclassOf<AItemActor> ItemActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Defaults")
	TSubclassOf<UWidget> DragVisualClass;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Defaults")
	TSubclassOf<UItemUseDefinitions> ItemUseDefinitionsClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Defaults")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Defaults")
	TSubclassOf<UItemTooltipWidget> TooltipWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Defaults")
	TSubclassOf<USplitterWidget> SplitterWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Defaults")
	TSubclassOf<UInventoryWidget> BagWidgetClass;
};

