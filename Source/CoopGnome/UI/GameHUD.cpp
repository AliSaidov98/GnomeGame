// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"
#include "Blueprint/UserWidget.h"
#include "MainInterfaceWidget.h"

void AGameHUD::BeginPlay()
{
    Super::BeginPlay();

    ShowMainInterface();
}

void AGameHUD::ShowMainInterface()
{
    if (!InterfaceWidgetClass)
    {
        // todo: log
        return;
    }

    APlayerController* PlayerController = Cast<APlayerController>(GetOwningPlayerController());
    if (!PlayerController)
        return;

    if (InterfaceWidget)
    {
        InterfaceWidget->SetVisibility(ESlateVisibility::Visible);
        return;
    }

    InterfaceWidget = CreateWidget<UMainInterfaceWidget>(PlayerController, InterfaceWidgetClass);
    InterfaceWidget->AddToViewport();
    InterfaceWidget->SetVisibility(ESlateVisibility::Visible);

}

void AGameHUD::ShowInventory()
{
}

void AGameHUD::SetHUDPackage(const FHUDPackage& Package)
{
    HUDPackage = Package;
}

void AGameHUD::DrawHUD()
{
    Super::DrawHUD();

    FVector2D ViewportSize;
    if(GEngine)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        const FVector2D ViewportCenter(ViewportSize.X / 2, ViewportSize.Y / 2);

        float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;
        
        if(HUDPackage.CrosshairsCenter)
        {
            FVector2D Spread(0, 0.f);
            DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
        }
        if (HUDPackage.CrosshairsLeft)
        {
            FVector2D Spread(-SpreadScaled, 0.f);
            DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
        }
        if (HUDPackage.CrosshairsRight)
        {
            FVector2D Spread(SpreadScaled, 0.f);
            DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
        }
        if (HUDPackage.CrosshairsTop)
        {
            FVector2D Spread(0.f, -SpreadScaled);
            DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
        }
        if (HUDPackage.CrosshairsBottom)
        {
            FVector2D Spread(0.f, SpreadScaled);
            DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
        }
    }
}

void AGameHUD::DrawCrosshair(UTexture2D* Texture, FVector2d ViewportCenter, FVector2d Spread, FLinearColor CrosshairColor)
{
    const float TextureWidth = Texture->GetSizeX();
    const float TextureHeight = Texture->GetSizeY();
    const FVector2D TextureDrawPoint(
        ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
        ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
    );

    DrawTexture(
        Texture,
        TextureDrawPoint.X,
        TextureDrawPoint.Y,
        TextureWidth,
        TextureHeight,
        0.f,
        0.f,
        1.f,
        1.f,
        CrosshairColor
    );
}



