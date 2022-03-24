// Shoot Them Up Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_PickupCouldBeTaken.generated.h"

/**
 *
 */
UCLASS()
class SHOOTTHEMUP_API UEnvQueryTest_PickupCouldBeTaken : public UEnvQueryTest
{
    GENERATED_BODY()

    UEnvQueryTest_PickupCouldBeTaken(const FObjectInitializer& ObjectInitializer);

    /** Function that does the actual work */
    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};
