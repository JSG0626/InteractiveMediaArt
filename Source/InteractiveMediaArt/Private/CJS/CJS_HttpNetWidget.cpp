// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_HttpNetWidget.h"
#include "CJS/CJS_HttpNetActor.h"

void UCJS_HttpNetWidget::NativeConstruct()
{
	//UE_LOG(LogTemp, Warning, TEXT("UCJS_HttpNetWidget::NativeConstruct()"));
	//ButtonWaveFileSend->OnClicked.AddDynamic(this, &UCJS_HttpNetWidget::OnMyClickSendPost);
}

void UCJS_HttpNetWidget::OnMyClickSendPost()
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_HttpNetWidget::OnMyClickSendPost()"));

	HttpNetActor->ReqPostWaveFile(ServerURL, WavFilePath);
}

void UCJS_HttpNetWidget::SetHttpNetActor(class ACJS_HttpNetActor* actor)
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_HttpNetWidget::SetHttpWaveFileActor()"));
	HttpNetActor = actor;
}

