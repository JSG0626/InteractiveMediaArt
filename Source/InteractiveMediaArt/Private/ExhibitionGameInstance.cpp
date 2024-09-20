// Fill out your copyright notice in the Description page of Project Settings.


#include "ExhibitionGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "../InteractiveMediaArt.h"
#include "../../../../Plugins/Online/OnlineBase/Source/Public/Online/OnlineSessionNames.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/GameplayStatics.h"


void UExhibitionGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get(); // IOnlineSubsystem ��������

	if (SubSystem)
	{
		PRINTLOG(TEXT("OnlineSubsystem: %s"), *SubSystem->GetSubsystemName().ToString());

		SessionInterface = SubSystem->GetSessionInterface(); // SessionInterface ��������

		if (SessionInterface.IsValid())
		{
			PRINTLOG(TEXT("SessionInterface is valid"));
		}
		else
		{
			PRINTLOG(TEXT("SessionInterface is invalid"));
		}
	}
	else
	{
		PRINTLOG(TEXT("No OnlineSubsystem found"));
	}

	if (auto* subSystem = IOnlineSubsystem::Get())
	{
		SessionInterface = subSystem->GetSessionInterface();

		// �� ���� ��û -> ����
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UExhibitionGameInstance::OnMyCreateSessionComplete);

		// �� ã�� ����
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UExhibitionGameInstance::OnMyFindSessionsCompleteDelegates);

		// �� ���� ����
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UExhibitionGameInstance::OnMyJoinSessionComplete);

		// �� ���� ����
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UExhibitionGameInstance::OnMyDestroySessionComplete);
	}

	/*FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle , [&]()
	{
		CreateMySession(MySessionName, 10);
		}, 3, false);*/
}

void UExhibitionGameInstance::CreateMySession()
{
	FOnlineSessionSettings settings;

	// 1. ���� ������ ����ϴ°�? �ƴϿ�;
	settings.bIsDedicated = false;

	// 2. ����(LAN)���� ��ġ�ϴ°�?
	FName subsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
	//settings.bIsLANMatch = subsystemName == "NULL";
	settings.bIsLANMatch = false;

	// �κ����� Ȱ��ȭ�Ѵ�. (Host �Ϸ��� �ʿ�)
	settings.bUseLobbiesIfAvailable = true;

	// 3. ��Ī�� ����(true) Ȥ�� �����(false, �ʴ븦 ���ؼ� ��Ī)
	settings.bShouldAdvertise = true;

	// 4. ������ ���� ���� ��� ���� ( �¶���, �ڸ� ���� ��� )
	settings.bUsesPresence = true;

	// 5. �߰��� ���� �����Ѱ�?
	settings.bAllowJoinViaPresence = true;
	settings.bAllowJoinInProgress = true;

	settings.bAllowJoinViaPresenceFriendsOnly = false;
	settings.bAntiCheatProtected = false;
	settings.bUseLobbiesIfAvailable = true;

	// 6. �ִ� �ο���
	settings.NumPublicConnections = 100;

	// 7. Ŀ���� ����, Ping = ����ſ� �ɸ��� �ð�
	//settings.Set(FName("ROOM_NAME"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	settings.Set(FName("HOST_NAME"), MySessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	PRINTLOG(TEXT("Attempting to create session with name: %s"), *MySessionName);
	PRINTLOG(TEXT("Create Session Start hostName : %s"), *MySessionName);

	//FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	SessionInterface->CreateSession(0, FName(MySessionName), settings);

}

void UExhibitionGameInstance::OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PRINTLOG(TEXT("JSG "));
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("OnMyCreateSessionComplete is Success"));
		PRINTLOG(TEXT("Session created successfully with name: %s"), *SessionName.ToString());
		GetWorld()->ServerTravel(TEXT("/Game/ArtProject/Main/Alpha_Exhibition?listen"));
	}
	else
	{
		PRINTLOG(TEXT("Failed to create session with name: %s"), *SessionName.ToString());
		PRINTLOG(TEXT("OnMyCreateSessionComplete is Failed"));
	}
}

