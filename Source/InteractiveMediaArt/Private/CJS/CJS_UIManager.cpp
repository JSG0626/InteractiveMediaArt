// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_UIManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TimerHandle.h"


UCJS_UIManager::UCJS_UIManager() : StartPanelInstance(nullptr), EndPanelInstance(nullptr), WorldRef(nullptr)
{}

void UCJS_UIManager::Initialize(UWorld* World, TSubclassOf<UUserWidget> InStartPanelFactory, TSubclassOf<UUserWidget> InEndPanelFactory)
{
	WorldRef = World;
	StartPanelFactory = InStartPanelFactory;
	EndPanelFactory = InEndPanelFactory;

	if (!WorldRef)
	{
		UE_LOG(LogTemp, Error, TEXT("UCJS_UIManager::Initialize(): WorldRef is null"));
		return;
	}

	// 시작 패널 인스턴스 생성
	if (StartPanelFactory)
	{
		StartPanelInstance = CreateWidget<UUserWidget>(WorldRef, StartPanelFactory);
		if (StartPanelInstance)
		{
			StartPanelInstance->AddToViewport();
			HideStartPanel();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StartPanelFactory is not set!"));
	}

	// 종료 패널 인스턴스 생성
	if (EndPanelFactory)
	{
		EndPanelInstance = CreateWidget<UUserWidget>(WorldRef, EndPanelFactory);
		if (EndPanelInstance)
		{
			EndPanelInstance->AddToViewport();
			HideEndPanel();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EndPanelFactory is not set!"));
	}
}

void UCJS_UIManager::ShowStartPanel()
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::ShowStartPanel()"));
	if (StartPanelInstance && WorldRef)
	{
		StartPanelInstance->SetVisibility(ESlateVisibility::Visible);

		FTimerHandle HideTimerHandle;
		WorldRef->GetTimerManager().SetTimer(HideTimerHandle, this, &UCJS_UIManager::HideStartPanel, 3.f, false);
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
}
void UCJS_UIManager::HideEndPanel()
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::HideEndPanel()"));
	EndPanelInstance->SetVisibility(ESlateVisibility::Hidden);
}
