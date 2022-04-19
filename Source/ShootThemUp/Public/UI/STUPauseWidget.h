// Shoot Them Up Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STUPauseWidget.generated.h"

class UButton;

UCLASS()
class SHOOTTHEMUP_API USTUPauseWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget)) // мета информации для ассоциации кнопки, которая в BP с кнопкой в С++ файле
    UButton* ClearPauseButton;

    virtual void NativeOnInitialized() override;

private:
    UFUNCTION()
    void OnClearPause();
};
