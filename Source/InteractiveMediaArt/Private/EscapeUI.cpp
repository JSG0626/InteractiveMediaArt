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

    if (ALHM_Player* pc = Cast<ALHM_Player>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        OriginalCameraActor = Cast<ACameraActor>(GetWorld()->GetFirstPlayerController()->GetViewTarget());
    }

    
}

void UEscapeUI::OnExitButtonClicked()
{
    // 원래 카메라로 돌아가기
    if (APlayerController* pc = UGameplayStatics::GetPlayerController(this, 0))
    {
        pc->SetViewTarget(OriginalCameraActor);
    }

    // 인풋 모드를 게임 모드로 변경
    if (APlayerController* pc = UGameplayStatics::GetPlayerController(this, 0))
    {
        FInputModeGameOnly InputMode;
        pc->SetInputMode(InputMode);
        pc->bShowMouseCursor = false;
    }

    // UI 제거
    RemoveFromViewport();
}

