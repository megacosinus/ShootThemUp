// Shoot Them Up Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUWeaponComponent.generated.h"

class ASTUBaseWeapon;

// тут мы будем хранить класс оружия и анимацию перезарядки для данного оружия
USTRUCT(BlueprintType)
struct FWeaponData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ASTUBaseWeapon> WeaponClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UAnimMontage* ReloadAnimMontage;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent)) class SHOOTTHEMUP_API USTUWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    USTUWeaponComponent();

    void StartFire();
    void StopFire();
    void NextWeapon();
    void Reload();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TArray<FWeaponData> WeaponData;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponEquipSocketName = "WeaponSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponArmorySocketName = "ArmorySocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    UAnimMontage* EquipAnimMontage;

    // Called when the game starts
    virtual void BeginPlay() override;

    // Тут мы убираем убираем оружие и прекращаем стрельбу при смерте персонажа
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    // делаем глобальную переменную с текущим оружием, ставим пока пустой, в cpp файле, в SpawnWeapon(), ей присвоится оружие
    UPROPERTY()
    ASTUBaseWeapon* CurrentWeapon = nullptr;

    // массив с оружием
    UPROPERTY()
    TArray<ASTUBaseWeapon*> Weapons;

    UPROPERTY()
    UAnimMontage* CurrentReloadAnimMontage = nullptr;

    // индекс текущего оружия
    int32 CurrentWeaponIndex = 0;

    bool EquipAnimInProgress = false;
    bool ReloadAnimInProgress = false;

    void SpawnWeapons();
    void AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);
    void EquipWeapon(int32 WeaponIndex);

    void PlayAnimMontage(UAnimMontage* Animation);

    void InitAnimations(); // находим и подписываемся на AnimNotify (нужно чтобы знать, когда закончилась анимация смены пушки)
    void OnEquipFinished(USkeletalMeshComponent* MeshComponent);  // колбэк для делегата AnimNotify
    void OnReloadFinished(USkeletalMeshComponent* MeshComponent); // колбэк для делегата AnimNotify

    bool CanFire() const;
    bool CanEquip() const;
    bool CanReload() const;

    // колбэк на делегат OnEmptyClip (из STUBaseWeapon)
    void OnEmptyClip();
    void ChangeClip();

    // Шаблонная функция (вообще, название немного некорректное, т.к. она достаёт только первый нотифай):
    template <typename T> T* FindNotifyByClass(UAnimSequenceBase* Animation)
    {
        if (!Animation)
            return nullptr;
        // берём массив анимационных ивентов:
        const auto NotifyEvents = Animation->Notifies;
        for (auto NotifyEvent : NotifyEvents)
        {
            // чтобы узнать, является ли данный нотифай нашим нотифаем EquipFinished
            // мы попытаемся преобразовать данный нотифай к нашему типу:
            auto AnimNotify = Cast<T>(NotifyEvent.Notify);
            // если преобразование прошло успешно, то именно этот нотифай нам и нужен
            if (AnimNotify)
            {
                return AnimNotify;
            }
        }
        return nullptr;
    }
    // Конец шаблонной функции
};
