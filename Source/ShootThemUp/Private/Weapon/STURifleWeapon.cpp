// Shoot Them Up Game. All Rights Reserved

#include "Weapon/STURifleWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Weapon/Components/STUWeaponFXComponent.h"

ASTURifleWeapon::ASTURifleWeapon()
{
    WeaponFXComponent = CreateDefaultSubobject<USTUWeaponFXComponent>("WeaponFXComponent");
}

void ASTURifleWeapon::BeginPlay()
{
    Super::BeginPlay();
    check(WeaponFXComponent);
}

void ASTURifleWeapon::StartFire()
{
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTURifleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
}

void ASTURifleWeapon::StopFire()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
}

void ASTURifleWeapon::MakeShot()
{

    if (!GetWorld() || IsAmmoEmpty())
    {
        StopFire();
        return;
    }

    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        StopFire();
        return;
    }

    const auto Controller = Player->GetController<APlayerController>();
    if (!Controller)
    {
        StopFire();
        return;
    }

    FVector ViewLocation;
    FRotator ViewRotation;
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

    const FTransform SocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);

    const FVector TraceStart = ViewLocation;
    const auto HalfRad = FMath::DegreesToRadians(BulletSpread);                      //�������� ���� �������� ����
    const FVector ShootDirection = FMath::VRandCone(ViewRotation.Vector(), HalfRad); // VRandCone ����� ��������� �������� ������� ���� ��� ��������
    const FVector TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;

    // �������� ���������� � ������ �������, ������� ��� ���������� �� ����� ����� ����
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner());    // ��� ������� �������� ���������� ������
    CollisionParams.bReturnPhysicalMaterial = true; // ������� ���������� ��������, � ������� ������
    FHitResult HitResult;                           // ��� �� ��� ������� ���������� bool � ���, ���� �� �����������
    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);

    if (HitResult.bBlockingHit) // ���� ���� �����������:
    {
        MakeDamage(HitResult);
        /*DrawDebugLine(GetWorld(), SocketTransform.GetLocation(), HitResult.ImpactPoint, FColor::Red, false, 3.0f, 0, 3.0f);
        DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 24, FColor::Red, false, 5.0f);*/
        WeaponFXComponent->PlayImpactFX(HitResult);
    }
    else
    {
        DrawDebugLine(GetWorld(), SocketTransform.GetLocation(), TraceEnd, FColor::Red, false, 3.0f, 0, 3.0f);
    }

    DecreaseAmmo();
}

void ASTURifleWeapon::MakeDamage(const FHitResult& HitResult)
{
    const auto DamagedActor = HitResult.GetActor();
    if (!DamagedActor)
        return;

    // ���� �� � ��������� ������� �������, �.�. �����������
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
        return;

    const auto Controller = Player->GetController<APlayerController>();
    if (!Controller)
        return;
    //

    DamagedActor->TakeDamage(DamageAmount, FDamageEvent(), Controller, this);
}