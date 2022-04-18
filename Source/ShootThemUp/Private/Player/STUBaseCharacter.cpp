// Shoot Them Up Game. All Rights Reserved

#include "Player/STUBaseCharacter.h"
#include "Components/STUCharacterMovementComponent.h"
#include "Components/STUHealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(BaseCharacterLog, All, All);

// Sets default values
// ��� ���� ����� ���� ������� ����� ��� CharacterMovementComponent
// �� ������������� ����������� ����� ������������ � ����������
// � ������� ��������� ������ �������������. ������ ������ ����� ���
// FObjectInitializer. �� ������ ������� ��� �� ����������� ������������� ������,
// ��� ����� ���������� Super. ACharacter::CharacterMovementComponentName - ��� ������ ��� ����������
ASTUBaseCharacter::ASTUBaseCharacter(const FObjectInitializer& ObjInit) : Super(ObjInit.SetDefaultSubobjectClass<USTUCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    HealthComponent = CreateDefaultSubobject<USTUHealthComponent>("HealthComponent");
    WeaponComponent = CreateDefaultSubobject<USTUWeaponComponent>("WeaponComponent");
}

// Called when the game starts or when spawned
void ASTUBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    check(HealthComponent);
    check(GetCharacterMovement());

    // ��������� BaseCharacter ����������� ����� �������, �� �������� � ���, ��� ������ 100 �� ��������� ����
    // �� ����� � ����� ������ �������� ���������� ������ � ����� ����:
    OnHealthChanged(HealthComponent->GetHealth(), 0.0f);
    // ������������� �� �������, ���������� � ������ ���������
    HealthComponent->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    // ������������� �� �������, ���������� � ��������� ������ (������� � ����� ������ ����� �� Tick)
    HealthComponent->OnHealthChanged.AddUObject(this, &ASTUBaseCharacter::OnHealthChanged);

    // ������������� �� ������� �����������, ����� ������������ ����������� ������� � ������:
    LandedDelegate.AddDynamic(this, &ASTUBaseCharacter::OnGroundLanded);
}

void ASTUBaseCharacter::OnHealthChanged(float Health, float HealthDelta) {}

// Called every frame
void ASTUBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// ���� ��� ����� ��� �������� ����:
bool ASTUBaseCharacter::IsRunning() const
{
    return false;
}

float ASTUBaseCharacter::GetMovementDirection() const
{
    if (GetVelocity().IsZero())
        return 0.0f;
    // ��������� ������� ������� ��������
    const auto VelocityNormal = GetVelocity().GetSafeNormal();
    // ���������� ��� ���� ����� ������� �������� � �������� �������� (��������)
    // (��������� �� ���������� ������������ ���� ��������)
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
    // ��������� ��������� ������������ ����� ������� ���������
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);

    const auto Degrees = FMath::RadiansToDegrees(AngleBetween);

    // ��������� �������� (FMath::Sign ���������� ���� �� ����� (+1/-1/0))
    return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void ASTUBaseCharacter::OnDeath()
{
    UE_LOG(BaseCharacterLog, Display, TEXT("Player %s is dead"), *GetName());

    // ����������� �������� ������. �������� �� ���������� �������� � �������, ��� ��� ����� ��� �� ������.
    // PlayAnimMontage(DeathAnimMontage);

    // ����� ������ ��������� �������� ���������
    GetCharacterMovement()->DisableMovement();

    // ���������� ��������� ����� 5 ������ ����� ������
    SetLifeSpan(5.0f);

    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponComponent->StopFire();

    // �������� RagDoll �� ����� ������
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);
}

void ASTUBaseCharacter::OnGroundLanded(const FHitResult& Hit)
{
    // � �������, ������ ��� ��� ������� ���������� Z �������������
    const auto FallVelocityZ = -GetVelocity().Z;

    if (FallVelocityZ < LandedDamageVelocity.X)
        return;

    const auto FinalDamage = FMath::GetMappedRangeValueClamped(LandedDamageVelocity, LandedDamage, FallVelocityZ);
    TakeDamage(FinalDamage, FPointDamageEvent{}, nullptr, nullptr);
}

void ASTUBaseCharacter::SetPlayerColor(const FLinearColor& Color)
{
    const auto MaterialInst = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
    if (!MaterialInst)
        return;

    MaterialInst->SetVectorParameterValue(MaterialColorName, Color);
}