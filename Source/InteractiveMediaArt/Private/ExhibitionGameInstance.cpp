// Fill out your copyright notice in the Description page of Project Settings.


#include "ExhibitionGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "../InteractiveMediaArt.h"


void UExhibitionGameInstance::Init()
{
	Super::Init();

	if (auto* subSystem = IOnlineSubsystem::Get()) // subSystem�̶�� ������ �����
	{
		SessionInterface = subSystem->GetSessionInterface();

		// ����� ��û -> ����
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

	// 1. ���� ������ ����ϴ°�? �ƴϿ�;
	settings.bIsDedicated = false;

	// 2. ����(LAN)���� ��ġ�ϴ°�?
	FName subsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
	settings.bIsLANMatch = subsystemName == "NULL";

	// 3. ��Ī�� ����(true) Ȥ�� �����(false, �ʴ븦 ���ؼ� ��Ī)
	settings.bShouldAdvertise = true;

	// 4. ������ ���� ���� ��� ���� ( �¶���, �ڸ� ���� ��� )
	settings.bUsesPresence = true;

	// 5. �߰��� ���� �����Ѱ�?
	settings.bAllowJoinViaPresence = true;
	settings.bAllowJoinInProgress = true;

	// 6. �ִ� �ο���
	//settings.NumPublicConnections = playerCount;

	// 7. Ŀ���� ����, Ping = ����ſ� �ɸ��� �ð�
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
