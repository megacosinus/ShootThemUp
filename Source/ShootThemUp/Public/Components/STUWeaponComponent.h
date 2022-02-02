// Shoot Them Up Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUWeaponComponent.generated.h"

class ASTUBaseWeapon;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    USTUWeaponComponent();

    void StartFire();
    void StopFire();
    void NextWeapon();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TArray<TSubclassOf<ASTUBaseWeapon>> WeaponClasses;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponEquipSocketName = "WeaponSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponArmorySocketName = "ArmorySocket";

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

    // ������ �������� ������
    int32 CurrentWeaponIndex = 0;

    void SpawnWeapons();
    void AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);
    void EquipWeapon(int32 WeaponIndex);
};
