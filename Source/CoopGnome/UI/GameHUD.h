// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class UMainInterfaceWidget;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
};

UCLASS()
class COOPGNOME_API AGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ShowMainInterface();

	void ShowInventory();
	void SetHUDPackage(const FHUDPackage& HUDPackage);

	virtual void DrawHUD() override;
	void DrawCrosshair(UTexture2D* Texture, FVector2d ViewportCenter, FVector2d Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMainInterfaceWidget> InterfaceWidgetClass;

private:
	TObjectPtr<UMainInterfaceWidget> InterfaceWidget;
	
	FHUDPackage HUDPackage;
	
};
