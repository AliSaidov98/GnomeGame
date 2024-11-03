// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "InventoryComponent.h"
#include "TimerManager.h"
#include "HelperFunctionLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AItemActor::AItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("Item Mesh");
	SetRootComponent(ItemMesh);

	ItemMesh->SetGenerateOverlapEvents(true);
		
	ItemMesh->SetEnableGravity(false);

	ItemMesh->SetSimulatePhysics(true);

	bReplicates = true;
	bAlwaysRelevant = true;

}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	if(!HeldItem.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemActor has no item selected for it."));
			
		Destroy();
		return;
	}

	if(HeldItem.ItemMesh != nullptr) ItemMesh->SetStaticMesh(HeldItem.ItemMesh);

	if(GetOwner() != nullptr)
	{
		if(GetOwner()->HasAuthority())
		{
			FTimerHandle DropTimerHandle;
			
			GetWorld()->GetTimerManager().SetTimer(
				DropTimerHandle, 
				this, 
				&AItemActor::EnableCollision, 
				TimeRequiredForDropEffect, 
				false,
				TimeRequiredForDropEffect
			);

			if(EnableDropEffect)
			{
				const FVector MeshMassCentre = GetActorLocation() - ItemMesh->GetCenterOfMass();
				SetActorLocation(GetActorLocation() + MeshMassCentre);

				FinalDropLocation = UHelperFunctionLibrary::FindItemDropLocation(
					this,
					MaxHeightWhenDropping,
					RandomDropAngle,
					RandomDropDistance,
					MeshMassCentre,
					10.0f
				);
			}
		}
	}
	else ItemMesh->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnItemBeginOverlap);

}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemActor, HeldItem);
	DOREPLIFETIME(AItemActor, FinalDropLocation);
}

void AItemActor::EnableCollision() 
{ 
	ItemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	ItemMesh->SetEnableGravity(true);
	
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for(const AActor* CurrentActor : OverlappingActors)
	{
		const AController* ActorController = CurrentActor->GetInstigatorController();

		if(ActorController && ActorController->IsA<APlayerController>())
		{
			ItemMesh->OnComponentEndOverlap.AddDynamic(this, &AItemActor::OnItemEndOverlap);
			return;
		}
	}
	ItemMesh->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnItemBeginOverlap);
}

void AItemActor::OnItemBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	const AController* OtherActorController = OtherActor->GetInstigatorController();

	if(OtherActorController && OtherActorController->IsA<APlayerController>())
	{
		UInventoryComponent* PlayerInventory = OtherActor->FindComponentByClass<UInventoryComponent>();

		if(PlayerInventory != nullptr && PlayerInventory->GetOwner()->HasAuthority())
		{
			PlayerInventory->Server_AddItem(HeldItem);

			if(PlayerInventory->GetLastAddedItemStack() == 0)
			{
				Destroy();
			}
			else HeldItem.CurrentStack = PlayerInventory->GetLastAddedItemStack();
		}
	}
}

void AItemActor::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!ItemMesh->OnComponentBeginOverlap.IsBound())
	{
		ItemMesh->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnItemBeginOverlap);		
	}
}
