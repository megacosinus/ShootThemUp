// Shoot Them Up Game. All Rights Reserved

#include "Pickups/STUAmmoPickup.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponComponent.h"
#include "STUUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogAmmoPickup, All, All);

bool ASTUAmmoPickup::GivePickUpTo(APawn* PlayerPawn)
{
    // проверяем, жив ли наш персонаж (может, просто рухнул мёртвым
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(PlayerPawn); // получаем указатель на Хелс компонент (шаблон лежит в STUUtils)
    if (!HealthComponent || HealthComponent->IsDead())
        return false;

    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(PlayerPawn); // получаем указатель на Випон компонент
    if (!WeaponComponent)
        return false;

    return WeaponComponent->TryToAddAmmo(WeaponType, ClipsAmount);
}