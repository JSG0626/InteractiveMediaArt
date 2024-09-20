// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_UIManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TimerHandle.h"

void UCJS_UIManager::BeginPlay()
{
	Super::BeginPlay();

	// 시작 패널
	if (StartPanelFactory)
	{
		UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::BeginPlay()::StartPanelFactory OK"));
		StartPanelInstance = CreateWidget<UUserWidget>(GetWorld(), StartPanelFactory);
		if (StartPanelInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::BeginPlay():StartPanelInstance OK"));
			StartPanelInstance->AddToViewport();
			HideStartPanel();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::BeginPlay():StartPanelInstance is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::BeginPlay()::StartPanelFactory is null"));
	}

	// 종료 패널
	if (EndPanelFactory)
	{
		UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::BeginPlay()::EndPanelFactory OK"));
		EndPanelInstance = CreateWidget<UUserWidget>(GetWorld(), EndPanelFactory);
		if (EndPanelInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::BeginPlay():EndPanelInstance OK"));
			EndPanelInstance->AddToViewport();
			HideEndPanel();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::BeginPlay():EndPanelInstance is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCJS_UIManager::BeginPlay()::EndPanelFactory is null"));
	}
}

void UCJS_UIManager::ShowStartPanel()
{
	StartPanelInstance->SetVisibility(ESlateVisibility::Visible);

	FTimerHandle HideTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UCJS_UIManager::HideStartPanel, 3.f, false);
}
void UCJS_UIManager::ShowEndPanel()
{
	EndPanelInstance-> SetVisibility(ESlateVisibility::Visible);

	FTimerHandle HideTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UCJS_UIManager::HideEndPanel, 3.f, false);
}

void UCJS_UIManager::HideStartPanel()
{
	StartPanelInstance->SetVisibility(ESlateVisibility::Hidden);
}
void UCJS_UIManager::HideEndPanel()
{
	EndPanelInstance->SetVisibility(ESlateVisibility::Hidden);
}
