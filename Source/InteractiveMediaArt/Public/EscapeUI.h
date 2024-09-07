// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EscapeUI.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API UEscapeUI : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly)
	class ACameraActor* OriginalCameraActor;

public:
	UPROPERTY(EditDefaultsOnly)
	class UButton* Button_Escape;


    UFUNCTION(BlueprintCallable)
    void OnExitButtonClicked();

protected:
	virtual void NativeConstruct() override; // UI가 생성될 때 호출되는 함수
};
