// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_HttpNetActor.h"
#include "HttpModule.h"
#include "Sound/SoundWaveProcedural.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACJS_HttpNetActor::ACJS_HttpNetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
// Called when the game starts or when spawned
void ACJS_HttpNetActor::BeginPlay()
{
	Super::BeginPlay();
}
// Called every frame
void ACJS_HttpNetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ACJS_HttpNetActor::ReqPostWaveFile(FString url, FString wavFilePath)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_HttpNetActor::ReqPostWaveFile()"));
	TArray<uint8> FileData;
	//if (FFileHelper::LoadFileToArray(FileData, TEXT("C:/Users/onlys/Desktop/sound/TestVoiceSound.wav")))
	if (FFileHelper::LoadFileToArray(FileData, *wavFilePath))
	{
		FHttpModule& HttpModule = FHttpModule::Get();
		TSharedRef<IHttpRequest> Request = HttpModule.CreateRequest();

		Request->SetURL(url);
		Request->SetVerb(TEXT("POST"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
		Request->SetContent(FileData);

		Request->OnProcessRequestComplete().BindUObject(this, &ACJS_HttpNetActor::OnResPostWaveFile);
		Request->ProcessRequest();
		UE_LOG(LogTemp, Warning, TEXT("ACJS_HttpNetActor::ReqPostWaveFile()::Request->ProcessRequest()"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load wave file."));
	}
}

void ACJS_HttpNetActor::OnResPostWaveFile(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_HttpNetActor::OnResPostWaveFile()"));

	if (bConnectedSuccessfully && Response.IsValid())
	{
		int32 ResponseCode = Response->GetResponseCode();
		UE_LOG(LogTemp, Warning, TEXT("OnResPostWaveFile OK... %d"), ResponseCode);

		// 응답 코드가 200(성공)인 경우에만 처리
		if (ResponseCode == 200)
		{
			TArray<uint8> WaveData = Response->GetContent();
			if (WaveData.Num() > 0)
			{
				// 파일 경로 설정
				//FString SavePath = FPaths::Combine(TEXT("C:/Users/onlys/Desktop/sound/Response"), TEXT("DownloadedWave.wav"));
				FString SavePath = GenerateSavePath();

				// 파일 저장
				if (FFileHelper::SaveArrayToFile(WaveData, *SavePath))
				{
					UE_LOG(LogTemp, Log, TEXT("Wave file saved successfully to %s"), *SavePath);
					// 저장된 파일로부터 사운드 재생
					PlaySoundFromFile(SavePath);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to save wave file."));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Received empty wave data."));
			}
		}
		else
		{
			// 응답 코드가 200이 아닌 경우 경고 출력
			UE_LOG(LogTemp, Warning, TEXT("Response not OK, received code: %d"), ResponseCode);
		}
	}
	else
	{
		// 연결 실패 또는 응답이 null인 경우 처리
		if (!bConnectedSuccessfully)
		{
			UE_LOG(LogTemp, Warning, TEXT("Connection to the server failed."));
		}

		if (!Response.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid response received."));
		}
	}
}

void ACJS_HttpNetActor::PlaySoundFromFile(const FString& FilePath)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_HttpNetActor::PlaySoundFromFile()"));

	// 파일에서 바이너리 데이터를 로드
	TArray<uint8> FileData;
	if (FFileHelper::LoadFileToArray(FileData, *FilePath))
	{
		// USoundWaveProcedural 객체 생성
		USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>();

		// 오디오 데이터를 SoundWaveProcedural에 큐에 추가
		if (SoundWave)
		{
			SoundWave->QueueAudio(FileData.GetData(), FileData.Num());

			// 재생 설정
			SoundWave->SetSampleRate(12000); // 샘플레이트 설정, 실제 WAV 파일 정보에 따라 변경 필요
			SoundWave->NumChannels = 2; // 채널 수 설정, 실제 WAV 파일 정보에 따라 변경 필요

			// 재생
			//UGameplayStatics::PlaySoundAtLocation(this, SoundWave, GetActorLocation());
			UGameplayStatics::PlaySound2D(this, SoundWave);
			UE_LOG(LogTemp, Warning, TEXT("Playing sound from file: %s"), *FilePath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create USoundWaveProcedural object."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
	}
}

FString ACJS_HttpNetActor::GenerateSavePath()
{
	// 현재 날짜와 시간을 가져옵니다.
	FDateTime Now = FDateTime::Now();

	// 날짜와 시간을 "YYYYMMDD_HHMMSS" 형식의 문자열로 포맷팅합니다.
	FString DateTimeString = Now.ToString(TEXT("%Y%m%d_%H%M%S"));

	// 파일 이름을 생성합니다.
	FString FileName = FString::Printf(TEXT("DownloadedWave_%s.wav"), *DateTimeString);

	// 최종 파일 경로를 생성합니다.
	//FString ReceiveFileSavePath = FPaths::Combine(TEXT("C:/Users/Sunny/Desktop/sound/Response"), *FileName);
	FString ReceiveFileSavePath = FPaths::Combine(TEXT("C:\\Users\\Admin\\IMA_Sound\\Response"), *FileName);
	//ReceiveFileSavePath = FPaths::Combine(TEXT("C:/Users/Sunny/Desktop/sound/Response/"), *FileName); 

	// SavePath를 사용하는 로직
	UE_LOG(LogTemp, Log, TEXT("Generated file path: %s"), *ReceiveFileSavePath);

	return ReceiveFileSavePath;
}

