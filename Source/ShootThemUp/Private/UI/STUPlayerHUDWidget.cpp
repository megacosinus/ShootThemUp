// Shoot Them Up Game. All Rights Reserved

#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/ProgressBar.h"
#include "STUUtils.h"
#include "Player/STUPlayerState.h"

void USTUPlayerHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    // ������������� �� �������, ������� ����� �������� ��� � ���, ��� � ��������� ����� ���� (����������� ����� ������)
    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &USTUPlayerHUDWidget::OnNewPawn);
        // � ������ ��� ����� ������� ������� OnNewPawn ���� (�.�. ������������� ���������� �� On Possesd):
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void USTUPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
    // ������� HealthComponent:
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(NewPawn);
    // ���� �� ��������� �� �������� �� ������� OnHealthChange:
    if (HealthComponent && !HealthComponent->OnHealthChanged.IsBoundToObject(this))
    {
        HealthComponent->OnHealthChanged.AddUObject(this, &USTUPlayerHUDWidget::OnHealthChanged);
    }
    UpdateHealthBar();
}

void USTUPlayerHUDWidget::OnHealthChanged(float Health, float HealthDelta)
{
    // ��������� �������� � ������������� ����� ������� ���� ����� �����������:
    if (HealthDelta < 0.0f)
    {
        OnTakeDamage();
    }
    UpdateHealthBar();
}

float USTUPlayerHUDWidget::GetHealthPercent() const
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    if (!HealthComponent)
        return 0.0f;

    return HealthComponent->GetHealthPercent();
}

// �� ����, ��� �������-������ ��� ����������� �������� � WeaponComponent, ����� �� ������ ������ �������������� � BluePrint
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

    return HealthComponent && !HealthComponent->IsDead(); // ������, ����������� ����� �� ������� ��������� HealthComponent
}

bool USTUPlayerHUDWidget::IsPlayerSpectating() const
{
    const auto Controller = GetOwningPlayer();

    return Controller && Controller->GetStateName() == NAME_Spectating;
}

int32 USTUPlayerHUDWidget::GetKillsNum() const
{
    const auto Controller = GetOwningPlayer();
    if (!Controller)
        return 0;

    const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
    return PlayerState ? PlayerState->GetKillsNum() : 0;
}

void USTUPlayerHUDWidget::UpdateHealthBar()
{
    if (HealthProgressBar)
    {
        HealthProgressBar->SetFillColorAndOpacity(GetHealthPercent() > PercentColorThreshold ? GoodColor : BadColor);
    }
}

FString USTUPlayerHUDWidget::FormatBullets(int32 BulletsNum) const
{
    const int32 MaxLen = 3;
    const TCHAR PrefixSymbol = '0';

    auto BulletStr = FString::FromInt(BulletsNum);
    const auto SymbolsNumToAdd = MaxLen - BulletStr.Len();

    if (SymbolsNumToAdd)
    {
        BulletStr = FString::ChrN(SymbolsNumToAdd, PrefixSymbol).Append(BulletStr); // ChrN ��������� ��������� ��������, Append ��������� ��� �����
    }

    return BulletStr;
}