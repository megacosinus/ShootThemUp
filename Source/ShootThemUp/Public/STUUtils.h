#pragma once

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
};