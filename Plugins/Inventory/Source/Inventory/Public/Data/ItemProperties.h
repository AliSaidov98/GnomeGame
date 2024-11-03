#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemProperties.generated.h"

class UInventoryComponent;
class UTexture2D;
class UStaticMesh;

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	Everything UMETA(DisplayName = "Everything"),
	Weapon UMETA(DisplayName = "Weapon"),
	Head UMETA(DisplayName = "Head"),
	BodyArmor UMETA(DisplayName = "Body Armor"),
	Arms UMETA(DisplayName = "Arms"),
	Legs UMETA(DisplayName = "Legs"),
	Boots UMETA(DisplayName = "Boots"),
	Ammo UMETA(DisplayName = "Ammo"),
	Health UMETA(DisplayName = "Health"),
	Consumable UMETA(DisplayName = "Consumable")
};

USTRUCT(BlueprintType)
struct FItemRowStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Row Struct")
	FName ItemRowName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Row Struct", meta = (ClampMin = "0"))
	int32 Amount = 1;
};

USTRUCT(BlueprintType)
struct FItemStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	FString Name = "None";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	EItemCategory Category = EItemCategory::Everything;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (EditCondition = "Category==EItemCategory::Weapon", EditConditionHides))
	TSubclassOf<AActor> WeaponClass;

	UPROPERTY(BlueprintReadOnly, Category = "Item Properties")
	bool IsItemInUse = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (EditCondition = "!IsBag"))
	bool IsEquipment = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bag Properties", meta = (EditCondition = "!IsEquipment"))
	bool IsBag = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bag Properties", meta = (EditCondition = "IsBag"))
	int32 BagSlotAmount = 12;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bag Properties", meta = (EditCondition = "IsBag"))
	int32 BagMaximalSlotPerRow = 3;	

	UPROPERTY(BlueprintReadOnly, Category = "Bag Properties")
	TObjectPtr<UInventoryComponent> ItemBagInventory = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	FString Description = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FString ConsumableDescription = FString();
	
	UPROPERTY(BlueprintReadWrite, Category = "Item Properties")
	int32 CurrentStack = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (ClampMin = "1"))
	int32 MaximumStack = 99;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (ClampMin = "0"))
	int32 UseDecrementStackAmount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	TObjectPtr<UTexture2D> Thumbnail = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	TObjectPtr<UStaticMesh> ItemMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	TArray<int32> ItemValue =  {0, 0, 0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Armor = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CriticalChance = 0.0f;

	bool IsValid() const
	{
		return Name != "None" && CurrentStack > 0;
	}

	void Clear()
	{
		Name = "None";
	}

	bool operator==(const FItemStruct& OtherItem) const
	{
		return Name == OtherItem.Name;
	}

	void DecrementStackWhenUsed()
	{
		CurrentStack -= UseDecrementStackAmount;

		if(CurrentStack < 0) CurrentStack = 0;
	}
};

