// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_QuitUI.h"
#include "CJS/CJS_LobbyPlayer.h"

#include "Components/Button.h"

void UCJS_QuitUI::OnQuitButtonClicked()
{
	ENetMode NetMode = GetWorld()->GetNetMode();
	FString NetModeString = NetMode == NM_Client ? TEXT("Client") : TEXT("Server");
	UE_LOG(LogTemp, Warning, TEXT("UCJS_QuitUI::OnQuitButtonClicked() called on %s"), *NetModeString);

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		ACJS_LobbyPlayer* LobbyPlayer = Cast<ACJS_LobbyPlayer>(PC->GetPawn());
		if (LobbyPlayer)
		{
			LobbyPlayer->OnExitBnt();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UCJS_QuitUI::OnQuitButtonClicked():: LobbyPlayer is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCJS_QuitUI::OnQuitButtonClicked():: PC is null"));
	}
}

void UCJS_QuitUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Quit)
	{
		Button_Quit->OnClicked.AddDynamic(this, &UCJS_QuitUI::OnQuitButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Button_Quit is null in UCJS_QuitUI::NativeConstruct()"));
	}
}
