// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CJS_CountPlayerUI.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API UCJS_CountPlayerUI : public UUserWidget
{
	GENERATED_BODY()

public:
	/*UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_CurNum;*/
	UPROPERTY(meta = (BindWidget))  
	class UTextBlock* Txt_CurNum;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_MaxNum;
	
	void InitPlayerNum(int32 curNum, int32 maxNum);
	void ShowPlayerNum(int32 curNum, int32 maxNum);
};
