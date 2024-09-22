// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_PopUpUI.h"
#include "CJS/CJS_PopUpBnt.h"

#include "Components/Button.h"

void UCJS_PopUpUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Close)
	{
		UE_LOG(LogTemp, Warning, TEXT(" UCJS_PopUpUI::NativeConstruct()::Button_Close Ok"));
		Button_Close->OnClicked.AddDynamic(this, &UCJS_PopUpUI::OnCloseButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Button_Close is null in ACJS_PopUpBnt::NativeConstruct()"));
	}
}

void UCJS_PopUpUI::OnCloseButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT(" UCJS_PopUpUI::OnCloseButtonClicked()"));
	//if (OwningBnt)
	//{
	//	OwningBnt->ToggleWidgetVisibility(); // PopUpBnt의 함수 호출
	//}


	// 현재 컨트롤러와 UI를 연 플레이어의 컨트롤러 비교
	if (OwningBnt && PlayerController)
	{
		// 현재 닫기 버튼을 클릭한 플레이어의 컨트롤러 가져오기 (멀티플레이어 환경에서)
		APlayerController* CurrentController = Cast<APlayerController>(OwningBnt->GetWorld()->GetFirstPlayerController()); // 클릭한 플레이어의 컨트롤러 가져오기

		if (CurrentController == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("CurrentController is null in OnCloseButtonClicked"));
			return;
		}

		if (CurrentController == PlayerController)  // 같을 때만 실행
		{
			//OwningBnt->ToggleWidgetVisibility(PlayerController);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UCJS_PopUpUI::OnCloseButtonClicked():: Player controllers do not match."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCJS_PopUpUI::OnCloseButtonClicked():: OwningBnt or PlayerController is null."));
	}
}

void UCJS_PopUpUI::SetOwningBnt(class ACJS_PopUpBnt* InOwningBnt, class APlayerController* InPlayerController)
{
	UE_LOG(LogTemp, Warning, TEXT(" UCJS_PopUpUI::SetOwningBnt()"));
	OwningBnt = InOwningBnt;
	PlayerController = InPlayerController;

	// OwningBnt 또는 PlayerController가 null일 경우 로그 출력
	if (!OwningBnt)
	{
		UE_LOG(LogTemp, Error, TEXT("UCJS_PopUpUI::SetOwningBnt():: OwningBnt is null."));
	}

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("UCJS_PopUpUI::SetOwningBnt():: PlayerController is null."));
	}
}
