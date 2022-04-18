// Shoot Them Up Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "STUCoreTypes.h"
#include "STUPlayerController.generated.h"

class USTURespawnComponent;

UCLASS()
class SHOOTTHEMUP_API ASTUPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASTUPlayerController();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USTURespawnComponent* RespawnComponent;

    virtual void BeginPlay() override;
    // фикс бага унрила (возможно, уже не нужен)
    virtual void OnPossess(APawn* InPawn) override;
    virtual void SetupInputComponent() override; // сама функция в лежит в APlayerController

private:
    void OnPauseGame();
    void OnMatchStateChanged(ESTUMatchState State);
};
