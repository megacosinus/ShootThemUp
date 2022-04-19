#pragma once
#include "Player/STUPlayerState.h"

class STUUtils
{
public:
    template <typename T> static T* GetSTUPlayerComponent(AActor* PlayerPawn)
    {
        if (!PlayerPawn)
            return nullptr;

        const auto Component = PlayerPawn->GetComponentByClass(T::StaticClass()); // на null можем не проверять, т.к. это проверяется в cast'е

        // GetComponentByClass возвращает указатель на UActorComponent, по этому нам нужно привести его к USTUHealthComponent:
        return Cast<T>(Component);
    }

    bool static AreEnemies(AController* Controller1, AController* Controller2)
    {
        if (!Controller1 || !Controller2 || Controller1 == Controller2)
            return false;
        const auto PlayerState1 = Cast<ASTUPlayerState>(Controller1->PlayerState);
        const auto PlayerState2 = Cast<ASTUPlayerState>(Controller2->PlayerState);

        return PlayerState1 && PlayerState2 && PlayerState1->GetTeamID() != PlayerState2->GetTeamID();
    }

    static FText TextFromInt(int32 Number) { return FText::FromString(FString::FromInt(Number)); }
};