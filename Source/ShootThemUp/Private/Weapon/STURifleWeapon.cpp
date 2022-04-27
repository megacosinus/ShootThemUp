// Shoot Them Up Game. All Rights Reserved

#include "Weapon/STURifleWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Weapon/Components/STUWeaponFXComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRifleWeapon, All, All);

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
    InitFX(); // �������������� � �������� ������� �� ���������
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTURifleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
}

void ASTURifleWeapon::StopFire()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
    SetFXActive(false); // ��������� ������� �� ���������
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

    FVector ViewLocation;
    FRotator ViewRotation;
    FVector MuzzleLocation = (WeaponMesh->GetSocketTransform(MuzzleSocketName)).GetLocation();

    if (Player->IsPlayerControlled())
    {
        const auto Controller = Player->GetController<APlayerController>();
        if (!Controller)
        {
            StopFire();
            return;
        }
        Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    }
    else
    {
        ViewLocation = MuzzleLocation;
        ViewRotation = WeaponMesh->GetSocketRotation(MuzzleSocketName);
    }

    const FVector TraceStart = ViewLocation;
    const auto HalfRad = FMath::DegreesToRadians(BulletSpread);                      //�������� ���� �������� ����
    const FVector ShootDirection = FMath::VRandCone(ViewRotation.Vector(), HalfRad); // VRandCone ����� ��������� �������� ������� ���� ��� ��������
    const FVector TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;

    // �������� ���������� � ������ �������, ������� ��� ���������� �� ����� ����� ����
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner());    // ��� ������� �������� ���������� ������
    CollisionParams.bReturnPhysicalMaterial = true; // ������� ���������� ��������, � ������� ������
    FHitResult HitResult;                           // ��� �� ��� ������� ���������� bool � ���, ���� �� �����������
    GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);

    FVector TraceFXEnd = TraceEnd;

    if (HitResult.bBlockingHit) // ���� ���� �����������:
    {
        TraceFXEnd = HitResult.ImpactPoint;
        MakeDamage(HitResult);
        WeaponFXComponent->PlayImpactFX(HitResult);
    }
    SpawnTraceFX(MuzzleLocation, TraceFXEnd);

    DecreaseAmmo();
}

void ASTURifleWeapon::MakeDamage(const FHitResult& HitResult)
{
    const auto DamagedActor = HitResult.GetActor();
    if (!DamagedActor)
        return;

    const auto Pawn = Cast<APawn>(GetOwner());
    if (!Pawn)
        return;

    const auto Controller = Pawn->GetController();

    FPointDamageEvent PointDamageEvent;
    PointDamageEvent.HitInfo = HitResult;

    DamagedActor->TakeDamage(DamageAmount, PointDamageEvent, Controller, this);
}

void ASTURifleWeapon::InitFX()
{
    // ���� ������ �� ����, �� ������� ���� �������
    if (!MuzzleFXComponent)
    {
        MuzzleFXComponent = SpawnMuzzleFX();
    }

    if (!FireAudioComponent)
    {
        FireAudioComponent = UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, MuzzleSocketName); // ��� ��� ������ �����������, �� ��
                                                                                                            // ��������� �� ���������������!
    }

    SetFXActive(true);
}

void ASTURifleWeapon::SetFXActive(bool IsActive)
{
    if (MuzzleFXComponent)
    {
        MuzzleFXComponent->SetPaused(!IsActive);
        MuzzleFXComponent->SetVisibility(IsActive, true);
    }

    if (FireAudioComponent)
    {
        FireAudioComponent->SetPaused(!IsActive);
    }
}

void ASTURifleWeapon::SpawnTraceFX(const FVector& TraceStart, const FVector& TraceEnd)
{
    const auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, TraceStart);
    if (TraceFXComponent)
    {
        TraceFXComponent->SetNiagaraVariableVec3(TraceTargetName, TraceEnd); // ������� � ����������, ��������� � ������� (TraceTarget) ���������� �����
    }
}

void ASTURifleWeapon::Zoom(bool Enabled)
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
        return;

    const auto Controller = Player->GetController<APlayerController>();
    if (!Controller)
        return;

    if (!Controller || !Controller->PlayerCameraManager)
        return;

    if (Enabled)
    {
        DefaultCameraFOV = Controller->PlayerCameraManager->GetFOVAngle();
    }

    /*const auto Controller = Cast<APlayerController>(GetController());
     */

    Controller->PlayerCameraManager->SetFOV(Enabled ? FOVZoomAngle : DefaultCameraFOV);
}
