// Fill out your copyright notice in the Description page of Project Settings.


#include "ExhibitionGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "../InteractiveMediaArt.h"
#include "../../../../Plugins/Online/OnlineBase/Source/Public/Online/OnlineSessionNames.h"
#include "GenericPlatform/GenericPlatformMisc.h"


void UExhibitionGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();
	if (SubSystem)
	{
		PRINTLOG(TEXT("OnlineSubsystem: %s"), *SubSystem->GetSubsystemName().ToString());
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
	//FName subsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
	//settings.bIsLANMatch = subsystemName == "NULL";
	settings.bIsLANMatch = false;

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
	settings.BuildUniqueId = 1;

	// 6. �ִ� �ο���
	//settings.NumPublicConnections = playerCount;

	// 7. Ŀ���� ����, Ping = ����ſ� �ɸ��� �ð�
	//settings.Set(FName("ROOM_NAME"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	settings.Set(FName("HOST_NAME"), MySessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	PRINTLOG(TEXT("Attempting to create session with name: %s"), *MySessionName);
	PRINTLOG(TEXT("Create Session Start hostName : %s"), *MySessionName);

	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	SessionInterface->CreateSession(*netID, FName(MySessionName), settings);

}

void UExhibitionGameInstance::OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("OnMyCreateSessionComplete is Success"));
		PRINTLOG(TEXT("Session created successfully with name: %s"), *SessionName.ToString());
		GetWorld()->ServerTravel(TEXT("/Game/ArtProject/CJS/Maps/CJS_Exhibition?listen"));
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
		PRINTLOG(TEXT("Reinitializing SessionInterface"));
		if (auto* SubSystem = IOnlineSubsystem::Get())
		{
			SessionInterface = SubSystem->GetSessionInterface();
		}
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	if (!SessionSearch.IsValid())
	{
		PRINTLOG(TEXT("FindSessions: Failed to create SessionSearch!"));
		return;
	}

	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	//SessionSearch->QuerySettings.Set(FName("HOST_NAME"), MySessionName, EOnlineComparisonOp::Equals);
	//SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->TimeoutInSeconds = 60;

	FUniqueNetIdPtr UserId = GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId().GetUniqueNetId();
	if (!UserId.IsValid())
	{
		PRINTLOG(TEXT("FindSessions: UserId is invalid!"));
		return;
	}

	if (!SessionInterface->FindSessions(*UserId, SessionSearch.ToSharedRef()))
	{
		PRINTLOG(TEXT("FindSessions: Failed to start FindSessions!"));
	}
	else
	{
		PRINTLOG(TEXT("FindSessions: Successfully started FindSessions"));
	}

	PRINTLOG(TEXT("Searching for sessions with name: %s, IsLAN: %s"), *MySessionName, SessionSearch->bIsLanQuery ? TEXT("True") : TEXT("False"));
}

void UExhibitionGameInstance::OnMyFindSessionsCompleteDelegates(bool bWasSuccessful)
{

	if (!SessionInterface.IsValid())
	{
		PRINTLOG(TEXT("OnMyFindSessionsCompleteDelegates: SessionInterface is invalid!"));
		return;
	}

	if (!SessionSearch.IsValid())
	{
		PRINTLOG(TEXT("OnMyFindSessionsCompleteDelegates: SessionSearch is invalid!"));
		return;
	}

	if (bWasSuccessful)
	{
		TArray<FOnlineSessionSearchResult> results = SessionSearch->SearchResults;
		PRINTLOG(TEXT("Found %d sessions"), results.Num());

		for (int32 i = 0; i < results.Num(); i++)
		{
			FString SessionId = results[i].GetSessionIdStr();
			FString HostName;
			results[i].Session.SessionSettings.Get(FName("HOST_NAME"), HostName);
			PRINTLOG(TEXT("Session %d: ID=%s, HostName=%s"), i, *SessionId, *HostName);
		}

		if (results.Num() > 0)
		{
			// ������ ã������ �˸�
			OnSessionSearchCompleteDelegate.Broadcast();
            PRINTLOG(TEXT("Session found. Broadcasting OnSessionSearchCompleteDelegate."));
		}
		else
		{
			PRINTLOG(TEXT("OnMyFindSessionsCompleteDelegates: No sessions found"));
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
	if (EOnJoinSessionCompleteResult::Success == EOnJoinSessionCompleteResult)
	{
		// ������ �ִ� ������ ������ ������ �ʹ�.
		auto* pc = GetWorld()->GetFirstPlayerController();

		FString url;
		SessionInterface->GetResolvedConnectString(SessionName, url);
		if (false == url.IsEmpty())
		{
			pc->ClientTravel(url, ETravelType::TRAVEL_Absolute);

			PRINTLOG(TEXT("OnMyJoinSessionComplete is Succeed!!!"));
		}
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
		
		// Ŭ���̾�Ʈ�� �κ�� ������ ����ʹ�.
		//auto* pc = GetWorld()->GetFirstPlayerController();
		//pc->ClientTravel(TEXT("/Game/ArtProject/LHM/Maps/LHM_Exit"), ETravelType::TRAVEL_Absolute);
	}
}
