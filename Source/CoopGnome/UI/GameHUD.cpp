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

void AGameHUD::SetHUDPackage(const FHUDPackage& HUDPackage)
{
}



