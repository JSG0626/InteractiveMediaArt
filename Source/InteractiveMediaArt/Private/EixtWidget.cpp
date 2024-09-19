// Fill out your copyright notice in the Description page of Project Settings.


#include "EixtWidget.h"
#include "Components/Button.h"
#include "ExhibitionGameInstance.h"
#include "DoorActor.h"

void UEixtWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Retry->OnClicked.AddDynamic(this, &UEixtWidget::OnRetry);
	Btn_Exit->OnClicked.AddDynamic(this, &UEixtWidget::OnExit);

}

void UEixtWidget::VisibleExitUI()
{
	SetVisibility(ESlateVisibility::Visible);
	AddToViewport();

	auto* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		pc->SetShowMouseCursor(true);
		FInputModeUIOnly InputModeData;
		pc->SetInputMode(InputModeData);
	}
}

void UEixtWidget::OnRetry()
{

	SetVisibility(ESlateVisibility::Hidden);

	auto* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		pc->SetShowMouseCursor(false);
		FInputModeGameOnly InputModeData;
		pc->SetInputMode(InputModeData);

		// DoorActor의 CloseDoor 호출
		if (DoorActor)
		{
			DoorActor->Return();
		}
	}
}

void UEixtWidget::OnExit()
{
	// 프로그램 종료
	auto* gi = Cast<UExhibitionGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->ExitSession();
	}
}

void UEixtWidget::SetDoorActor(ADoorActor* InDoorActor)
{
	DoorActor = InDoorActor;
}
