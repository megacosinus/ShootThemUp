// Shoot Them Up Game. All Rights Reserved

#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STUProjectile.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"

void ASTULauncherWeapon::StartFire()
{
    MakeShot();
}

void ASTULauncherWeapon::MakeShot()
{
    if (!GetWorld())
        return;

    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
        return;

    const auto Controller = Player->GetController<APlayerController>();
    if (!Controller)
        return;

    FVector ViewLocation;
    FRotator ViewRotation;
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

    const FTransform SocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);

    const FVector TraceStart = ViewLocation;
    const FVector ShootDirection = ViewRotation.Vector(); // VRandCone задаёт рандомный конусный разброс пуль при стрельбе
    const FVector TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;

    // получаем информацию о первом объекте, который нам встретится на линии полёта пули
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner()); // при расчёте коллизий игнорируем игрока
    FHitResult HitResult;                        // так же эта функция возвращает bool о том, было ли пересечение
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
}