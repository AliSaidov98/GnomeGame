// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/ItemProperties.h"
#include "Engine/DataTable.h"
#include "ItemActor.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Inventory")
class INVENTORY_API AItemActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Category = "Item Actor")
	void EnableCollision();

	UFUNCTION(Category = "Item Actor")
	void OnItemBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Category = "Item Actor")
	void OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Actor", meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* ItemMesh;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Item Actor", meta = (AllowPrivateAccess = true))
	FDataTableRowHandle DataTableRow;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Item Actor", meta = (AllowPrivateAccess = true))
	int32 AmountToAdd;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Item Actor", meta = (AllowPrivateAccess = true))
	FVector FinalDropLocation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Actor", meta = (AllowPrivateAccess = true))
	bool EnableDropEffect = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Actor",
		meta = (ClampMin = "0", EditCondition="EnableDropEffect", AllowPrivateAccess = true))
	float MaxHeightWhenDropping = 75.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Actor",
		meta = (ClampMin = "0", EditCondition="EnableDropEffect", AllowPrivateAccess = true))
	float TimeRequiredForDropEffect = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Item Actor", meta = (ClampMin = "0", EditCondition = "EnableDropEffect"))
	float RandomDropAngle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Item Actor", meta = (ClampMin = "0", EditCondition = "EnableDropEffect"))
	FVector2D RandomDropDistance;

public:
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Item Actor", meta = (ExposeOnSpawn = "true"))
	FItemStruct HeldItem;

};
