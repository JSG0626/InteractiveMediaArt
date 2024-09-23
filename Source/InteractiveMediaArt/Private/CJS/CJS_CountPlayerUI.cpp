// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_CountPlayerUI.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"


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

	curNum > 0 ? SetVisibility(ESlateVisibility::Visible) : SetVisibility(ESlateVisibility::Hidden);

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

void UCJS_CountPlayerUI::UpdateProgressBar(int32 curNum, int32 maxNum)
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_CountPlayerUI::UpdateProgressBar()"));

	// 실수로 변환하여 나눗셈 수행
	if ( maxNum > 0 ) // maxNum이 0이 아닐 때만 나누기 수행
	{
		float PercentValue = static_cast<float>(curNum) / static_cast<float>(maxNum);
		ProgressBar_CountNum->SetPercent(PercentValue);
		UE_LOG(LogTemp, Warning, TEXT("ProgressBar Percent set to: %f"), PercentValue);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("maxNum is zero, cannot divide."));
	}

}
