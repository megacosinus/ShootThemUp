// Shoot Them Up Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUCoreTypes.h"
#include "STUBaseWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class SHOOTTHEMUP_API ASTUBaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASTUBaseWeapon();

    FOnClipEmptySignature OnClipEmpty;

    virtual void StartFire();
    virtual void StopFire();

    void ChangeClip();
    bool CanReload() const; // определяет, может ли данное оружие делать перезарядку впринципе

    FWeaponUIData GetUIData() const { return UIData; }
    FAmmoData GetAmmoData() const { return CurrentAmmo; }

    bool TryToAddAmmo(int32 ClipsAmount); // для пикапов

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* WeaponMesh;

    // переменная для указания названия сокета из которого будут вылетать пули (сокет находится на меше оружия)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FName MuzzleSocketName = "MuzzleSocket";

    // дистанция выстрела
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float TraceMaxDistance = 1500.0f;

    // создаём структуру с патронами (она в начале этого файла)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FAmmoData DefaultAmmo{15, 10, false}; // начальный арсенал оружия

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FWeaponUIData UIData;

    virtual void MakeShot();

    void DecreaseAmmo(); // функция, которая будет уменьшать количество патронов
    bool IsAmmoEmpty() const;
    bool IsClipEmpty() const;
    bool IsAmmoFull() const;

    void LogAmmo();

private:
    FAmmoData CurrentAmmo; // текущий арсенал оружия
};
