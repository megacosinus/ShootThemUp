// Shoot Them Up Game. All Rights Reserved

#include "Components/STUCharacterMovementComponent.h"
#include "Player/STUBaseCharacter.h"

float USTUCharacterMovementComponent::GetMaxSpeed() const
{

    const float MaxSpeed = Super::GetMaxSpeed();

    // ������ ��������� �� ������ ���������, �������� Player, ����� ������ ����,
    // �.�. ��������� ������ ��������� ������ �� ����� ����� � ��� ����� �������������
    // ������ ��������� � ������ ���������:
    const ASTUBaseCharacter* Player = Cast<ASTUBaseCharacter>(GetPawnOwner());

    // ���� ���� Player � �� �����, �� ���� �������� �������� �� �����������, ����� ������ ���� ��������:
    return Player && Player->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}