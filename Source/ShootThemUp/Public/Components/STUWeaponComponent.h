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

    // ��� �� ������� ������� ������ � ���������� �������� ��� ������ ���������
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    // ������ ���������� ���������� � ������� �������, ������ ���� ������, � cpp �����, � SpawnWeapon(), �� ���������� ������
    UPROPERTY()
    ASTUBaseWeapon* CurrentWeapon = nullptr;

    // ������ � �������
    UPROPERTY()
    TArray<ASTUBaseWeapon*> Weapons;

    UPROPERTY()
    UAnimMontage* CurrentReloadAnimMontage = nullptr;

    // ������ �������� ������
    int32 CurrentWeaponIndex = 0;

    bool EquipAnimInProgress = false;
    bool ReloadAnimInProgress = false;

    void SpawnWeapons();
    void AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);
    void EquipWeapon(int32 WeaponIndex);

    void PlayAnimMontage(UAnimMontage* Animation);

    void InitAnimations(); // ������� � ������������� �� AnimNotify (����� ����� �����, ����� ����������� �������� ����� �����)
    void OnEquipFinished(USkeletalMeshComponent* MeshComponent);  // ������ ��� �������� AnimNotify
    void OnReloadFinished(USkeletalMeshComponent* MeshComponent); // ������ ��� �������� AnimNotify

    bool CanFire() const;
    bool CanEquip() const;
    bool CanReload() const;

    // ������ �� ������� OnEmptyClip (�� STUBaseWeapon)
    void OnEmptyClip();
    void ChangeClip();
};
