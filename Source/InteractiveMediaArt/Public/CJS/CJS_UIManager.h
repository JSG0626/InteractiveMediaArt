// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CJS_UIManager.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API UCJS_UIManager : public UObject
{
	GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    // 시작 및 종료 패널을 표시하는 함수
    void ShowStartPanel();
    void ShowEndPanel();

    // 시작 및 종료 패널을 숨기는 함수
    void HideStartPanel();
    void HideEndPanel();


private:
    
    // 시작 및 종료 패널 위젯 클래스
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UUserWidget> StartPanelFactory;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UUserWidget> EndPanelFactory;

    // 시작 및 종료 패널 위젯 인스턴스
    UPROPERTY()
    class UUserWidget* StartPanelInstance;
    UPROPERTY()
    class UUserWidget* EndPanelInstance;


	
};
