// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SG_Art1_Main.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API USG_Art1_Main : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TX_RestTime;

	void SetRestTime(int32 RestTime);

};
