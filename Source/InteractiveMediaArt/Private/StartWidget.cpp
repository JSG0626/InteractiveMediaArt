// Fill out your copyright notice in the Description page of Project Settings.


#include "StartWidget.h"
#include "Components/Button.h"
#include "ExhibitionGameInstance.h"
#include "../InteractiveMediaArt.h"
#include "Kismet/GameplayStatics.h"

void UStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CR_Btn_Start->OnClicked.AddDynamic(this, &UStartWidget::CR_OnClickCreateSession);
	
}

void UStartWidget::CR_OnClickCreateSession()
{
	auto* gi = Cast<UExhibitionGameInstance>(GetWorld()->GetGameInstance());
	if (!gi) return;
	
	// 서버인지 확실하게 체크
	bool isServer = GetWorld()->GetNetMode() == NM_ListenServer || GetWorld()->GetNetMode() == NM_DedicatedServer;

	if (isServer)
	{
		PRINTLOG(TEXT("Server is creating session"));
		// 방 생성
		gi->CreateMySession();
	}
	else if (!isServer) // 서버가 아니고 클라이언트라면
	{
		gi->OnSessionSearchCompleteDelegate.AddDynamic(this, &UStartWidget::OnSessionSearchComplete);

		// 5초 후에 세션 검색 시작
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [gi]()
			{
				//PRINTLOG(TEXT("Starting session search after delay"));
				gi->FindSessions();
			}, 5.0f, false);
	}
	
}

void UStartWidget::OnSessionSearchComplete()
{
	auto* gi = Cast<UExhibitionGameInstance>(GetWorld()->GetGameInstance());
	if (!gi) return;

	// 첫 번째 세션에 자동으로 조인
	gi->JoinSession(0);

	PRINTLOG(TEXT("Attempting to join the first found session."));
}
