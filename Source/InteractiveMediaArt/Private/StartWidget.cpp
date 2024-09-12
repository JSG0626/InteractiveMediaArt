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

	// GameInstance ��������
	auto* gameinstance = Cast<UExhibitionGameInstance>(GetWorld()->GetGameInstance());
	if (gameinstance)
	{
		// �� ����
		gameinstance->CreateMySession(/*roomName, playerCount*/);
	}

}
