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

// �� ����, ��� �������-������ ��� ����������� �������� � WeaponComponent, ����� �� ������ ������ �������������� � BluePrint
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

    return HealthComponent && !HealthComponent->IsDead(); // ������, ����������� ����� �� ������� ��������� HealthComponent
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

    const auto Component = Player->GetComponentByClass(USTUWeaponComponent::StaticClass()); // �� null ����� �� ���������, �.�. ��� ����������� � cast'�

    // GetComponentByClass ���������� ��������� �� UActorComponent, �� ����� ��� ����� �������� ��� � USTUHealthComponent:
    const auto WeaponComponent = Cast<USTUWeaponComponent>(Component);
    return WeaponComponent;
}

USTUHealthComponent* USTUPlayerHUDWidget::GetHealthComponent() const
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
        return nullptr;

    const auto Component = Player->GetComponentByClass(USTUHealthComponent::StaticClass()); // �� null ����� �� ���������, �.�. ��� ����������� � cast'�

    // GetComponentByClass ���������� ��������� �� UActorComponent, �� ����� ��� ����� �������� ��� � USTUHealthComponent:
    const auto HealthComponent = Cast<USTUHealthComponent>(Component);
    return HealthComponent;
}
