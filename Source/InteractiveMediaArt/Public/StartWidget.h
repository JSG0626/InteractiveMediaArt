// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartWidget.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API UStartWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(Bindwidget))
	class UButton* CR_Btn_CreateSession;
	
	//UPROPERTY(meta=(Bindwidget))
	//class UButton* CR_Btn_JoinSession;

	UFUNCTION()
	void CR_OnClickCreateSession();
	
	//UFUNCTION()
	//void CR_OnClickJoinSession();

};
