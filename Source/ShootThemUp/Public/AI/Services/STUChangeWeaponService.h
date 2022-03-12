// Shoot Them Up Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "STUChangeWeaponService.generated.h"

/**
 *
 */
UCLASS()
class SHOOTTHEMUP_API USTUChangeWeaponService : public UBTService
{
    GENERATED_BODY()

    USTUChangeWeaponService();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Probability = 0.5f; // вероятность смены оружия (по-умолчанию - 50%)

    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
