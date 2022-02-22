// Shoot Them Up Game. All Rights Reserved

#include "Pickups/STUHealthPickup.h"
#include "Components/STUHealthComponent.h"
#include "STUUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthPickup, All, All);

bool ASTUHealthPickup::GivePickUpTo(APawn* PlayerPawn)
{
    // получаем компонент здоровья по шаблону из STUUtils.h
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(PlayerPawn);
    if (!HealthComponent)
        return false;

    return HealthComponent->TryToAddHealth(HealthAmount);
}