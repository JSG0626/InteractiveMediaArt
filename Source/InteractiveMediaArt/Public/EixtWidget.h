// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EixtWidget.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API UEixtWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(Bindwidget))
	class UButton* Btn_Exit;
	
	UPROPERTY(meta=(Bindwidget))
	class UButton* Btn_Retry;

	UFUNCTION()
	void VisibleExitUI();
	
	UFUNCTION()
	void OnRetry();

	UFUNCTION()
	void OnExit();
};
