// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM_Art3UI.h"

void ULHM_Art3UI::NativeConstruct()
{

}

void ULHM_Art3UI::SetVisible()
{
	UE_LOG(LogTemp, Warning, TEXT("ULHM_Art3UI::SetVisible()"));
	SetVisibility(ESlateVisibility::Visible);
	FTimerHandle timer;
	GetWorld()->GetTimerManager().SetTimer(timer, this, &ULHM_Art3UI::SetHidden, 10.0f, false);
}

void ULHM_Art3UI::SetHidden()
{
	UE_LOG(LogTemp, Warning, TEXT("ULHM_Art3UI::SetHidden()"));
	SetVisibility(ESlateVisibility::Hidden);
}

