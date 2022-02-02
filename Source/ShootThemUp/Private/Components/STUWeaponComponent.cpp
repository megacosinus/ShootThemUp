// Shoot Them Up Game. All Rights Reserved

#include "Components/STUWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"
#include "GameFramework/Character.h" // нужен чтобы приаттачить оружие к персонажу

// Sets default values for this component's properties
USTUWeaponComponent::USTUWeaponComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;

    // ...
}

// Called when the game starts
void USTUWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentWeaponIndex = 0;

    SpawnWeapons();
    EquipWeapon(CurrentWeaponIndex);
}

void USTUWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CurrentWeapon = nullptr;
    for (auto Weapon : Weapons)
    {
        Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        Weapon->Destroy();
    }

    // очищаем массив с оружием:
    Weapons.Empty();

    Super::EndPlay(EndPlayReason);
}

void USTUWeaponComponent::SpawnWeapons()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner()); // нужно чтобы приаттачить оружие к персонажу
    if (!Character || !GetWorld())
        return;

    for (auto WeaponClass : WeaponClasses)
    {
        // GetWorld()->SpawnActor возвращает указатель, по этому сразу создаём переменную с ним
        auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(WeaponClass);
        if (!Weapon)
            continue;
        Weapon->SetOwner(Character);
        Weapons.Add(Weapon);

        AttachWeaponToSocket(Weapon, Character->GetMesh(), WeaponArmorySocketName);
    }
}

void USTUWeaponComponent::AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName)
{
    if (!Weapon || !SceneComponent)
        return;
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

void USTUWeaponComponent::EquipWeapon(int32 WeaponIndex)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner()); // нужно чтобы приаттачить оружие к персонажу
    if (!Character)
        return;

    // возвращаем текущую пушку за спину
    if (CurrentWeapon)
    {
        CurrentWeapon->StopFire();
        AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
    }

    // берём новую пушку в руки
    CurrentWeapon = Weapons[WeaponIndex];
    AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);
}

void USTUWeaponComponent::StartFire()
{
    if (!CurrentWeapon)
        return;

    CurrentWeapon->StartFire();
}

void USTUWeaponComponent::StopFire()
{
    if (!CurrentWeapon)
        return;

    CurrentWeapon->StopFire();
}

void USTUWeaponComponent::NextWeapon()
{
    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num(); // берём по модулю длинны массива
    EquipWeapon(CurrentWeaponIndex);
}
