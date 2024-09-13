// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_CountPlayerUI.h"
#include "Components/TextBlock.h"

//void UCJS_CountPlayerUI::AddPlayerNum()
//{
//	UE_LOG(LogTemp, Warning, TEXT("UCJS_CountPlayerUI::AddPlayerNum()"));
//	CurPlayer += 1;
//	ShowPlayerNum(CurPlayer, MaxPlayer);
//
//	//if (CurPlayer == MaxPlayer)
//	//{
//	//	// 작품1번 체험 위치로 이동
//	//}
//}

void UCJS_CountPlayerUI::InitPlayerNum(int32 curNum, int32 maxNum)
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_CountPlayerUI::InitPlayerNum()"));

	Txt_CurNum = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_CurNum")));
	Txt_MaxNum = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_MaxNum")));

	ShowPlayerNum(curNum, maxNum);
}


void UCJS_CountPlayerUI::ShowPlayerNum(int32 curNum, int32 maxNum)
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_CountPlayerUI::ShowPlayerNum()"));

	if (Txt_CurNum)
	{
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		Txt_CurNum->SetText(FText::AsNumber(curNum, &Opts));
		UE_LOG(LogTemp, Warning, TEXT("UCJS_CountPlayerUI::ShowPlayerNum() - Txt_CurNum updated to %d"), curNum);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Txt_CurNum is null"));
	}

	if (Txt_MaxNum)
	{
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		Txt_MaxNum->SetText(FText::AsNumber(maxNum, &Opts));
		UE_LOG(LogTemp, Warning, TEXT("UCJS_CountPlayerUI::ShowPlayerNum() - Txt_MaxNum updated to %d"), maxNum);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Txt_MaxNum is null"));
	}
}
