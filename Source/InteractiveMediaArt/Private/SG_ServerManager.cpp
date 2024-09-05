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
#include "SG_Player.h"
// Sets default values
const uint32 MIN_JSON_SIZE = 1500;
const uint32 MAX_JSON_SIZE = 3000;
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
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("IPString: %s, Port: %d"), *ServerIP, ServerPort));

	RunPythonScript(PyConnectServer);
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, [&]()
		{
			CreateClient();
		}, 3.0f, false);


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

// Called every frame
void ASG_ServerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//TestReceiveData();
	if (ServerProcHandle.IsValid() && ClientSocket)
	{
		if (bReceiveSuccess) ReceiveData();
		else ReceiveRestData();

		uint32 pendingData = 0;
		ClientSocket->HasPendingData(pendingData);
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("%u"), pendingData));

	}
}

void ASG_ServerManager::RunPythonScript(const FString& Path)
{
	FString scriptPath = PyDeafultPath + Path;
	FString pythonExePath = TEXT("C:\\Users\\Admin\\AppData\\Local\\Microsoft\\WindowsApps\\PythonSoftwareFoundation.Python.3.10_qbz5n2kfra8p0\\python.exe");
	bool bPythonExe = FPaths::FileExists(pythonExePath);
	bool bScript = FPaths::FileExists(scriptPath);
	if (bPythonExe && bScript)
	{
		FString params = FString::Printf(TEXT("\"%s\""), *scriptPath);
		ServerProcHandle = FPlatformProcess::CreateProc(*pythonExePath,
			*params,
			false,	// bLaunchDetached: false 로 설정하여 CMD 창을 띄움
			false,
			false,
			&ServerPID,
			0,
			nullptr,
			nullptr
		);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("ServerProcHandle is valid = %d"), ServerProcHandle.IsValid()));
	}
	else
	{
		if (!bPythonExe) UE_LOG(LogTemp, Warning, TEXT("%s is not exist"), *pythonExePath);
		if (!bScript) UE_LOG(LogTemp, Warning, TEXT("%s is not exist"), *scriptPath);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Python executable or script not found")));
	}
}

void ASG_ServerManager::CreateClient()
{
	// 클라이언트 소켓 생성
	ClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("TCP Client"), false);
	ServerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	// 서버 IP와 포트 설정
	FIPv4Address ip;

	// 서버의 IP 주소 (예: LocalHost)
	FIPv4Address::Parse(ServerIP, ip);
	// IP 주소 설정
	ServerAddr->SetIp(ip.Value);
	// 서버 포트
	ServerAddr->SetPort(ServerPort);

	FString connectionStr = ClientSocket->Connect(*ServerAddr) ? TEXT("서버 연결 완료") : TEXT("서버 연결 실패");
	GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Green, FString::Printf(TEXT("%s"), *connectionStr));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *connectionStr);
}


