// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_ServerManager.h"
#include "Engine/Engine.h"
#include "Misc/Paths.h"
#include "Misc/ConfigCacheIni.h"
#include "HAL/PlatformProcess.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Sockets.h"
#include "Windows/WindowsPlatformProcess.h"
#include "Kismet/GameplayStatics.h"

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
	//RunPythonScript(TEXT("C:\\Work\\InteractiveMediaArt\\Content\\Python\\Cat_HelloWorld.py"));
	//RunPythonScript(TEXT("Cat_HelloWorld.py"));
	//RunPythonScript(TEXT("while.py"));
	RunPythonScript(TEXT("Cat_HelloWorld.py"));
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
}

// Called every frame
void ASG_ServerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("%d"), ServerProcHandle.IsValid()));
}

void ASG_ServerManager::RunPythonScript(const FString& Path)
{
	FString scriptPath = PyDeafultPath + Path;
	FString pythonExePath = TEXT("C:\\Python\\python.exe");

	if (FPaths::FileExists(pythonExePath) && FPaths::FileExists(scriptPath))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("File Exists")));
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

		if (ServerProcHandle.IsValid())
		{
			GEngine->AddOnScreenDebugMessage(-1, 300.f, FColor::Green, FString::Printf(TEXT("CreateProc %u"), ServerPID));
			GEngine->AddOnScreenDebugMessage(-1, 3000.f, FColor::Green, FString::Printf(TEXT("%s"), *params));
		}
		//GEngine->AddOnScreenDebugMessage(-1, 3000.f, FColor::Green, TEXT("C:\\Work\\InteractiveMediaArt\\Content\\Scripts\\Cat_HelloWorld.py"));
		
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
	if (ClientSocket && ClientSocket->GetConnectionState() == SCS_Connected)
	{
		uint32 size;
		while(ClientSocket->HasPendingData(size))
		{
			TArray<uint8> ReceivedData;
			ReceivedData.SetNumUninitialized(FMath::Min(size, 1024u));

			// 데이터 수신
			int32 BytesRead = 0;
			if (ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead))
			{
				if (BytesRead > 0)
				{
					FString ReceivedString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
					if (ReceivedString.Contains(TEXT("1")))
					{
						SendDataToPlayer(1);
					}
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