void UExhibitionGameInstance::FindSessions()
{
	PRINTLOG(TEXT("SessionInterface valid: %s"), SessionInterface.IsValid() ? TEXT("True") : TEXT("False"));

	if (GetWorld()->GetNetMode() == NM_ListenServer || GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		PRINTLOG(TEXT("FindSessions: Skipping on server"));
		return;
	}

	if (!SessionInterface.IsValid())
	{
		PRINTLOG(TEXT("FindSessions: SessionInterface is invalid. Cannot find sessions."));
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (!SessionSearch.IsValid())
	{
		PRINTLOG(TEXT("FindSessions: Failed to create SessionSearch!"));
		return;
	}

	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	//SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(FName("HOST_NAME"), MySessionName, EOnlineComparisonOp::Equals);
	//SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 40;


	if (!SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		PRINTLOG(TEXT("FindSessions: Failed to start FindSessions!"));
	}
	else
	{
		PRINTLOG(TEXT("FindSessions: Successfully started FindSessions"));
	}

	//PRINTLOG(TEXT("Searching for sessions with name: %s, IsLAN: %s"), *MySessionName, SessionSearch->bIsLanQuery ? TEXT("True") : TEXT("False"));
}

void UExhibitionGameInstance::OnMyFindSessionsCompleteDelegates(bool bWasSuccessful)
{
	// �������� ȣ����� �ʰ���
	if (GetWorld()->GetNetMode() == NM_ListenServer || GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		PRINTLOG(TEXT("OnMyFindSessionsCompleteDelegates: Skipping on server"));
		return;
	}

	if (!SessionInterface.IsValid() || !SessionSearch.IsValid())
	{
		PRINTLOG(TEXT("OnMyFindSessionsCompleteDelegates: SessionInterface or SessionSearch is invalid!"));
		return;
	}

	if (bWasSuccessful)
	{
		TArray<FOnlineSessionSearchResult> results = SessionSearch->SearchResults;
		PRINTLOG(TEXT("Found %d sessions"), results.Num());
		bool bJoinSession = false;
		for (int32 i = 0; i < results.Num(); i++)
		{
			FOnlineSessionSearchResult ret = results[i];
			if (false == ret.IsValid())
			{
				continue;
			}

			FRoomInfo roomInfo;
			roomInfo.index = i;

			FString HostName;
			results[i].Session.SessionSettings.Get(FName("HOST_NAME"), HostName);
			PRINTLOG(TEXT("Session %d: HostName=%s"), i, *HostName);

			if (HostName == MySessionName)
			{
				PRINTLOG(TEXT("Join Session Call"));
				JoinSession(roomInfo.index);


				bJoinSession = true;
				break;
			}
		}

		if (!bJoinSession)
		{
			CreateMySession();
		}
	}

	else
	{
		PRINTLOG(TEXT("OnMyFindSessionsCompleteDelegates is Failed!!!"));
	}

}

void UExhibitionGameInstance::JoinSession(int32 index)
{
	// SessionSearch�� ��ȿ���� Ȯ��
	PRINTLOG(TEXT("Join Session Called"));

	if (!SessionSearch.IsValid() || SessionSearch->SearchResults.Num() <= index)
	{
		PRINTLOG(TEXT("JoinSession failed: SessionSearch is invalid or no search results."));
		return;
	}

	auto result = SessionSearch->SearchResults[index];
	SessionInterface->JoinSession(0, FName(MySessionName), result);
}

void UExhibitionGameInstance::OnMyJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type EOnJoinSessionCompleteResult)
{
	PRINTLOG(TEXT("OnMyJoinSessionComplete"));
	if (EOnJoinSessionCompleteResult == EOnJoinSessionCompleteResult::Success)
	{
		// ������ �ִ� ������ ������ ������ �ʹ�.
		auto* pc = GetWorld()->GetFirstPlayerController();

		FString url;
		if (SessionInterface->GetResolvedConnectString(SessionName, url))
		{
			PRINTLOG(TEXT("Travelling to session URL: %s"), *url);
			pc->ClientTravel(url, ETravelType::TRAVEL_Absolute);
		}
		else
		{
			PRINTLOG(TEXT("Failed to get resolved connect string for session."));
		}
	}
	else
	{
		PRINTLOG(TEXT("JoinSession failed with result: %d"), static_cast<int32>(EOnJoinSessionCompleteResult));
	}
}

void UExhibitionGameInstance::ExitSession()
{
	ServerRPCExitSession();
}

void UExhibitionGameInstance::ServerRPCExitSession_Implementation()
{
	MulticastRPCExitSession();
}

void UExhibitionGameInstance::MulticastRPCExitSession_Implementation()
{
	// �� ���� ��û.
	SessionInterface->DestroySession(FName(MySessionName));
}

void UExhibitionGameInstance::OnMyDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// ���α׷��� �����ϰ� �ʹ�.
		FGenericPlatformMisc::RequestExit(false);
	}
}