bool ASG_ServerManager::RecvAll(TArray<uint8>& OutData, uint32 Length, int32& BytesReceived)
{
	bool bSuccess = true;
	if (Length >= 3000)
	{
		UE_LOG(LogTemp, Warning, TEXT("!!Length Over!!"));
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
	uint32 PackedLength = 0;
	if (ClientSocket && ClientSocket->GetConnectionState() != SCS_Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("Socket is not Connected or does not exist.")));
		return;
	}

	uint32 headerPendingData = 0;
	if (ClientSocket->HasPendingData(headerPendingData) && headerPendingData < 4)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("message")));
		return;
	}

	// 데이터 수신
	int32 BytesRead = 0;
	if (!ClientSocket->Recv(reinterpret_cast<uint8*>(&PackedLength), sizeof(uint32), BytesRead, ESocketReceiveFlags::WaitAll))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Received Fail")));
		return;
	}

	DataLength = FGenericPlatformProperties::IsLittleEndian() ? BYTESWAP_ORDER32(PackedLength) : PackedLength;

	UE_LOG(LogTemp, Warning, TEXT("[ReceiveData] DataLength : %u"), DataLength);
	uint32 bodyPendingData;

	// 4바이트의 길이 데이터를 읽고, 펜딩 데이터가 있다면
	if (ClientSocket->HasPendingData(bodyPendingData))
	{
		// 전체 Json이 버퍼에 존재한다면
		int32 bodyByteReceived = 0;
		if (bodyPendingData >= DataLength)
		{
			TArray<uint8> ReceivedData;
			if (RecvAll(ReceivedData, DataLength, bodyByteReceived))
			{
				UE_LOG(LogTemp, Warning, TEXT("RecvAll Success"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Buffer Clear"));
			}
			bReceiveSuccess = true;
			ReceivedJson = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
			ReceivedJson = ReceivedJson.Mid(0, DataLength);
			UE_LOG(LogTemp, Warning, TEXT("DataLength: %u"), DataLength);
			if (USG_JsonUtilityLibrary::MediaPipeJsonParse(ReceivedJson, Me->Landmarks, Me->TargetJointLocations))
			{
				Me->SetJointPosition();
			}
		}
		// 아직 Json이 도착하기 전이라면
		else if (bodyPendingData < DataLength)
		{
			bReceiveSuccess = false;
			TargetDataLength = DataLength;
			CurDataLength = 0;
			ReceiveRestData(bodyPendingData);
		}
	}
	else
	{
		bReceiveSuccess = false;
		TargetDataLength = DataLength;
		CurDataLength = 0;
		ReceiveRestData(bodyPendingData);
	}

}

void ASG_ServerManager::ReceiveRestData(uint32 bodyPendingData)
{
	UE_LOG(LogTemp, Warning, TEXT("ReceiveRestData"));
	if (bodyPendingData == 0)
	{
		if (!ClientSocket->HasPendingData(bodyPendingData)) return;
	}

	if (!(TargetDataLength >= MIN_JSON_SIZE && TargetDataLength <= MAX_JSON_SIZE))
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetDataLength %u not valid"), TargetDataLength);
		/*TArray<uint8> ReceivedData;
		int32 datas;
		RecvAll(ReceivedData, TargetDataLength, datas);*/
		return;
	}
	uint32 ReadDataLength = FMath::Min(bodyPendingData, TargetDataLength - CurDataLength);
	TArray<uint8> ReceivedData;
	ReceivedData.Empty();
	ReceivedData.SetNumUninitialized(ReadDataLength);
	int32 bodyByteReceived = 0;
	ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), bodyByteReceived);
	CurDataLength += bodyByteReceived;
	ReceivedJson += FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
	ReceivedJson = ReceivedJson.Mid(0, CurDataLength);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ReceivedJson);

	if (CurDataLength == TargetDataLength)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurDataLength: %u"), CurDataLength);
		USG_JsonUtilityLibrary::MediaPipeJsonParse(ReceivedJson, Me->Landmarks, Me->TargetJointLocations);

		Me->SetJointPosition();
		ReceivedJson = "";
		CurDataLength = 0;
		bReceiveSuccess = true;
		TargetDataLength = 0;

	}
	else
	{

	}
}

void ASG_ServerManager::TestReceiveData()
{
	const uint32 DATA_SIZE = 2048;
	if (ClientSocket && ClientSocket->GetConnectionState() == SCS_Connected)
	{
		// 더 이상 수신할 데이터가 없으면 루프 종료
		//while(ClientSocket->HasPendingData(JSON_SIZE))
		uint32 pendingData = DATA_SIZE;
		if (ClientSocket->HasPendingData(pendingData))
		{
			TArray<uint8> ReceivedData;
			ReceivedData.SetNumUninitialized(pendingData);
			// 데이터 수신
			int32 BytesRead = 0;
			if (ClientSocket->Recv(ReceivedData.GetData(), pendingData, BytesRead))
			{
				if (BytesRead > DATA_SIZE)
				{
					//UE_LOG(LogTemp, Warning, TEXT(""))
					ReceivedJson = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
					//UE_LOG(LogTemp, Warning, TEXT("ReceivedJson : %s"), *ReceivedJson);

					if (USG_JsonUtilityLibrary::MediaPipeJsonParse(ReceivedJson, Me->Landmarks, Me->TargetJointLocations))
					{
						Me->SetJointPosition();
					}
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("%d is not exist"), BytesRead));
				}
			}
			{
				GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("Failed to Reveive Data")));
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("Socket is not Connected or does not exist.")));
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
	FString testJson = TEXT("{\"NOSE\": {\"x\": 0.5, \"y\": 0.1}, \"LEFT_SHOULDER\": {\"x\": 1.2, \"y\": 3.0}}");
	USG_JsonUtilityLibrary::MediaPipeJsonParse(testJson, Me->Landmarks, Me->TargetJointLocations);
	GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Green, FString::Printf(TEXT("%d"), Me->TargetJointLocations.Num()));
	for (int i = 0; i < Me->TargetJointLocations.Num(); i++)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Black, FString::Printf(TEXT("LandMark: %s, %f, %f"), *Me->Landmarks[i], Me->TargetJointLocations[i].Key, Me->TargetJointLocations[i].Value));
	}
}

void ASG_ServerManager::testMakeCoordinates()
{
	int8 BonesSize = 23;
	for (int8 i = 0; i < BonesSize; i++)
	{
		float bound = 3;
		float x = FMath::RandRange(-bound, bound);
		float y = FMath::RandRange(-bound, bound);
		Me->TargetJointLocations.Add({ x, y });
	}

	Me->SetJointPosition();
}
