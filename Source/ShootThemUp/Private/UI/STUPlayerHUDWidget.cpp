// Shoot Them Up Game. All Rights Reserved

#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponComponent.h"
#include "STUUtils.h"

void USTUPlayerHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    // подписываемся на делегат, который будет сообщать нам о том, что у персонажа новый паун (респавнился после смерти)
    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &USTUPlayerHUDWidget::OnNewPawn);
        // в первый раз нужно вызвать функцию OnNewPawn явно (т.к. инициилизация происходит до On Possesd):
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void USTUPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
    // получем HealthComponent:
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(NewPawn);
    // если он ненулевой то биндимся на делегат OnHealthChange:
    if (HealthComponent && !HealthComponent->OnHealthChanged.IsBoundToObject(this))
    {
        HealthComponent->OnHealthChanged.AddUObject(this, &USTUPlayerHUDWidget::OnHealthChanged);
    }
}

void USTUPlayerHUDWidget::OnHealthChanged(float Health, float HealthDelta)
{
    // Оповещаем блупринт о возникновении этого события если жизнь уменьшается:
    if (HealthDelta < 0.0f)
    {
        OnTakeDamage();
    }
}

float USTUPlayerHUDWidget::GetHealthPercent() const
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    if (!HealthComponent)
        return 0.0f;

    return HealthComponent->GetHealthPercent();
}

// по сути, это функция-обёртка над аналогичной функцией в WeaponComponent, чтобы не делать данное преобразоывник в BluePrint
bool USTUPlayerHUDWidget::GetCurrentWeaponUIData(FWeaponUIData& UIData) const
{
    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    if (!WeaponComponent)
        return false;

    return WeaponComponent->GetCurrentWeaponUIData(UIData);
}

bool USTUPlayerHUDWidget::GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const
{
    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    if (!WeaponComponent)
        return false;

    return WeaponComponent->GetCurrentWeaponAmmoData(AmmoData);
}

bool USTUPlayerHUDWidget::IsPlayerAlive() const
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());

    return HealthComponent && !HealthComponent->IsDead(); // вообще, срабатывать будет на нулевой указатель HealthComponent
}

bool USTUPlayerHUDWidget::IsPlayerSpectating() const
{
    const auto Controller = GetOwningPlayer();

    return Controller && Controller->GetStateName() == NAME_Spectating;
}