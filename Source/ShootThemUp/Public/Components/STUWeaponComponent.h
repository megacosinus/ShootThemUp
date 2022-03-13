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

    // ������� ��� ��������� ���������� � ������� ������� � ��������� ������ ��� ������ �� ����� ����� STUPlayerHUD (��������� ������)
    bool GetCurrentWeaponUIData(FWeaponUIData& UIData) const;
    bool GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const;

    bool TryToAddAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType, int32 ClipsAmount); // ������� ��� ���������� ��������. �������� �� ������
    bool NeedAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType);                        // ������� ��� ������������� � AI

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TArray<FWeaponData> WeaponData;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponEquipSocketName = "WeaponSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponArmorySocketName = "ArmorySocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    UAnimMontage* EquipAnimMontage;

    // ������ ���������� ���������� � ������� �������, ������ ���� ������, � cpp �����, � SpawnWeapon(), �� ���������� ������
    UPROPERTY()
    ASTUBaseWeapon* CurrentWeapon = nullptr;

    // ������ � �������
    UPROPERTY()
    TArray<ASTUBaseWeapon*> Weapons;

    // ������ �������� ������
    int32 CurrentWeaponIndex = 0;

    // Called when the game starts
    virtual void BeginPlay() override;

    // ��� �� ������� ������� ������ � ���������� �������� ��� ������ ���������
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

    void InitAnimations(); // ������� � ������������� �� AnimNotify (����� ����� �����, ����� ����������� �������� ����� �����)
    void OnEquipFinished(USkeletalMeshComponent* MeshComponent);  // ������ ��� �������� AnimNotify
    void OnReloadFinished(USkeletalMeshComponent* MeshComponent); // ������ ��� �������� AnimNotify

    bool CanReload() const;

    // ������ �� ������� OnEmptyClip (�� STUBaseWeapon)
    void OnEmptyClip(ASTUBaseWeapon* AmmoEmtyWeapon);
    void ChangeClip();
};
