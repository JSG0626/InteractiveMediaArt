// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Art1_Main.h"
#include "Components/TextBlock.h"

void USG_Art1_Main::SetRestTime(int32 RestTime)
{
	TX_RestTime->SetText(FText::AsNumber(RestTime));
}
