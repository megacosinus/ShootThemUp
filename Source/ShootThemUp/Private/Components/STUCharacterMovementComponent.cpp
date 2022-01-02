// Shoot Them Up Game. All Rights Reserved

#include "Components/STUCharacterMovementComponent.h"
#include "Player/STUBaseCharacter.h"

float USTUCharacterMovementComponent::GetMaxSpeed() const
{

    const float MaxSpeed = Super::GetMaxSpeed();

    // создаём указатель на нашего чарактера, называем Player, сразу создаём каст,
    // т.к. компонент хранит указатель только на класс Пауна и нам нужно преобразовать
    // данный указатель к нашему чарактеру:
    const ASTUBaseCharacter* Player = Cast<ASTUBaseCharacter>(GetPawnOwner());

    // если есть Player и он бежит, то макс скорость умножаем на модификатор, иначе просто макс скорость:
    return Player && Player->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}