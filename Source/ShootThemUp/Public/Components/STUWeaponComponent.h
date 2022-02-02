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

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<ASTUBaseWeapon> WeaponClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponAttachPointName = "WeaponSocket";

    // Called when the game starts
    virtual void BeginPlay() override;

private:
    // делаем глобальную переменную с текущим оружием, ставим пока пустой, в cpp файле, в SpawnWeapon(), ей присвоится оружие
    UPROPERTY()
    ASTUBaseWeapon* CurrentWeapon = nullptr;

    void SpawnWeapon();
};
