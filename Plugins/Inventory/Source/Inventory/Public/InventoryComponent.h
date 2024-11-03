// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ItemProperties.h"
#include "InventoryComponent.generated.h"


class AItemActor;
class UInventoryInfo;
class UContainerInventoryWidget;
class UInventoryWidget;
class UItemUseDefinitions;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOpenedOrClosedInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCloseBag, UInventoryComponent*, DroppedBag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquipped, FString, WeaponName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponUnequipped, FString, WeaponName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(FSplittingInitialized, UInventoryComponent*, DraggedFromInventory, int32, DraggedSlotIndex, int32, DroppedSlotIndex);

UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent), ClassGroup = "Inventory")
class INVENTORY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UInventoryComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
	virtual void BeginPlay() override;

public:

	UFUNCTION(Server, Reliable, Category = "Inventory Functions")
	void Server_ConnectInventory(UInventoryComponent* NewConnectedInventory, const bool MutualConnection = false);

	UFUNCTION(Server, Reliable, Category = "Inventory Functions")
	void Server_SetPlayerOwnedInventory(UInventoryComponent* ForWhichInventory);

	UFUNCTION(Server, Reliable, Category = "Inventory Functions")
	void Server_SetItemInUse(UInventoryComponent* ForWhichInventory, const int32& ItemIndex, bool InUse = true);

	UFUNCTION(Server, Reliable, Category = "Inventory Functions")
	void Server_DecrementUsedItemStack(UInventoryComponent* FromWhichInventory, const int32& ItemIndex);

	UFUNCTION(Server, Reliable, Category = "Inventory Functions")
	void Server_NewSlotAdded(UInventoryComponent* ForWhichInventory, EItemCategory Category, const int32& InsertPosition = -1);

	UFUNCTION(Server, Reliable, Category = "Inventory Functions")
	void Server_RemoveLastBasicSlot(UInventoryComponent* ForWhichInventory);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory Functions")
	void Server_AddItem(FItemStruct InputItem);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory Functions")
	void Server_DropItemFromInventory(UInventoryComponent* FromWhichInventory, const int32& DroppedItemIndex);

	UFUNCTION(Server, Reliable, Category = "Inventory Functions")
	void Server_HalveItemStack(UInventoryComponent* WhereToHalve, const int32& ItemArrayIndex);

	UFUNCTION(Server, Reliable, Category = "Inventory Functions")
	void Server_SplitItem(UInventoryComponent* FromWhichInventory, UInventoryComponent* InventoryForSplit, const int32& FromWhereToSplit, const int32& SplitToIndex, int32 AmountToSplit);

	UFUNCTION(Server, Reliable, Category = "Inventory Functions")
	void Server_TransferToConnectedInventory(UInventoryComponent* FromWhichInventory,
		const bool TransferJustOne = true, const int32 SpecificTransferIndex = -1);

	UFUNCTION(Server, Reliable, Category = "Inventory Functions")
	void Server_OnItemDropped(UInventoryComponent* DraggedFromInventory, UInventoryComponent* DroppedOnInventory,
		const EItemCategory& SlotAcceptableCategory, const int32& DraggedSlotIndex, const int32& DroppedSlotIndex, const bool ShouldSplitItem);

	UFUNCTION(Category = "Inventory Functions")
	bool CanSplitItem(const FItemStruct& SplitItem, const FItemStruct& DestinationItem, const EItemCategory& SlotAcceptableCategory) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Functions")
	void BlueprintApplyEquipmentStats(const FItemStruct& EquipmentItem, const bool RemoveStats = false);
	
	UFUNCTION(NetMulticast, Reliable, Category = "Inventory Functions")
	void Multicast_UpdateInventoryUI() const;

	UFUNCTION(Category = "Inventory Functions")
	FORCEINLINE bool GetHalveStackKeyPressed() const { return HalveStackKeyPressed; }

	UFUNCTION(Category = "Inventory Functions")
	FORCEINLINE bool GetSplitKeyPressed() const { return SplitKeyPressed; }
	
	UFUNCTION(Category = "Inventory Functions")
	FORCEINLINE int32 GetBasicSlotAmount() const { return BasicSlotAmount; }

	UFUNCTION(Category = "Inventory Functions")
	FORCEINLINE int32 GetMaximumSlotsPerRow() const { return MaximumSlotsPerRow; }

	UFUNCTION(Category = "Inventory Functions")
	FORCEINLINE int32 GetLastAddedItemStack() const { return LastAddedItemStack; }

	UFUNCTION(BlueprintCallable, Category = "Inventory Functions")
	int32 GetValidSlotForAutoEquip(const EItemCategory& InItemCategory, const bool EmptySlotsOnly = true);
	
	UFUNCTION(Category = "Inventory Functions")
	FORCEINLINE void SetBasicSlotAmount(const int32& NewSlotAmount) { BasicSlotAmount = NewSlotAmount; }
	
	bool GetItemByName(FString ItemName, FItemStruct& OutItem);

