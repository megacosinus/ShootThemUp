// Shoot Them Up Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Player/STUBaseCharacter.h"
#include "STUAICharacter.generated.h"

class UBehaviorTree;

UCLASS()
class SHOOTTHEMUP_API ASTUAICharacter : public ASTUBaseCharacter
{
    GENERATED_BODY()

public:
    ASTUAICharacter(const FObjectInitializer& ObjInit);

    // можно было бы сделать и в контроллере, но лучше здесь, т.к. контроллером может завладеть другой персонаж
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;
};