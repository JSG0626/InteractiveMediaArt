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
	
	// �������� Ȯ���ϰ� üũ
	bool isServer = GetWorld()->GetNetMode() == NM_ListenServer || GetWorld()->GetNetMode() == NM_DedicatedServer;

	if (isServer)
	{
		PRINTLOG(TEXT("Server is creating session"));
		// �� ����
		gi->CreateMySession();
	}
	else if (!isServer) // ������ �ƴϰ� Ŭ���̾�Ʈ���
	{
		gi->OnSessionSearchCompleteDelegate.AddDynamic(this, &UStartWidget::OnSessionSearchComplete);

		// 5�� �Ŀ� ���� �˻� ����
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

	// ù ��° ���ǿ� �ڵ����� ����
	gi->JoinSession(0);

	PRINTLOG(TEXT("Attempting to join the first found session."));
}
