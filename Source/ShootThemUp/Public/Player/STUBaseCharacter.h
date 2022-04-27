// Shoot Them Up Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "STUBaseCharacter.generated.h"

class USTUHealthComponent;
class USTUWeaponComponent;
class USoundCue;

UCLASS()
class SHOOTTHEMUP_API ASTUBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    // для того чтобы поменять класс CharacterMovementComponent на USTUCharacterMovementComponent
    // мы воспользуемся специальным видом конструктора с параметром
    // в котором передаётся объект инициализации. Данный объект имеет вид
    // FObjectInitializer:
    ASTUBaseCharacter(const FObjectInitializer& ObjInit);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USTUHealthComponent* HealthComponent; // весь код жизней в STUHealthComponent.h

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USTUWeaponComponent* WeaponComponent;

    // подключаем анимацию смерти
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* DeathAnimMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float LifeSpanOnDeath = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FVector2D LandedDamageVelocity = FVector2D(900.0f, 1200.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FVector2D LandedDamage = FVector2D(10.0f, 100.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Material")
    FName MaterialColorName = "Paint Color";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    USoundCue* DeathSound;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void OnDeath(); // виртуальная чтобы можно было переопределить в AICharacter
    virtual void OnHealthChanged(float Health, float HealthDelta);

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    virtual void TurnOff() override;
    virtual void Reset() override;

    // функция, которая возвращает, бежит ли персонаж (нужно для анимации бега
    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual bool IsRunning() const;

    // функция, которая будет возвращать значения угла для правильной обработки анимации движения в стороны
    UFUNCTION(BlueprintCallable, Category = "Movement")
    float GetMovementDirection() const;

    void SetPlayerColor(const FLinearColor& Color);

private:
    UFUNCTION()
    void OnGroundLanded(const FHitResult& Hit);
};
