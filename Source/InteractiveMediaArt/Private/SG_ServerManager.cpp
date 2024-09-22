// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_ServerManager.h"
#include "Engine/Engine.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h" 
#include "Kismet/GameplayStatics.h"

#include "SG_JsonUtilityLibrary.h"
#include "SG_ArtPlayer.h"
#include "../InteractiveMediaArt.h"
#include "CJS/CJS_LobbyPlayer.h"
#include "Net/UnrealNetwork.h"
// Sets default values
const uint32 MIN_JSON_SIZE = 1500;
const uint32 MAX_JSON_SIZE = 3000;
ASG_ServerManager::ASG_ServerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PyDeafultPath = FPaths::ProjectContentDir() + TEXT("Scripts/");

	bReplicates = true;
}

// Called when the game starts or when spawned
void ASG_ServerManager::BeginPlay()
{
	Super::BeginPlay();


}

void ASG_ServerManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("EndPlay"));
	Disconnect();

	if (FPlatformProcess::IsProcRunning(ServerProcHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("서버를 닫습니다."));
		FPlatformProcess::TerminateProc(ServerProcHandle);
		FPlatformProcess::CloseProc(ServerProcHandle);
	}
}

void ASG_ServerManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ASG_ServerManager, Me);
	DOREPLIFETIME(ASG_ServerManager, Player);
}

// Called every frame
void ASG_ServerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//TestReceiveData();

	if (Player)
	{
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("[%s] Owner: [%s] isLocallyControlled(): %d"), NETMODE, *Player->GetName(), Player->IsLocallyControlled()));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, TEXT("나 Player 널인데??"));
	}
	if (Player && Player->IsLocallyControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("나는 클라이언트 플레이어다~")));
		//if ( ServerProcHandle.IsValid() && ClientSocket )
		if (ClientSocket)
		{
			if (bReceiveSuccess) ReceiveData();
			else ReceiveRestData();

			uint32 pendingData = 0;
			ClientSocket->HasPendingData(pendingData);
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("%u"), pendingData));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("프로세스가 안켜졌거나 소켓 연결이 안됐거나임")));
		}
	}	
	else
	{
		//PRINTLOG(TEXT("Player is not locallyControlled"));
	}
}

void ASG_ServerManager::OnRep_Player()
{
	PRINTLOG(TEXT("Active"));
	if (Player && Player->IsLocallyControlled())
	{
		PRINTLOG(TEXT("if (Player && Player->IsLocallyControlled())"));
		//RunPythonScript(PyConnectServer);
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [&]()
			{
				CreateClient();
			}, 0.10f, false);
	}
	else
	{
		PRINTLOG(TEXT("Player is not local player"));
	}
}

void ASG_ServerManager::Active()
{
	PRINTLOG(TEXT("Active"));
	if (Player && Player->IsLocallyControlled())
	{
		PRINTLOG(TEXT("if (Player && Player->IsLocallyControlled())"));
		RunPythonScript(PyConnectServer);
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [&]()
			{
				CreateClient();
			}, 1.0f, false);
	}
	else
	{
		PRINTLOG(TEXT("Player is not local player"));
	}
}

void ASG_ServerManager::RunPythonScript(const FString& Path)
{
	FString scriptPath = PyDeafultPath + Path;
	bool bPythonExe = FPaths::FileExists(pythonExePath);
	bool bScript = FPaths::FileExists(scriptPath);
	bool bPythonWindowHidden = false;
	if (bPythonExe && bScript)
	{
		FString params = FString::Printf(TEXT("\"%s\""), *scriptPath);
		UE_LOG(LogTemp, Warning, TEXT("params: %s"), *params);
		ServerProcHandle = FPlatformProcess::CreateProc(*pythonExePath,
			*params,
			bPythonWindowHidden,	// bLaunchDetached: false 로 설정하여 CMD 창을 띄움
			bPythonWindowHidden,
			bPythonWindowHidden,
			&ServerPID,
			0,
			nullptr,
			nullptr
		);

		PRINTLOG(TEXT("ServerProcHandle is valid = %d"), ServerProcHandle.IsValid());
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("ServerProcHandle is valid = %d"), ServerProcHandle.IsValid()));
	}
	else
	{
		if (!bPythonExe) UE_LOG(LogTemp, Warning, TEXT("error: %s is not exist"), *pythonExePath);
		if (!bScript) UE_LOG(LogTemp, Warning, TEXT("error: %s is not exist"), *scriptPath);

		PRINTLOG(TEXT("Python executable or script not found"));
	}
}

void ASG_ServerManager::CreateClient()
{
	PRINTLOG(TEXT("CreateClient"));
	// 클라이언트 소켓 생성
	ClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("TCP Client"), false);
	ServerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	// 서버 IP와 포트 설정
	FIPv4Address ip;

	bool bCanBindAll;
	TSharedPtr<FInternetAddr> LocalIP = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
	uint32 myLocalAddress;

	LocalIP->GetIp(myLocalAddress);

	// 서버의 IP 주소 (예: LocalHost)
	//FIPv4Address::Parse(ServerIP, ip);
	// IP 주소 설정
	ServerAddr->SetIp(myLocalAddress);
	// 서버 포트
	ServerAddr->SetPort(ServerPort);

	bool bIsConnect = ClientSocket->Connect(*ServerAddr);
	FString connectionStr = bIsConnect ? TEXT("서버 연결 완료") : TEXT("서버 연결 실패");
	GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Green, FString::Printf(TEXT("%s"), *connectionStr));
	PRINTLOG(TEXT("%s"), *connectionStr);

	if (bIsConnect)
	{
		//Me->ActiveComponents();
	}
	else
	{
		/*
			이곳에 기존 플레이어인 LobbyPlayer로 돌아가는 로직이 필요함.
		*/
		Destroy();

	}
}


