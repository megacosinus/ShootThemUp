// Shoot Them Up Game. All Rights Reserved

#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponComponent.h"

float USTUPlayerHUDWidget::GetHealthPercent() const
{
    const auto HealthComponent = GetHealthComponent();
    if (!HealthComponent)
        return 0.0f;

    return HealthComponent->GetHealthPercent();
}

// по сути, это функция-обёртка над аналогичной функцией в WeaponComponent, чтобы не делать данное преобразоывник в BluePrint
bool USTUPlayerHUDWidget::GetCurrentWeaponUIData(FWeaponUIData& UIData) const
{
    const auto WeaponComponent = GetWeaponComponent();
    if (!WeaponComponent)
        return false;

    return WeaponComponent->GetCurrentWeaponUIData(UIData);
}

bool USTUPlayerHUDWidget::GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const
{
    const auto WeaponComponent = GetWeaponComponent();
    if (!WeaponComponent)
        return false;

    return WeaponComponent->GetCurrentWeaponAmmoData(AmmoData);
}

bool USTUPlayerHUDWidget::IsPlayerAlive() const
{
    const auto HealthComponent = GetHealthComponent();

    return HealthComponent && !HealthComponent->IsDead(); // вообще, срабатывать будет на нулевой указатель HealthComponent
}

bool USTUPlayerHUDWidget::IsPlayerSpectating() const
{
    const auto Controller = GetOwningPlayer();

    return Controller && Controller->GetStateName() == NAME_Spectating;
}

USTUWeaponComponent* USTUPlayerHUDWidget::GetWeaponComponent() const
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
        return nullptr;

    const auto Component = Player->GetComponentByClass(USTUWeaponComponent::StaticClass()); // на null можем не проверять, т.к. это проверяется в cast'е

    // GetComponentByClass возвращает указатель на UActorComponent, по этому нам нужно привести его к USTUHealthComponent:
    const auto WeaponComponent = Cast<USTUWeaponComponent>(Component);
    return WeaponComponent;
}

USTUHealthComponent* USTUPlayerHUDWidget::GetHealthComponent() const
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
        return nullptr;

    const auto Component = Player->GetComponentByClass(USTUHealthComponent::StaticClass()); // на null можем не проверять, т.к. это проверяется в cast'е

    // GetComponentByClass возвращает указатель на UActorComponent, по этому нам нужно привести его к USTUHealthComponent:
    const auto HealthComponent = Cast<USTUHealthComponent>(Component);
    return HealthComponent;
}
