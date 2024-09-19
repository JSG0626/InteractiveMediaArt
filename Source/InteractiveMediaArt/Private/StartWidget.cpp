// Fill out your copyright notice in the Description page of Project Settings.


#include "StartWidget.h"
#include "Components/Button.h"
#include "ExhibitionGameInstance.h"
#include "../InteractiveMediaArt.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"

void UStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CR_Btn_CreateSession->OnClicked.AddDynamic(this, &UStartWidget::CR_OnClickCreateSession);
	//CR_Btn_JoinSession->OnClicked.AddDynamic(this, &UStartWidget::CR_OnClickJoinSession);
}

void UStartWidget::CR_OnClickCreateSession()
{
	auto* gi = Cast<UExhibitionGameInstance>(GetWorld()->GetGameInstance());
	if (!gi) return;
	
	//PRINTLOG(TEXT("Server is creating session"));
	//gi->CreateMySession();

	PRINTLOG(TEXT("Searching for existing sessions"));
	gi->FindSessions();
}

//void UStartWidget::CR_OnClickJoinSession()
//{
//	auto* gi = Cast<UExhibitionGameInstance>(GetWorld()->GetGameInstance());
//	if (!gi) return;
//
//	PRINTLOG(TEXT("Searching for existing sessions"));
//	gi->FindSessions();
//}