private:

	UFUNCTION(Category = "Inventory Functions")
	void AddStartupItems();

	UFUNCTION(Category = "Inventory Functions")
	void InitializeBagInventory(FItemStruct& BagItem) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory Functions")
	bool AutoEquipItem(FItemStruct& InputItem);

	UFUNCTION(Category = "Inventory Functions")
	bool IsOwnerOnServer() const;

	UFUNCTION(Category = "Inventory Functions")
	void MoveItemToEmptySlot(UInventoryComponent* FromWhichInventory, const int32& FromWhereIndex, const int32& IndexToMoveTo);

	UFUNCTION(Category = "Inventory Functions")
	void SwapItems(UInventoryComponent* FromWhichInventory, const int32& FromWhereIndex, const int32& ToWhereIndex);

	UFUNCTION(Category = "Inventory Functions")
	void MergeItems(UInventoryComponent* FromWhichInventory, const int32& FromWhereToMerge, const int32& WhereToMerge);

	UFUNCTION(Category = "Inventory Functions")
	bool RemoveItem(const int32& ArrayIndex, const int32& AmountToDrop);

	UFUNCTION(Category = "Inventory Functions")
	void ApplyEquipmentStats(const FItemStruct& EquipmentItem, const bool RemoveStats = false) const;

	UFUNCTION(NetMulticast, Reliable, Category = "Inventory Functions")
	void Multicast_CallBagClosedDelegate(UInventoryComponent* DroppedBag);

	UFUNCTION(NetMulticast, Reliable, Category = "Inventory Functions")
	void Multicast_CallSplitterDelegate(UInventoryComponent* DraggedFromInventory, int32 DraggedSlotIndex, int32 DroppedSlotIndex);

	UFUNCTION(Category = "Inventory Functions")
	void OnRep_CurrentItems() const;

public:
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Inventory")
	FOpenedOrClosedInventory OnOpenedOrClosedInventory;

	UPROPERTY(BlueprintCallable, Category = "Inventory")
	FInventoryUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintCallable, Category = "Inventory")
	FCloseBag OnBagClosed;

	UPROPERTY(BlueprintCallable, Category = "Inventory")
	FSplittingInitialized OnSplittingInitialized;

	UPROPERTY(BlueprintCallable, Category = "Inventory")
	FOnWeaponEquipped OnWeaponEquipped;

	UPROPERTY(BlueprintCallable, Category = "Inventory")
	FOnWeaponUnequipped OnWeaponUnequipped;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	bool IsInventoryOpen = false;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentItems, BlueprintReadWrite, Category = "Inventory")
	TArray<FItemStruct> CurrentItems;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UInventoryComponent> PlayerOwnedInventory = this;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UInventoryComponent> ConnectedInventory = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UItemUseDefinitions> InventoryUseDefinitions = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryInfo> InventoryData;
	
private:
	
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Inventory")
	bool EnableAutoEquip = false;
	
	UPROPERTY(BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = true))
	bool SplitKeyPressed = false;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = true))
	bool HalveStackKeyPressed = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	bool IsBagInventory = false;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FItemRowStruct> DefaultItems;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	TArray<EItemCategory> EquipmentSlotAcceptableItems;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	int32 LastAddedItemStack = 0;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	int32 BasicSlotAmount = 10;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	int32 MaximumSlotsPerRow = 5;

};
