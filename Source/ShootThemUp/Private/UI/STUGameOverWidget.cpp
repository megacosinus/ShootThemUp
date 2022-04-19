// Shoot Them Up Game. All Rights Reserved

#include "STUGameOverWidget.h"
#include "STUGameModeBase.h"
#include "Player/STUPlayerState.h"
#include "UI/STUPlayerStatRowWidget.h"
#include "Components/VerticalBox.h"
#include "STUUtils.h"

bool USTUGameOverWidget::Initialize()
{
    if (GetWorld())
    {
        const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->OnMatchStateChanged.AddUObject(this, &USTUGameOverWidget::OnMatchStateChanged); // ������������� �� ������� � STUGameModeBase
        }
    }

    return Super::Initialize();
}

void USTUGameOverWidget::OnMatchStateChanged(ESTUMatchState State)
{
    if (State == ESTUMatchState::GameOver)
    {
        UpdatePlayerStat();
    }
}

void USTUGameOverWidget::UpdatePlayerStat()
{
    if (!GetWorld() || !PlayerStatBox)
        return;

    PlayerStatBox->ClearChildren(); // ������ �� �������, ��� �������� ���� ������ ����� ���, ��� ������� ���� ���� �������

    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        // �������� ��������� �� ����������
        const auto Controller = It->Get();
        if (!Controller)
            continue;

        // �����, � ������� ����������� �������� ��������� �� PlayerState
        const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        if (!PlayerState)
            continue;

        // ������ PlayerStatRowWidget
        const auto PlayerStatRowWidget = CreateWidget<USTUPlayerStatRowWidget>(GetWorld(), PlayerStatRowWidgetClass);
        if (!PlayerStatRowWidget)
            continue;

        PlayerStatRowWidget->SetPlayerName(FText::FromString(PlayerState->GetPlayerName()));
        PlayerStatRowWidget->SetKills(STUUtils::TextFromInt(PlayerState->GetKillsNum()));
        PlayerStatRowWidget->SetDeaths(STUUtils::TextFromInt(PlayerState->GetDeathsNum()));
        PlayerStatRowWidget->SetTeam(STUUtils::TextFromInt(PlayerState->GetTeamID()));
        PlayerStatRowWidget->SetPlayerIndicatorVisibility(Controller->IsPlayerController()); // �.�. ���� ���������� ����������������
                                                                                             // �� ��������� ����� ������������
        // �� ���������� ���� �������, ���� �������� �:
        PlayerStatBox->AddChild(PlayerStatRowWidget);
    }
}
