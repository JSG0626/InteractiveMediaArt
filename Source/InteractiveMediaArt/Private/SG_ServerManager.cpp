// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_ServerManager.h"
#include "Engine/Engine.h"
#include "Misc/Paths.h"
#include "Misc/ConfigCacheIni.h"
#include "HAL/PlatformProcess.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Sockets.h"
#include "SocketSubSystem.h"
#include "Windows/WindowsPlatformProcess.h"
#include "Kismet/GameplayStatics.h"

#include "SG_JsonUtilityLibrary.h"
#include "SG_Player.h"

// Sets default values
ASG_ServerManager::ASG_ServerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PyDeafultPath = FPaths::ProjectContentDir() + TEXT("Scripts/");
}

// Called when the game starts or when spawned
void ASG_ServerManager::BeginPlay()
{
	Super::BeginPlay();
	RunPythonScript(PyConnectServer);

	
}

void ASG_ServerManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("EndPlay"));

	if (FPlatformProcess::IsProcRunning(ServerProcHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("서버를 닫습니다."));
		FPlatformProcess::TerminateProc(ServerProcHandle);
		FPlatformProcess::CloseProc(ServerProcHandle);
	}

	Disconnect();
}

// Called every frame
void ASG_ServerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASG_ServerManager::RunPythonScript(const FString& Path)
{
	FString scriptPath = PyDeafultPath + Path;
	FString pythonExePath = TEXT("C:\\Python\\python.exe");

	if (FPaths::FileExists(pythonExePath) && FPaths::FileExists(scriptPath))
	{
		FString params = FString::Printf(TEXT("\"%s\""), *scriptPath);
		ServerProcHandle = FPlatformProcess::CreateProc(*pythonExePath,
		*params,
		false,	// bLaunchDetached: false 로 설정하여 CMD 창을 띄움
		true,
		true,
		&ServerPID,
		0,
		nullptr,
		nullptr
		);
		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Python executable or script not found")));
	}
}

void ASG_ServerManager::CreateClient(FString IPString, int32 Port)
{
	// IP가 설정되지 않은 상태라면
	if (IPString == TEXT(""))
	{
		IPString = ServerIP;
	}
	if (Port == 0)
	{
		Port = ServerPort ;
	}

	// 클라이언트 소켓 생성
	ClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("TCP Client"), false);
	ServerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	// 서버 IP와 포트 설정
	FIPv4Address ip;
	// 서버의 IP 주소 (예: LocalHost)
	FIPv4Address::Parse(IPString, ip);
	// IP 주소 설정
	ServerAddr->SetIp(ip.Value);
	// 서버 포트
	ServerAddr->SetPort(Port);

	FString connectionStr = ClientSocket->Connect(*ServerAddr) ? TEXT("서버 연결 완료") : TEXT("서버 연결 실패");

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s"), *connectionStr));
}

void ASG_ServerManager::ReceiveData()
{
	uint32 JSON_SIZE = 1024;
	if (ClientSocket && ClientSocket->GetConnectionState() == SCS_Connected)
	{
		// 더 이상 수신할 데이터가 없으면 루프 종료
		while(ClientSocket->HasPendingData(JSON_SIZE))
		{
			TArray<uint8> ReceivedData;
			ReceivedData.SetNumUninitialized(FMath::Min(JSON_SIZE, 1024u));

			// 데이터 수신
			int32 BytesRead = 0;
			if (ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead))
			{
				if (BytesRead > 0)
				{
					FString ReceivedString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())), BytesRead);
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("ReceivedString: %s"), *ReceivedString));
					
					USG_JsonUtilityLibrary::MediaPipeJsonParse(ReceivedString, Me->Landmarks, Me->TargetJointLocations);
					Me->SetJointPosition();
					
					/*for(int i = 0; i < Me->TargetJointLocations.Num(); i++)
						GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%f %f"), Me->TargetJointLocations[i].Key, Me->TargetJointLocations[i].Value));*/
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("No Data Received")));
				}
			}
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Failed to Reveive Data")));
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Socket is not Connected or does not exist.")));
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

void ASG_ServerManager::SendDataToPlayer(int32 data)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("SendDataToPlayer")));
}

void ASG_ServerManager::testJsonParse()
{
	/*
	FString testJson = TEXT("{\"NOSE\": {\"x\": 0.5, \"y\": 0.1}, \"LEFT_SHOULDER\": {\"x\": 1.2, \"y\": 3.0}}");
	USG_JsonUtilityLibrary::MediaPipeJsonParse(testJson, Me->Landmarks, Me->TargetJointLocations);
	GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Green, FString::Printf(TEXT("%d"), Me->TargetJointLocations.Num()));
	for (int i = 0; i < Me->TargetJointLocations.Num(); i++)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Black, FString::Printf(TEXT("LandMark: %s, %f, %f"), *Me->Landmarks[i], Me->TargetJointLocations[i].Key, TargetJointLocations[i].Value));
	}*/
}

void ASG_ServerManager::testMakeCoordinates()
{
	int8 BonesSize = 23;
	for (int8 i = 0; i < BonesSize; i++)
	{
		float bound = 3;
		float x = FMath::RandRange(-bound, bound);
		float y = FMath::RandRange(-bound, bound);
		Me->TargetJointLocations.Add({x, y});
	}

	Me->SetJointPosition();
}
