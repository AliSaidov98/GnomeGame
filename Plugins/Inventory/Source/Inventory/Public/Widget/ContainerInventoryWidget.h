// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ContainerInventoryWidget.generated.h"


class UInventoryWidget;
class UInventoryComponent;
class UButton;

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Inventory Widgets")
class INVENTORY_API UContainerInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

	
public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Container Widget")
	void SetNewContainerInventory(UInventoryComponent* InInventory, const AActor* InteractingPlayer);

protected:

	UFUNCTION(BlueprintCallable, Category = "Container Widget")
	void CloseContainerWidget();
	
	UFUNCTION(Category = "Container Widget")
	void OnTakeSingleButtonClicked();
	
	UFUNCTION(Category = "Container Widget")
	void OnTakeAllButtonClicked();

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
private:

	UPROPERTY(BlueprintReadWrite, Category = "Container Widget", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UInventoryWidget> InventoryWidget;
	
	UPROPERTY(BlueprintReadWrite, Category = "Container Widget", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> TakeSingleButton;

	UPROPERTY(BlueprintReadWrite, Category = "Container Widget", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> TakeAllButton;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> CurrentContainerInventory;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> PlayerInventoryReference;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerControllerReference;

};
