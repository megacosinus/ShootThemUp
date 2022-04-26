// Shoot Them Up Game. All Rights Reserved

#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STUProjectile.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void ASTULauncherWeapon::StartFire()
{
    MakeShot();
}

void ASTULauncherWeapon::MakeShot()
{
    if (!GetWorld())
        return;

    if (IsAmmoEmpty())
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), NoAmmoSound, GetActorLocation());
        return;
    }

    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
        return;

    FVector ViewLocation;
    FRotator ViewRotation;
    if (Player->IsPlayerControlled())
    {
        const auto Controller = Player->GetController<APlayerController>();
        if (!Controller)
            return;
        Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    }
    else
    {
        ViewLocation = (WeaponMesh->GetSocketTransform(MuzzleSocketName)).GetLocation();
        ViewRotation = WeaponMesh->GetSocketRotation(MuzzleSocketName);
    }
    const FTransform SocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);

    const FVector TraceStart = ViewLocation;
    const FVector ShootDirection = ViewRotation.Vector(); // VRandCone ����� ��������� �������� ������� ���� ��� ��������
    const FVector TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;

    // �������� ���������� � ������ �������, ������� ��� ���������� �� ����� ����� ����
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner());    // ��� ������� �������� ���������� ������
    CollisionParams.bReturnPhysicalMaterial = true; // ������� ���������� ��������, � ������� ������
    FHitResult HitResult;                           // ��� �� ��� ������� ���������� bool � ���, ���� �� �����������
    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);

    const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
    const FVector Direction = (EndPoint - SocketTransform.GetLocation()).GetSafeNormal();

    const FTransform SpawnTransform(FRotator::ZeroRotator, SocketTransform.GetLocation());
    ASTUProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASTUProjectile>(ProjectileCLass, SpawnTransform);
    if (Projectile)
    {
        Projectile->SetShotDirection(Direction);
        Projectile->SetOwner(GetOwner());
        Projectile->FinishSpawning(SpawnTransform);
    }
    DecreaseAmmo();
    SpawnMuzzleFX(); // �������
    UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, MuzzleSocketName);
}