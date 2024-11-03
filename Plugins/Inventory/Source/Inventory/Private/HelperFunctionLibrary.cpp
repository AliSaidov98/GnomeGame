// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperFunctionLibrary.h"
#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ItemActor.h"

bool UHelperFunctionLibrary::DoesItemExist(FItemStruct Item) 
{
	return Item.IsValid();
}

FItemStruct UHelperFunctionLibrary::GetItemFromDataTable(UDataTable* InputTable, const FName& RowName) 
{
	if(InputTable != nullptr)
	{
		FItemStruct* ItemFromTable = InputTable->FindRow<FItemStruct>(RowName, "");

		if(ItemFromTable != nullptr) return *ItemFromTable;
	}
	else UE_LOG(LogTemp, Error, TEXT("Passed in a null data table in %hs"), __FUNCTION__);
    
	return FItemStruct();
}

bool UHelperFunctionLibrary::SpawnItemActor(
    const FItemStruct InputItem, AActor* OwningActor, TSubclassOf<AItemActor> ItemClass) 
{
    if(OwningActor == nullptr || !InputItem.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid OwningActor or Item passed into %hs"), __FUNCTION__);
        return false;
    }
    
    FVector SpawnLocation, DummyVector;

    OwningActor->GetActorBounds(true, SpawnLocation, DummyVector);

    FTransform ActorTransform;
    ActorTransform.SetLocation(SpawnLocation);

    AItemActor* ItemActor = OwningActor->GetWorld()->SpawnActorDeferred<AItemActor>(
        ItemClass, ActorTransform, 
        OwningActor, OwningActor->GetInstigator(), 
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );

    if(ItemActor != nullptr)
    {
        ItemActor->HeldItem = InputItem;

        ItemActor->FinishSpawning(ActorTransform);

        return true;
    }

    UE_LOG(LogTemp, Error, TEXT("Item Actor was not spawned properly in %hs"), __FUNCTION__);
    return false;
}

FVector UHelperFunctionLibrary::FindItemDropLocation(AActor* ItemActor, float MaxHeight,
    float DropAngle, FVector2D RandomDistance, FVector MeshOffset, float DebugDrawTime) 
{
	FVector TraceStart, ItemActorExtent, NotFoundLocation;

    ItemActor->GetActorBounds(true, TraceStart, ItemActorExtent);

    TraceStart = ItemActor->GetActorLocation();

    TraceStart.Z += MaxHeight;

    FHitResult TraceHitResult;

    FCollisionQueryParams QueryParams;

    QueryParams.AddIgnoredActor(ItemActor);
    QueryParams.AddIgnoredActor(ItemActor->GetOwner());

    ItemActor->GetWorld()->LineTraceSingleByChannel(
        TraceHitResult, TraceStart, TraceStart - FVector(0, 0, 2000), ECC_Visibility, QueryParams
    );

    if(TraceHitResult.bBlockingHit)
    {
        FVector TraceEnd, TraceExtent;
        
        NotFoundLocation = FVector(
            ItemActor->GetActorLocation().X,
            ItemActor->GetActorLocation().Y,
            TraceHitResult.Location.Z + 1
        );

        TraceExtent = FVector(ItemActorExtent.X, ItemActorExtent.Y, TraceHitResult.Distance / 2);

        TraceStart += TraceHitResult.Location + FVector(0, 0, 1);
        TraceStart /= 2;

        TArray<AActor*> ActorsToIgnore;

        ActorsToIgnore.Add(ItemActor);
        ActorsToIgnore.Add(ItemActor->GetOwner());

        FVector OwnerForwardVector = ItemActor->GetOwner()->GetActorForwardVector();

        FVector RandomDirection;
        FRotator RandomRotation;

        for(int i = 0; i <= 50; i++)
        {
            RandomRotation = FRotator::MakeFromEuler(
                FVector(0.0f, 0.0f, FMath::RandRange(-DropAngle, DropAngle))
            );

            RandomDirection = RandomRotation.RotateVector(OwnerForwardVector);
            
            RandomDirection *= FMath::RandRange(RandomDistance.X, RandomDistance.Y);

            TraceEnd = TraceStart + FVector(
                RandomDirection.X + ItemActorExtent.X * 2,
                RandomDirection.Y + ItemActorExtent.Y * 2,
                0
            );

            UKismetSystemLibrary::BoxTraceSingle(
                ItemActor->GetWorld(),
                TraceStart,
                TraceEnd,
                TraceExtent,
                FRotator::ZeroRotator,
                UEngineTypes::ConvertToTraceType(ECC_Visibility),
                false,
                ActorsToIgnore,
                EDrawDebugTrace::ForDuration,
                TraceHitResult,
                true,
                FLinearColor::Red,
                FLinearColor::Green,
                DebugDrawTime
            );

            if(TraceHitResult.bBlockingHit == false)
            {
                return FVector(
                    TraceEnd.X + MeshOffset.X,
                    TraceEnd.Y + MeshOffset.Y,
                    TraceEnd.Z - TraceExtent.Z
                );
            }
        }
    }
    else
    {  
        return FVector::ZeroVector;
    }

    return NotFoundLocation;
}
