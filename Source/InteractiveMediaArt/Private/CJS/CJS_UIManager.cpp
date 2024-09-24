// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_UIManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TimerHandle.h"
#include "Components/Button.h"
#include "CJS/CJS_LobbyPlayer.h"


UCJS_UIManager::UCJS_UIManager() : StartPanelInstance(nullptr), EndPanelInstance(nullptr), QuitUIInstance(nullptr), WorldRef(nullptr)
{}

void UCJS_UIManager::Initialize(UWorld* World, TSubclassOf<UUserWidget> InStartPanelFactory, TSubclassOf<UUserWidget> InEndPanelFactory, TSubclassOf<UUserWidget> InQuitUIFactory, APlayerController* InPC)
//void UCJS_UIManager::Initialize(UWorld* World, TSubclassOf<UUserWidget> InStartPanelFactory, TSubclassOf<UUserWidget> InEndPanelFactory)
{
	WorldRef = World;
	StartPanelFactory = InStartPanelFactory;
	EndPanelFactory = InEndPanelFactory;
	QuitUIFactory = InQuitUIFactory;
	PC = InPC;

    if (!WorldRef || !PC)
    //if (!WorldRef)
    {
        UE_LOG(LogTemp, Error, TEXT("UCJS_UIManager::Initialize(): WorldRef or PC is null"));
        return;
    }

    // 시작 패널 인스턴스 생성
    if (StartPanelFactory)
    {
        StartPanelInstance = CreateWidget<UUserWidget>(PC, StartPanelFactory);
        if (StartPanelInstance)
        {
            StartPanelInstance->AddToViewport();
            HideStartPanel();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create StartPanelInstance"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("StartPanelFactory is not set!"));
    }

    // 종료 패널 인스턴스 생성
    if (EndPanelFactory)
    {
        EndPanelInstance = CreateWidget<UUserWidget>(PC, EndPanelFactory);
        if (EndPanelInstance)
        {
            EndPanelInstance->AddToViewport();
            HideEndPanel();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create EndPanelInstance"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("EndPanelFactory is not set!"));
    }

    // 종료 UI 인스턴스 생성
    if (QuitUIFactory)
    {
        QuitUIInstance = CreateWidget<UUserWidget>(PC, QuitUIFactory);
        if (QuitUIInstance)
        {
            //QuitUIInstance->AddToViewport();
            HideQuitUI();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create QuitUIInstance"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("QuitUIFactory is not set!"));
    }
}

void UCJS_UIManager::ShowStartPanel()
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::ShowStartPanel()"));
	if (StartPanelInstance && WorldRef)
	{
		StartPanelInstance->SetVisibility(ESlateVisibility::Visible);

		FTimerHandle HideTimerHandle;
		WorldRef->GetTimerManager().SetTimer(HideTimerHandle, this, &UCJS_UIManager::HideStartPanel, 10.f, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StartPanelInstance or WorldRef is null in ShowStartPanel()"));
	}
}
void UCJS_UIManager::ShowEndPanel()
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::ShowEndPanel()"));

	if (EndPanelInstance && WorldRef)
	{
		EndPanelInstance->SetVisibility(ESlateVisibility::Visible);

		FTimerHandle HideTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UCJS_UIManager::HideEndPanel, 3.f, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EndPanelInstance or WorldRef is null in ShowEndPanel()"));
	}
}

void UCJS_UIManager::HideStartPanel()
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::HideStartPanel()"));
	StartPanelInstance->SetVisibility(ESlateVisibility::Hidden);

    ShowQuitUI();

}
void UCJS_UIManager::HideEndPanel()
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::HideEndPanel()"));
	EndPanelInstance->SetVisibility(ESlateVisibility::Hidden);

    // LobbyPlayer 인스턴스 가져오기
    if ( WorldRef && PC )
    {
        // PlayerController의 Pawn을 가져옴
        APawn* Pawn = PC->GetPawn();
        if ( Pawn )
        {
            // Pawn을 ACJS_LobbyPlayer로 캐스팅
            ACJS_LobbyPlayer* LobbyPlayer = Cast<ACJS_LobbyPlayer>(Pawn);
            if ( LobbyPlayer )
            {
                // ShowAimPoint() 호출
                LobbyPlayer->ShowAimPoint();
                UE_LOG(LogTemp, Warning, TEXT("ShowAimPoint() called on LobbyPlayer"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to cast Pawn to ACJS_LobbyPlayer"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerController's Pawn is null in HideEndPanel()"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WorldRef or PC is null in HideEndPanel()"));
    }
    
}


void UCJS_UIManager::ShowQuitUI()
{
    UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::ShowQuitUI()"));
	if (QuitUIInstance && PC)
	{
		QuitUIInstance->AddToViewport();

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(QuitUIInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
		PC->bEnableMouseOverEvents = true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("QuitUIInstance or PC is null in ShowQuitUI"));
	}
}

void UCJS_UIManager::HideQuitUI()
{
    UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::HideQuitUI()"));
	if (QuitUIInstance && PC)
	{
		QuitUIInstance->RemoveFromViewport();

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
		PC->bEnableMouseOverEvents = false;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("QuitUIInstance or PC is null in HideQuitUI"));
	}
}
