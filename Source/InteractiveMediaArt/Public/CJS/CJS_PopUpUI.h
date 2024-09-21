// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CJS_PopUpUI.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API UCJS_PopUpUI : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION()
    void OnCloseButtonClicked();

    void SetOwningBnt(class ACJS_PopUpBnt* InOwningBnt, class APlayerController* InPlayerController);
	
protected:
    virtual void NativeConstruct() override;

private:
    class ACJS_PopUpBnt* OwningBnt; // PopUpBnt 액터의 참조
    class APlayerController* PlayerController;  // 처음 버튼을 누른 플레이어 컨트롤러 참조

    UPROPERTY(meta = (BindWidget))
    class UButton* Button_Close;
	
};
