// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_CountPlayerUIActor.h"
#include "CJS/CJS_CountPlayerUI.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ACJS_CountPlayerUIActor::ACJS_CountPlayerUIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 서버 연동
	bReplicates = true;
	NetUpdateFrequency = 10; // 1초에 10번 확인 (바뀐게 있으면 서버에서 강제 동기화)

}

// Called when the game starts or when spawned
void ACJS_CountPlayerUIActor::BeginPlay()
{
	Super::BeginPlay();

	if (WBP_CountPlayerUI) // WBP_CountPlayerUI가 유효한지 먼저 확인
	{
		CountPlayerUI = CreateWidget<UCJS_CountPlayerUI>(GetWorld(), WBP_CountPlayerUI);
		UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::BeginPlay()::WBP_CountPlayerUI created"))

			if (CountPlayerUI) // CreateWidget으로 올바르게 생성되었는지 확인
			{
				UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::BeginPlay()::CountPlayerUI created"))
				CountPlayerUI->AddToViewport();
				CountPlayerUI->SetVisibility(ESlateVisibility::Hidden);
				InitCountPlayerUiActor();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("CountPlayerUI could not be created!"));
			}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WBP_CountPlayerUI is not set!"));
	}
}

// Called every frame
void ACJS_CountPlayerUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACJS_CountPlayerUIActor::AddPlayerNum()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::AddPlayerNum()"));
	CurPlayer += 1;
	CountPlayerUI->ShowPlayerNum(CurPlayer, MaxPlayer);
}

void ACJS_CountPlayerUIActor::InitCountPlayerUiActor()
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_CountPlayerUI::InitCountPlayerUiActor()"));
	CurPlayer = 0;
	CountPlayerUI->InitPlayerNum(CurPlayer, MaxPlayer);
}




// 네트워크 동기화 하는 부분
void ACJS_CountPlayerUIActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// CurPlayer가 네트워크 동기화가 되도록 설정
	DOREPLIFETIME(ACJS_CountPlayerUIActor, CurPlayer);
}

