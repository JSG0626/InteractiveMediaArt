// Fill out your copyright notice in the Description page of Project Settings.


#include "ExhibitionGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "../InteractiveMediaArt.h"


void UExhibitionGameInstance::Init()
{
	Super::Init();

	if (auto* subSystem = IOnlineSubsystem::Get()) // subSystem이라는 변수를 담았음
	{
		SessionInterface = subSystem->GetSessionInterface();

		// 방생성 요청 -> 응답
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UExhibitionGameInstance::OnMyCreateSessionComplete);
	}

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle , [&]()
	{
		CreateMySession(/*MySessionName*/);
		}, 3, false);
}

void UExhibitionGameInstance::CreateMySession(/*FString roomName, int32 playerCount*/)
{
	FOnlineSessionSettings settings;

	// 1. 전용 서버를 사용하는가? 아니요;
	settings.bIsDedicated = false;

	// 2. 랜선(LAN)으로 매치하는가?
	FName subsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
	settings.bIsLANMatch = subsystemName == "NULL";

	// 3. 매칭이 공개(true) 혹은 비공개(false, 초대를 통해서 매칭)
	settings.bShouldAdvertise = true;

	// 4. 유저의 상태 정보 사용 여부 ( 온라인, 자리 여부 등등 )
	settings.bUsesPresence = true;

	// 5. 중간에 난입 가능한가?
	settings.bAllowJoinViaPresence = true;
	settings.bAllowJoinInProgress = true;

	// 6. 최대 인원수
	//settings.NumPublicConnections = playerCount;

	// 7. 커스텀 정보, Ping = 통수신에 걸리는 시간
	//settings.Set(FName("ROOM_NAME"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	settings.Set(FName("HOST_NAME"), MySessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	SessionInterface->CreateSession(*netID, FName(MySessionName), settings);

	PRINTLOG(TEXT("Create Session Start hostName : %s"), *MySessionName);
}

void UExhibitionGameInstance::OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("OnMyCreateSessionComplete is Success"));
	}
	else
	{
		PRINTLOG(TEXT("OnMyCreateSessionComplete is Failed"));
	}
}
