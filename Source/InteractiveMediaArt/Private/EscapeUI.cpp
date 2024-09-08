// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeUI.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Camera/CameraActor.h"
#include "LHM_Player.h"
#include "CJS/CJS_LobbyPlayer.h"

void UEscapeUI::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (Button_Escape)
    {
        Button_Escape->OnClicked.AddDynamic(this, &UEscapeUI::OnExitButtonClicked);
    }

    //ACJS_LobbyPlayer* PlayerCharacter = Cast<ACJS_LobbyPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
    //if (PlayerCharacter)
    //{
    //    OriginalCameraActor = Cast<ACameraActor>(GetWorld()->GetFirstPlayerController()->GetViewTarget());
    //}
}

void UEscapeUI::OnExitButtonClicked()
{
    if (Me->bExitBnt2_1)
    {
        UE_LOG(LogTemp, Warning, TEXT("UEscapeUI::OnExitButtonClicked()::Me->bExitBnt2_1 "));
        Me->OnExitBnt();
    }
    else
    {
        Me->ExitArt();
    }
}

