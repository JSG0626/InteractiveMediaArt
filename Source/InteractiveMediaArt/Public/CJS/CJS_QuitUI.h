// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CJS_QuitUI.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API UCJS_QuitUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Quit;

	//UPROPERTY()
	//class ACJS_LobbyPlayer* Me;

	UFUNCTION()
    void OnQuitButtonClicked();

protected:
	virtual void NativeConstruct() override;
};
