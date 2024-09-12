// Fill out your copyright notice in the Description page of Project Settings.


#include "StartWidget.h"
#include "Components/Button.h"
#include "ExhibitionGameInstance.h"

void UStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CR_Btn_Start->OnClicked.AddDynamic(this, &UStartWidget::CR_OnClickCreateSession);
	
}

void UStartWidget::CR_OnClickCreateSession()
{
	//FString roomName;
	//int32 playerCount;

	// GameInstance 가져오기
	auto* gameinstance = Cast<UExhibitionGameInstance>(GetWorld()->GetGameInstance());
	if (gameinstance)
	{
		// 방 생성
		gameinstance->CreateMySession(/*roomName, playerCount*/);
	}

}
