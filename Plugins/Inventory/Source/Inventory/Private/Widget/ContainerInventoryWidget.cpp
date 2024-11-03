// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ContainerInventoryWidget.h"
#include "Widget/InventoryWidget.h"
#include "InventoryComponent.h"
#include "Components/Button.h"
#include "Widget/InventoryDragOperation.h"


void UContainerInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TakeSingleButton->OnClicked.AddDynamic(this, &UContainerInventoryWidget::OnTakeSingleButtonClicked);
	TakeAllButton->OnClicked.AddDynamic(this, &UContainerInventoryWidget::OnTakeAllButtonClicked);

	SetIsFocusable(true);
}

void UContainerInventoryWidget::SetNewContainerInventory(UInventoryComponent* InInventory, const AActor* InteractingPlayer)
{
	if(InInventory == nullptr || InteractingPlayer == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Null inventory or null actor passed to %hs"), __FUNCTION__);
		return;
	}

	PlayerInventoryReference = InteractingPlayer->FindComponentByClass<UInventoryComponent>();
	PlayerControllerReference = Cast<APlayerController>(InteractingPlayer->GetInstigatorController());

	if(PlayerInventoryReference == nullptr || PlayerControllerReference == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Interacting Actors' inventory or controller is null in %hs"), __FUNCTION__);
		return;
	}
	
	CurrentContainerInventory = InInventory;

	InventoryWidget->InitializeInventoryWidget(CurrentContainerInventory);

	PlayerInventoryReference->Server_SetPlayerOwnedInventory(CurrentContainerInventory);
	
	PlayerInventoryReference->Server_ConnectInventory(CurrentContainerInventory, true);

	if(!PlayerInventoryReference->IsInventoryOpen)
	{
		PlayerControllerReference->SetShowMouseCursor(true);

		FInputModeGameAndUI InputMode;
		
		UWidget* WidgetPtr = Cast<UWidget>(this);
		InputMode.SetWidgetToFocus(WidgetPtr->TakeWidget());

		PlayerControllerReference->SetInputMode(InputMode);		
	}
}

void UContainerInventoryWidget::CloseContainerWidget()
{
	PlayerInventoryReference->Server_ConnectInventory(nullptr);
	
	CurrentContainerInventory->OnOpenedOrClosedInventory.Broadcast();

	if(!PlayerInventoryReference->IsInventoryOpen)
	{
		PlayerControllerReference->SetShowMouseCursor(false);

		const FInputModeGameOnly InputMode;
		PlayerControllerReference->SetInputMode(InputMode);		
	}
	
	RemoveFromParent();
}

void UContainerInventoryWidget::OnTakeSingleButtonClicked()
{
	PlayerInventoryReference->Server_TransferToConnectedInventory(CurrentContainerInventory, true);
}

void UContainerInventoryWidget::OnTakeAllButtonClicked()
{
	PlayerInventoryReference->Server_TransferToConnectedInventory(CurrentContainerInventory, false);
}

bool UContainerInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if(const UInventoryDragOperation* DragOperation = Cast<UInventoryDragOperation>(InOperation))
	{
		DragOperation->WhereItemCameFrom->Multicast_UpdateInventoryUI();
	}

	return true;
}



