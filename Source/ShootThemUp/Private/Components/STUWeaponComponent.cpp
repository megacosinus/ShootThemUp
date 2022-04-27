// Shoot Them Up Game. All Rights Reserved

#include "Components/STUWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"
#include "GameFramework/Character.h"                // нужен чтобы приаттачить оружие к персонажу
#include "Animations/STUEquipFinishedAnimNotify.h"  // нотифай об окончании анимации смены оружия
#include "Animations/STUReloadFinishedAnimNotify.h" // ну ты понял
#include "Animations/AnimUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All);

constexpr static int32 WeaponNum = 2;

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

    checkf(WeaponData.Num() == WeaponNum, TEXT("Our characte can hold only %i weapon items"), WeaponNum);

    CurrentWeaponIndex = 0;
    InitAnimations();
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

    for (auto OneWeaponData : WeaponData)
    {
        // GetWorld()->SpawnActor возвращает указатель, по этому сразу создаём переменную с ним
        auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(OneWeaponData.WeaponClass);
        if (!Weapon)
            continue;
        Weapon->OnClipEmpty.AddUObject(this, &USTUWeaponComponent::OnEmptyClip); // подписались на делегат
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
    if (WeaponIndex < 0 || WeaponIndex >= Weapons.Num())
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Invalid weapon index"));
        return;
    }

    ACharacter* Character = Cast<ACharacter>(GetOwner()); // нужно чтобы приаттачить оружие к персонажу
    if (!Character)
        return;

    // возвращаем текущую пушку за спину
    if (CurrentWeapon)
    {
        CurrentWeapon->StopFire();
        CurrentWeapon->Zoom(false);
        AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
    }

    // берём новую пушку в руки
    CurrentWeapon = Weapons[WeaponIndex];
    //меняем анимацию перезарядки:
    // CurrentReloadAnimMontage = WeaponData[WeaponIndex].ReloadAnimMontage;
    const auto CurrentWeaponData = WeaponData.FindByPredicate([&](const FWeaponData& Data) { return Data.WeaponClass == CurrentWeapon->GetClass(); }); // Предикат - это функциональный объект, который
                                                                                                                                                       // возвращает истинну или ложь Лекция 62, 5:40
    CurrentReloadAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;

    AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);
    EquipAnimInProgress = true;
    PlayAnimMontage(EquipAnimMontage); // переход в функцию анимации смены оружия
}

void USTUWeaponComponent::StartFire()
{
    if (!CanFire())
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
    if (!CanEquip())
        return;

    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num(); // берём по модулю длинны массива
    EquipWeapon(CurrentWeaponIndex);
}

void USTUWeaponComponent::PlayAnimMontage(UAnimMontage* Animation)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
        return;

    Character->PlayAnimMontage(Animation); // на null проверять не нужно, т.к. в теле функции PlayAnimMontage эта проверка уже есть
}

void USTUWeaponComponent::InitAnimations()
{
    auto EquipFinishedNotify = AnimUtils::FindNotifyByClass<USTUEquipFinishedAnimNotify>(EquipAnimMontage);
    if (EquipFinishedNotify)
    {
        // достаём наш делегат
        EquipFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnEquipFinished);
    }
    else
    {
        UE_LOG(LogWeaponComponent, Error, TEXT("Equip anim notify is forgotten to set"));
        checkNoEntry();
    }

    for (auto OneWeaponData : WeaponData)
    {
        auto ReloadFinishedNotify = AnimUtils::FindNotifyByClass<USTUReloadFinishedAnimNotify>(OneWeaponData.ReloadAnimMontage);
        if (!ReloadFinishedNotify)
        {
            UE_LOG(LogWeaponComponent, Error, TEXT("Reload anim notify is forgotten to set"));
            checkNoEntry();
        }
        // достаём наш делегат
        ReloadFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnReloadFinished);
    }
}

void USTUWeaponComponent::OnEquipFinished(USkeletalMeshComponent* MeshComponent)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || Character->GetMesh() != MeshComponent) // Сравниваем меш, который прилетел в нотифае с текущим мешем и если не совпадают, то выходим
        return;

    EquipAnimInProgress = false;
}

void USTUWeaponComponent::OnReloadFinished(USkeletalMeshComponent* MeshComponent)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || Character->GetMesh() != MeshComponent) // Сравниваем меш, который прилетел в нотифае с текущим мешем и если не совпадают, то выходим
        return;

    ReloadAnimInProgress = false;
}

bool USTUWeaponComponent::CanFire() const
{
    // указатель должен быть ненулевым и не должно быть анимации
    return CurrentWeapon && !EquipAnimInProgress && !ReloadAnimInProgress;
}

bool USTUWeaponComponent::CanEquip() const
{
    return !EquipAnimInProgress && !ReloadAnimInProgress;
}

bool USTUWeaponComponent::CanReload() const
{
    // указатель должен быть ненулевым и не должно быть анимации
    return CurrentWeapon            //
           && !EquipAnimInProgress  //
           && !ReloadAnimInProgress //
           && CurrentWeapon->CanReload();
}

void USTUWeaponComponent::Reload()
{
    ChangeClip();
}

void USTUWeaponComponent::OnEmptyClip(ASTUBaseWeapon* AmmoEmtyWeapon)
{
    if (!AmmoEmtyWeapon)
        return;

    // логика ниже нужна чтобы определить, к какому оружию мы взяли патроны и либо включить перезарядку с анимацией и прочими
    // свистоперделками, либо просто добавить патроны к тому оружию, что у нас в инвентаре
    if (CurrentWeapon == AmmoEmtyWeapon)
    {
        ChangeClip();
    }
    else // иначе ищем нужное оружие
    {
        for (const auto Weapon : Weapons)
        {
            if (Weapon == AmmoEmtyWeapon)
            {
                Weapon->ChangeClip();
            }
        }
        // в принципе, можно просто AmmoEmptyWeapon->ChangeClip()..
    }
}

void USTUWeaponComponent::ChangeClip()
{
    if (!CanReload())
        return;
    CurrentWeapon->StopFire();
    CurrentWeapon->ChangeClip();
    ReloadAnimInProgress = true;
    PlayAnimMontage(CurrentReloadAnimMontage);
}

bool USTUWeaponComponent::GetCurrentWeaponUIData(FWeaponUIData& UIData) const
{
    if (CurrentWeapon)
    {
        UIData = CurrentWeapon->GetUIData();
        return true;
    }
    return false;
}

bool USTUWeaponComponent::GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const
{
    if (CurrentWeapon)
    {
        AmmoData = CurrentWeapon->GetAmmoData();
        return true;
    }
    return false;
}

bool USTUWeaponComponent::TryToAddAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType, int32 ClipsAmount)
{
    // пройдёмся по всем типам оружия и определим, существует ли нужный
    for (const auto Weapon : Weapons)
    {
        if (Weapon && Weapon->IsA(WeaponType))
        {
            return Weapon->TryToAddAmmo(ClipsAmount);
        }
    }
    return false;
}

bool USTUWeaponComponent::NeedAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType)
{
    // пройдёмся по всем типам оружия и определим, существует ли нужный
    for (const auto Weapon : Weapons)
    {
        if (Weapon && Weapon->IsA(WeaponType))
        {
            return !Weapon->IsAmmoFull();
        }
    }
    return false;
}

void USTUWeaponComponent::Zoom(bool Enabled)
{
    if (CurrentWeapon)
    {
        CurrentWeapon->Zoom(Enabled);
    }
}
