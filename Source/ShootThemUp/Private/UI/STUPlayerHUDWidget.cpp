// Shoot Them Up Game. All Rights Reserved

#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponComponent.h"

float USTUPlayerHUDWidget::GetHealthPercent() const
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
        return 0.0f;

    const auto Component = Player->GetComponentByClass(USTUHealthComponent::StaticClass()); // на null можем не проверять, т.к. это проверяется в cast'е

    // GetComponentByClass возвращает указатель на UActorComponent, по этому нам нужно привести его к USTUHealthComponent:
    const auto HealthComponent = Cast<USTUHealthComponent>(Component);
    if (!HealthComponent)
        return 0.0f;

    return HealthComponent->GetHealthPercent();
}

bool USTUPlayerHUDWidget::GetWeaponUIData(FWeaponUIData& UIData) const
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
        return false;

    const auto Component = Player->GetComponentByClass(USTUWeaponComponent::StaticClass()); // на null можем не проверять, т.к. это проверяется в cast'е

    // GetComponentByClass возвращает указатель на UActorComponent, по этому нам нужно привести его к USTUHealthComponent:
    const auto WeaponComponent = Cast<USTUWeaponComponent>(Component);
    if (!WeaponComponent)
        return false;

    return WeaponComponent->GetWeaponUIData(UIData);
}