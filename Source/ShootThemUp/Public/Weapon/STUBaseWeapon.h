// Shoot Them Up Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUBaseWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class SHOOTTHEMUP_API ASTUBaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASTUBaseWeapon();

    virtual void StartFire();
    virtual void StopFire();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* WeaponMesh;

    // ���������� ��� �������� �������� ������ �� �������� ����� �������� ���� (����� ��������� �� ���� ������)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FName MuzzleSocketName = "MuzzleSocket";

    // ��������� ��������
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float TraceMaxDistance = 1500.0f;

    virtual void MakeShot();
};
