// Shoot Them Up Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUCoreTypes.h"
#include "STUWeaponComponent.generated.h"

class ASTUBaseWeapon;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent)) class SHOOTTHEMUP_API USTUWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    USTUWeaponComponent();

    virtual void StartFire();
    void StopFire();
    virtual void NextWeapon();
    void Reload();

    // функции для получения информации о текущих иконках и картинках оружия для вывода на экран через STUPlayerHUD (вызывются оттуда)
    bool GetCurrentWeaponUIData(FWeaponUIData& UIData) const;
    bool GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const;

    bool TryToAddAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType, int32 ClipsAmount); // функция для пополнения магазина. Например от пикапа
    bool NeedAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType);                        // функция для использования в AI

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TArray<FWeaponData> WeaponData;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponEquipSocketName = "WeaponSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponArmorySocketName = "ArmorySocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    UAnimMontage* EquipAnimMontage;

    // делаем глобальную переменную с текущим оружием, ставим пока пустой, в cpp файле, в SpawnWeapon(), ей присвоится оружие
    UPROPERTY()
    ASTUBaseWeapon* CurrentWeapon = nullptr;

    // массив с оружием
    UPROPERTY()
    TArray<ASTUBaseWeapon*> Weapons;

    // индекс текущего оружия
    int32 CurrentWeaponIndex = 0;

    // Called when the game starts
    virtual void BeginPlay() override;

    // Тут мы убираем убираем оружие и прекращаем стрельбу при смерте персонажа
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    bool CanFire() const;
    bool CanEquip() const;

    void EquipWeapon(int32 WeaponIndex);

private:
    UPROPERTY()
    UAnimMontage* CurrentReloadAnimMontage = nullptr;

    bool EquipAnimInProgress = false;
    bool ReloadAnimInProgress = false;

    void SpawnWeapons();
    void AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);

    void PlayAnimMontage(UAnimMontage* Animation);

    void InitAnimations(); // находим и подписываемся на AnimNotify (нужно чтобы знать, когда закончилась анимация смены пушки)
    void OnEquipFinished(USkeletalMeshComponent* MeshComponent);  // колбэк для делегата AnimNotify
    void OnReloadFinished(USkeletalMeshComponent* MeshComponent); // колбэк для делегата AnimNotify

    bool CanReload() const;

    // колбэк на делегат OnEmptyClip (из STUBaseWeapon)
    void OnEmptyClip(ASTUBaseWeapon* AmmoEmtyWeapon);
    void ChangeClip();
};
