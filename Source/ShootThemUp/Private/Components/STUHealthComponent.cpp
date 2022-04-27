// Shoot Them Up Game. All Rights Reserved

#include "Components/STUHealthComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Camera/CameraShakeBase.h"
#include "STUGameModeBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Perception/AISense_Damage.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All);

// Sets default values for this component's properties
USTUHealthComponent::USTUHealthComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;

    // ...
}

// Called when the game starts
void USTUHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    check(MaxHealth > 0); // чтобы в некоторых местах кода не было деления на ноль

    SetHealth(MaxHealth);

    // подпишемся на получение урона:
    AActor* ComponentOwner = GetOwner();
    if (ComponentOwner)
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamage); // больше не нужна, т.к. обрабатываем в функциях ниже
        ComponentOwner->OnTakePointDamage.AddDynamic(this, &USTUHealthComponent::OnTakePointDamage);
        ComponentOwner->OnTakeRadialDamage.AddDynamic(this, &USTUHealthComponent::OnTakeRadialDamage);
    }
}

void USTUHealthComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
    FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
    const auto FinalDamage = Damage * GetPointDamageModifier(DamagedActor, BoneName);
    // UE_LOG(LogHealthComponent, Display, TEXT("On point damage: %f, final damage: %f, bone: %s"), Damage, FinalDamage, *BoneName.ToString());
    ApplyDamage(FinalDamage, InstigatedBy);
}

void USTUHealthComponent::OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
    // UE_LOG(LogHealthComponent, Display, TEXT("On radial damage: %f"), Damage);
    ApplyDamage(Damage, InstigatedBy);
}

void USTUHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    // UE_LOG(LogHealthComponent, Display, TEXT("On any damage: %f"), Damage);
}

void USTUHealthComponent::ApplyDamage(float Damage, AController* InstigatedBy)
{
    if (Damage <= 0.0f || IsDead() || !GetWorld())
        return;

    SetHealth(Health - Damage);

    // если шёл процесс автохила, то останавливаем его:
    GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);

    // если умер, то бродкастим делегат
    if (IsDead())
    {
        Killed(InstigatedBy);
        OnDeath.Broadcast();
    }
    else if (AutoHeal)
    {
        GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, this, &USTUHealthComponent::HealUpdate, HealUpdateTime, true, HealDelay);
    }

    PlayCameraShake();
    ReportDamageEvent(Damage, InstigatedBy);
}

void USTUHealthComponent::HealUpdate()
{
    SetHealth(Health + HealModifier);

    // ниже FMath::IsNearlyEqual используется из-за того что сравнивать числа с плавающей точкой чревато
    if (IsHealthFull() && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    }
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
    // код ниже нужен чтобы вычислить дельту изменения жизни, чтобы её можно было передавать с делегатом
    const auto NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    const auto HealthDelta = NextHealth - Health;

    Health = NextHealth;
    //поскольку тут присваиваем жизни, то нужно об этом объявить, потому броадкастим:
    OnHealthChanged.Broadcast(Health, HealthDelta);
}

bool USTUHealthComponent::TryToAddHealth(float HealthAmount)
{
    if (IsDead() || IsHealthFull())
        return false;

    SetHealth(Health + HealthAmount);
    return true;
}

bool USTUHealthComponent::IsHealthFull() const
{
    return FMath::IsNearlyEqual(Health, MaxHealth);
}

void USTUHealthComponent::PlayCameraShake()
{
    if (IsDead())
        return;

    const auto Player = Cast<APawn>(GetOwner());
    if (!Player)
        return;
    const auto Controller = Player->GetController<APlayerController>();
    if (!Controller || !Controller->PlayerCameraManager)
        return;

    Controller->PlayerCameraManager->StartCameraShake(CameraShake);
}

void USTUHealthComponent::Killed(AController* KillerController)
{
    if (!GetWorld())
        return;
    const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
        return;

    const auto Player = Cast<APawn>(GetOwner());
    const auto VictimController = Player ? Player->Controller : nullptr;

    GameMode->Killed(KillerController, VictimController);
}

float USTUHealthComponent::GetPointDamageModifier(AActor* DamagedActor, const FName& BoneName)
{
    const auto Character = Cast<ACharacter>(DamagedActor);
    if (!Character ||            //
        !Character->GetMesh() || //
        !Character->GetMesh()->GetBodyInstance(BoneName))
        return 1.0f; // при единице финальная дамага не изменится (домножение на 1)

    const auto PhysMaterial = Character->GetMesh()->GetBodyInstance(BoneName)->GetSimplePhysicalMaterial();
    if (!PhysMaterial || !DamageModifiers.Contains(PhysMaterial))
        return 1.0f;

    return DamageModifiers[PhysMaterial];
}

void USTUHealthComponent::ReportDamageEvent(float Damage, AController* InstigatedBy)
{
    if (!InstigatedBy || !InstigatedBy->GetPawn() || !GetOwner())
        return;

    UAISense_Damage::ReportDamageEvent(GetWorld(),   //
        GetOwner(),                                  //
        InstigatedBy->GetPawn(),                     //
        Damage,                                      //
        InstigatedBy->GetPawn()->GetActorLocation(), //
        GetOwner()->GetActorLocation());
    // внутри формируется спец. ивент и передаётся в персепшн систему.
    // Этот актор будет добавлен а мы сможем его получить
}
