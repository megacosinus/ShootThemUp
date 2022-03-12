// Shoot Them Up Game. All Rights Reserved

#include "Components/STUAIWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"

void USTUAIWeaponComponent::StartFire()
{
    if (!CanFire())
        return;
    if (CurrentWeapon->IsAmmoEmpty())
    {
        NextWeapon();
    }
    else
    {
        CurrentWeapon->StartFire();
    }
}

void USTUAIWeaponComponent::NextWeapon()
{
    if (!CanEquip())
        return;
    int32 NextIndex = (CurrentWeaponIndex + 1) % Weapons.Num();

    // чтобы не делать смену оружия, если пусто боекомплект:
    while (NextIndex != CurrentWeaponIndex) // если NextIndex == CurrentWeaponIndex, то это означает, что у нас нет оружия с боеприпасами
    {
        if (!Weapons[NextIndex]->IsAmmoEmpty())
            break;
        // если оружие пустое:
        NextIndex = (NextIndex + 1) % Weapons.Num();
    }
    if (CurrentWeaponIndex != NextIndex) // значит нашли какое-то другое оружие
    {
        CurrentWeaponIndex = NextIndex;
        EquipWeapon(CurrentWeaponIndex);
    }
}
