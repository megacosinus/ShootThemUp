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
// для того чтобы явно указать класс для CharacterMovementComponent
// мы воспользуемся специальным видом конструктора с параметром
// в котором передаётся объект инициализации. Данный объект имеет вид
// FObjectInitializer. Мы должны вызвать так же конструктор родительского класса,
// для этого используем Super. ACharacter::CharacterMovementComponentName - это просто имя компонента
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

    // поскольку BaseCharacter запускается после модулей, то бродкаст с тем, что жизней 100 не прилетает сюда
    // по этому в самом начале запросим количество жизней в явном виде:
    OnHealthChanged(HealthComponent->GetHealth(), 0.0f);
    // подписываемся на делегат, извещающий о смерти персонажа
    HealthComponent->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    // подписываемся на делегат, извещающий о изменении жизней (сделано в чтобы убрать опрос из Tick)
    HealthComponent->OnHealthChanged.AddUObject(this, &ASTUBaseCharacter::OnHealthChanged);

    // подписываемся на делегат приземления, чтобы отрабатывать повреждения падения с высоты:
    LandedDelegate.AddDynamic(this, &ASTUBaseCharacter::OnGroundLanded);
}

void ASTUBaseCharacter::OnHealthChanged(float Health, float HealthDelta) {}

// Called every frame
void ASTUBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// этот код нужен для анимации бега:
bool ASTUBaseCharacter::IsRunning() const
{
    return false;
}

float ASTUBaseCharacter::GetMovementDirection() const
{
    if (GetVelocity().IsZero())
        return 0.0f;
    // вычисляем нормаль вектора движения
    const auto VelocityNormal = GetVelocity().GetSafeNormal();
    // переменная для угла между форвард вектором и вектором скорости (движения)
    // (аркосинус от скалярного произведения двух векторов)
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
    // вычисляем векторное произведение между данными векторами
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);

    const auto Degrees = FMath::RadiansToDegrees(AngleBetween);

    // финальное значение (FMath::Sign возвращает знак от числа (+1/-1/0))
    return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void ASTUBaseCharacter::OnDeath()
{
    UE_LOG(BaseCharacterLog, Display, TEXT("Player %s is dead"), *GetName());

    // проигрываем анимацию смерти. Проверка на валидность встроена в функцию, так что можно тут не делать.
    // PlayAnimMontage(DeathAnimMontage);

    // после смерти запрещаем движение персонажа
    GetCharacterMovement()->DisableMovement();

    // уничтожаем персонажа через 5 секунд после смерти
    SetLifeSpan(5.0f);

    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponComponent->StopFire();

    // включаем RagDoll во время смерти
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);
}

void ASTUBaseCharacter::OnGroundLanded(const FHitResult& Hit)
{
    // с минусом, потому что при падении координата Z отрицательная
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