// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeUI.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Camera/CameraActor.h"
#include "LHM_Player.h"

void UEscapeUI::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (Button_Escape)
    {
        Button_Escape->OnClicked.AddDynamic(this, &UEscapeUI::OnExitButtonClicked);
    }

    ALHM_Player* PlayerCharacter = Cast<ALHM_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (PlayerCharacter)
    {
        OriginalCameraActor = Cast<ACameraActor>(GetWorld()->GetFirstPlayerController()->GetViewTarget());
    }
}

void UEscapeUI::OnExitButtonClicked()
{
    // ���� ī�޶�� ���ư���
    if (APlayerController* pc = UGameplayStatics::GetPlayerController(this, 0))
    {
        pc->SetViewTarget(OriginalCameraActor);
    }

    // ��ǲ ��带 ���� ���� ����
    if (APlayerController* pc = UGameplayStatics::GetPlayerController(this, 0))
    {
        FInputModeGameOnly InputMode;
        pc->SetInputMode(InputMode);
        pc->bShowMouseCursor = false;
    }

    // UI ����
    this->RemoveFromParent();
}

