// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthWidget.h"
#include "HealthComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthWidget::NativeConstruct()
{
    Super::NativeConstruct();

    APawn* PlayerPawn = GetOwningPlayerPawn();
    if (!PlayerPawn)
        return;

    auto HealthComponent = PlayerPawn->FindComponentByClass<UHealthComponent>();
    if (!HealthComponent)
        return;

    HealthComponent->OnHealthChange.AddDynamic(this, &UHealthWidget::UpdateHealth);
    UpdateHealth(HealthComponent->GetCurrentHealth(), HealthComponent->GetMaxHealth());
}

void UHealthWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    if (!HealthProgressBar || !HealthValueText)
        return;

    float HealthPercent = 0.f;
    if (CurrentHealth > 0 && MaxHealth > 0)
    {
        HealthPercent = CurrentHealth / MaxHealth;
    }

    HealthProgressBar->SetPercent(HealthPercent);

    HealthValueText->SetText(FText::AsNumber(HealthPercent));
}


