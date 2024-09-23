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
    // 생성자
    UCJS_UIManager();

    // 초기화 함수
    void Initialize(UWorld* World, TSubclassOf<UUserWidget> InStartPanelFactory, TSubclassOf<UUserWidget> InEndPanelFactory, TSubclassOf<UUserWidget> InQuitUIFactory, APlayerController* InPC);
    //void Initialize(UWorld* World, TSubclassOf<UUserWidget> InStartPanelFactory, TSubclassOf<UUserWidget> InEndPanelFactory);

    // 시작 및 종료 패널을 표시하는 함수
    void ShowStartPanel();
    void ShowEndPanel();

    // 시작 및 종료 패널을 숨기는 함수
    void HideStartPanel();
    void HideEndPanel();

    // 종료 버튼
    UFUNCTION()
    void ShowQuitUI();
    void HideQuitUI();


private:
    UPROPERTY()
    class APlayerController* PC;

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

    // Quit UI
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UUserWidget> QuitUIFactory;
    UPROPERTY()
    class UUserWidget* QuitUIInstance;
    //UPROPERTY()
	//class UButton* Button_Quit;


    // 월드 레퍼런스 (UObject에서는 GetWorld()를 직접 사용할 수 없으므로, 월드 레퍼런스를 멤버 변수로 저장하면 좋음)
    class UWorld* WorldRef;
	
};