bool ASG_ServerManager::RecvAll(TArray<uint8>& OutData, uint32 Length, int32& BytesReceived)
{	
	bool bSuccess = true;
	if (Length >= 3000)
	{
		UE_LOG(LogTemp, Warning, TEXT("error: !!Length Over!!"));
		Length = 10000;
		bSuccess = false;
	}
	OutData.Empty();
	OutData.SetNumUninitialized(Length);

	if (bSuccess)
		ClientSocket->Recv(OutData.GetData(), OutData.Num(), BytesReceived, ESocketReceiveFlags::WaitAll);
	else
		ClientSocket->Recv(OutData.GetData(), OutData.Num(), BytesReceived, ESocketReceiveFlags::None);

	return bSuccess;
	//UE_LOG(LogTemp, Warning, TEXT("BytesReceived : %d"), BytesReceived);
}

void ASG_ServerManager::ReceiveData()
{
	//UE_LOG(LogTemp, Warning, TEXT("[ReceiveData]"));
	uint32 PackedLength = 0;
	if (ClientSocket && ClientSocket->GetConnectionState() != SCS_Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("error: Socket is not Connected or does not exist."));
		return;
	}

	uint32 headerPendingData = 0;
	if (ClientSocket->HasPendingData(headerPendingData) && headerPendingData < 4)
	{
		UE_LOG(LogTemp, Warning, TEXT("error: headetPendingData < 4"));
		return;
	}

	// 데이터 수신
	int32 BytesRead = 0;
	if (!ClientSocket->Recv(reinterpret_cast<uint8*>(&PackedLength), sizeof(uint32), BytesRead, ESocketReceiveFlags::WaitAll))
	{
		UE_LOG(LogTemp, Warning, TEXT("error: Recv Length Information Fail"));
		return;
	}

	DataLength = FGenericPlatformProperties::IsLittleEndian() ? BYTESWAP_ORDER32(PackedLength) : PackedLength;

	//UE_LOG(LogTemp, Warning, TEXT("[ReceiveData] DataLength : %u"), DataLength);
	uint32 bodyPendingData;

	// 버퍼에 body 데이터가 있다면
	if (ClientSocket->HasPendingData(bodyPendingData))
	{
		// 전체 Json이 버퍼에 존재한다면
		int32 bodyByteReceived = 0;
		if (bodyPendingData >= DataLength)
		{
			TArray<uint8> ReceivedData;
			RecvAll(ReceivedData, DataLength, bodyByteReceived);
			bReceiveSuccess = true;
			ReceivedJson = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
			ReceivedJson = ReceivedJson.Mid(0, DataLength);
			//UE_LOG(LogTemp, Warning, TEXT("DataLength: %u"), DataLength);
			if (USG_JsonUtilityLibrary::MediaPipeJsonParse(ReceivedJson, Me, Me->Landmarks, Me->TargetJointLocations))
			{
				Me->SetJointPosition(Me->TargetJointLocations);
				Me->TargetJointLocations.Empty();

			}
			return;
		}
		//UE_LOG(LogTemp, Warning, TEXT("bodyPendingData < DataLength"));
	}
	//UE_LOG(LogTemp, Warning, TEXT("Buffer no Have pending data"));
	bReceiveSuccess = false;
	TargetDataLength = DataLength;
	CurDataLength = 0;
	ReceivedJson = "";
	ReceiveRestData();
}

void ASG_ServerManager::ReceiveRestData()
{
	uint32 bodyPendingData;
	if (!ClientSocket->HasPendingData(bodyPendingData)) return;

	if (!(TargetDataLength >= MIN_JSON_SIZE && TargetDataLength <= MAX_JSON_SIZE))
	{
		UE_LOG(LogTemp, Warning, TEXT("error: TargetDataLength %u not valid"), TargetDataLength);
		/*TArray<uint8> ReceivedData;
		int32 datas;
		RecvAll(ReceivedData, TargetDataLength, datas);*/
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("CurDataLength: %u"), CurDataLength);
	uint32 ReadDataLength = FMath::Min(bodyPendingData, TargetDataLength - CurDataLength);
	TArray<uint8> ReceivedData;
	ReceivedData.Empty();
	ReceivedData.SetNumUninitialized(ReadDataLength);
	int32 bodyByteReceived = 0;
	ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), bodyByteReceived);
	CurDataLength += bodyByteReceived;
	ReceivedJson += FString(UTF8_TO_TCHAR(ReceivedData.GetData())).Mid(0, bodyByteReceived);
	//UE_LOG(LogTemp, Warning, TEXT("[ReceiveRestData] : CurDataLength: %u, TargetDataLength: %u, bodyPendingData: %u, Json: %s"), CurDataLength, TargetDataLength, bodyPendingData, *ReceivedJson);
	//ReceivedJson = ReceivedJson.Mid(0, CurDataLength);
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *ReceivedJson);

	if (CurDataLength == TargetDataLength)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CurDataLength == TargetDataLength"));
		if (USG_JsonUtilityLibrary::MediaPipeJsonParse(ReceivedJson, Me, Me->Landmarks, Me->TargetJointLocations))
		{
			Me->SetJointPosition(Me->TargetJointLocations);
			Me->TargetJointLocations.Empty();
		}
		ReceivedJson = "";
		CurDataLength = 0;
		TargetDataLength = 0;
		bReceiveSuccess = true;
	}
}


void ASG_ServerManager::Disconnect()
{
	if (ClientSocket)
	{
		ClientSocket->Close();
		ClientSocket = nullptr;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Disconnect Server.")));
	}
}