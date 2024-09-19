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

	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get(); // IOnlineSubsystem 가져오기

	if (SubSystem)
	{
		PRINTLOG(TEXT("OnlineSubsystem: %s"), *SubSystem->GetSubsystemName().ToString());

		SessionInterface = SubSystem->GetSessionInterface(); // SessionInterface 가져오기

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

		// 방 생성 요청 -> 응답
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UExhibitionGameInstance::OnMyCreateSessionComplete);

		// 방 찾기 응답
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UExhibitionGameInstance::OnMyFindSessionsCompleteDelegates);

		// 방 입장 응답
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UExhibitionGameInstance::OnMyJoinSessionComplete);

		// 방 퇴장 응답
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

	// 1. 전용 서버를 사용하는가? 아니요;
	settings.bIsDedicated = false;

	// 2. 랜선(LAN)으로 매치하는가?
	FName subsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
	settings.bIsLANMatch = subsystemName == "NULL";
	settings.bIsLANMatch = true;

	// 로비기능을 활성화한다. (Host 하려면 필요)
	settings.bUseLobbiesIfAvailable = true; 

	// 3. 매칭이 공개(true) 혹은 비공개(false, 초대를 통해서 매칭)
	settings.bShouldAdvertise = true;

	// 4. 유저의 상태 정보 사용 여부 ( 온라인, 자리 여부 등등 )
	settings.bUsesPresence = true;

	// 5. 중간에 난입 가능한가?
	settings.bAllowJoinViaPresence = true;
	settings.bAllowJoinInProgress = true;

	settings.bAllowJoinViaPresenceFriendsOnly = false;
	settings.bAntiCheatProtected = false;
	settings.bUseLobbiesIfAvailable = true;

	// 6. 최대 인원수
	settings.NumPublicConnections = 100;

	// 7. 커스텀 정보, Ping = 통수신에 걸리는 시간
	//settings.Set(FName("ROOM_NAME"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	settings.Set(FName("HOST_NAME"), MySessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	PRINTLOG(TEXT("Attempting to create session with name: %s"), *MySessionName);
	PRINTLOG(TEXT("Create Session Start hostName : %s"), *MySessionName);

	//FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	SessionInterface->CreateSession(0, FName(MySessionName), settings);

}

void UExhibitionGameInstance::OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{                                                                                                                                                                                                                                                                                                                                                                                                                                       
		PRINTLOG(TEXT("OnMyCreateSessionComplete is Success"));
		PRINTLOG(TEXT("Session created successfully with name: %s"), *SessionName.ToString());
		GetWorld()->ServerTravel(TEXT("/Game/ArtProject/CJS/Maps/CJS_Alpha_Exhibition?listen"));
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
	SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSearch->bIsLanQuery = true;
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
	// 서버에서 호출되지 않게함
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
		// TArray<FServerData> 
		TArray<FOnlineSessionSearchResult> results = SessionSearch->SearchResults;
		PRINTLOG(TEXT("Found %d sessions"), results.Num());

		if (results.Num() == 0)
		{
			CreateMySession();
		}
		else
		{
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

				JoinSession(roomInfo.index);
			}
		}

	}
	else
	{
		PRINTLOG(TEXT("OnMyFindSessionsCompleteDelegates is Failed!!!"));
	}

}

void UExhibitionGameInstance::JoinSession(int32 index)
{
	// SessionSearch가 유효한지 확인
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
	if (EOnJoinSessionCompleteResult == EOnJoinSessionCompleteResult::Success)
	{
		// 서버가 있는 레벨로 여행을 떠나고 싶다.
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
	// 방 퇴장 요청.
	SessionInterface->DestroySession(FName(MySessionName));
}

void UExhibitionGameInstance::OnMyDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// 프로그램을 종료하고 싶다.
		FGenericPlatformMisc::RequestExit(false);
	}
}
