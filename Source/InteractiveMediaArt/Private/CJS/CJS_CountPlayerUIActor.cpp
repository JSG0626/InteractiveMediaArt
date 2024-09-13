// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_CountPlayerUIActor.h"
#include "CJS/CJS_CountPlayerUI.h"
#include "Net/UnrealNetwork.h"
#include "CJS/CJS_LobbyPlayer.h"


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

	// 서버 권한이 있는지 확인
	auto MyOwner = GetOwner<ACJS_LobbyPlayer>();
	if (  MyOwner && MyOwner->IsLocallyControlled())
	{
		// UI 위젯 초기화
		if (WBP_CountPlayerUI) // WBP_CountPlayerUI가 유효한지 먼저 확인
		{
			CountPlayerUI = CreateWidget<UCJS_CountPlayerUI>(GetWorld(), WBP_CountPlayerUI);
			UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::BeginPlay()::WBP_CountPlayerUI created"))

				if (CountPlayerUI) // CreateWidget으로 올바르게 생성되었는지 확인
				{
					UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::BeginPlay()::CountPlayerUI created"))
						CountPlayerUI->AddToViewport();
					CountPlayerUI->SetVisibility(ESlateVisibility::Hidden);
					InitCountPlayerUiActor(CurPlayer);
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

		// 첫 번째 플레이어의 Pawn을 Owner로 설정
		//APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		//if (MyOwner)
		//{
		//	SetOwner(PlayerPawn);  // Owner 설정
		//	UE_LOG(LogTemp, Warning, TEXT("Owner has been set to the PlayerPawn."));
		//}
		//else
		//{
		//	UE_LOG(LogTemp, Error, TEXT("Failed to set Owner. PlayerPawn is null."));
		//}
	}

	
}

// Called every frame
void ACJS_CountPlayerUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintNetLog();
	//FindOwner();
}

void ACJS_CountPlayerUIActor::InitCountPlayerUiActor(int32 curPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_CountPlayerUI::InitCountPlayerUiActor()"));
	/*SetCurPlayerNum(0);
	CountPlayerUI->InitPlayerNum(CurPlayer, MaxPlayer);*/

	UpdatePlayerNum(curPlayer);  // Start with 0 players
}

void ACJS_CountPlayerUIActor::ServerRPC_AddPlayerNum_Implementation(int32 AddNum)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::ServerRPC_AddPlayerNum_Implementation()"));
	if (HasAuthority())
	{
		//SetOwner(GetWorld()->GetFirstPlayerController()->GetPawn());

		// Update the player count on the server
		CurPlayer += AddNum;
		if (CurPlayer > MaxPlayer)
		{
			CurPlayer = MaxPlayer;
		}

		// Sync the value with clients
		//OnRep_CurPlayer();

		 // 클라이언트에게 동기화
		MulticastRPC_UpdatePlayerNum(CurPlayer);
	}
}
bool ACJS_CountPlayerUIActor::ServerRPC_AddPlayerNum_Validate(int32 AddNum)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::ServerRPC_AddPlayerNum_Validate()"));
	return true;  // 기본적으로 유효성 검사는 항상 true로 설정
}

void ACJS_CountPlayerUIActor::OnRep_CurPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::OnRep_CurPlayer()"));

	// 클라이언트에서 해당 함수가 호출되었음을 확인
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_CurPlayer() called on client without authority."));
	}

	UpdatePlayerNum(CurPlayer);
}


// Multicast 함수로 모든 클라이언트에 값 전달
void ACJS_CountPlayerUIActor::MulticastRPC_UpdatePlayerNum_Implementation(int32 NewPlayerNum)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::MulticastRPC_UpdatePlayerNum_Implementation()"));

	// 서버와 클라이언트를 구분하여 출력
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("This is the server broadcasting the player number update."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("This is a client receiving the updated player number."));
	}

	UpdatePlayerNum(NewPlayerNum);
}


void ACJS_CountPlayerUIActor::AddPlayerNum()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::AddPlayerNum()"));
	SetCurPlayerNum(1);
	//CountPlayerUI->ShowPlayerNum(CurPlayer, MaxPlayer);
}
int32 ACJS_CountPlayerUIActor::GetCurPlayerNum()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::GetCurPlayerNum()"));
	return CurPlayer;
}
void ACJS_CountPlayerUIActor::SetCurPlayerNum(int32 addNum)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::SetCurPlayerNum()"));
	CurPlayer += addNum;
	ServerRPC_AddPlayerNum(CurPlayer);
}

void ACJS_CountPlayerUIActor::UpdatePlayerNum(int32 NewPlayerNum)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::UpdatePlayerNum()"));
	if (CountPlayerUI)
	{
		CountPlayerUI->ShowPlayerNum(NewPlayerNum, MaxPlayer);
	}
}


// 네트워크 동기화 하는 부분
void ACJS_CountPlayerUIActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// CurPlayer가 네트워크 동기화가 되도록 설정
	DOREPLIFETIME(ACJS_CountPlayerUIActor, CurPlayer);
}


void ACJS_CountPlayerUIActor::PrintNetLog()
{
	const FString conStr = GetNetConnection() ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	const FString ownerName = GetOwner() ? GetOwner()->GetName() : TEXT("No Owner");

	//FString logStr = FString::Printf(TEXT("Connection : %s\nOwner Name : %s\nLocal Role : %s\nRemote Role : %s"), *conStr, *ownerName, *LOCALROLE, *REMOTEROLE);
	//FVector loc = GetActorLocation() + GetActorUpVector() * 30;
	//DrawDebugString(GetWorld(), loc, logStr, nullptr, FColor::Yellow, 0, true, 1.f);
}
void ACJS_CountPlayerUIActor::FindOwner()
{
	//if (HasAuthority())
	//{
	//	AActor* newOwner = nullptr;

	//	// TActorIterator에서 명시적으로 ACJS_LobbyPlayer 타입을 지정
	//	for (TActorIterator<ACJS_LobbyPlayer> it(GetWorld()); it; ++it)
	//	{
	//		AActor* otherActor = *it;  // 반복자가 가리키는 액터를 가져옴

	//		// 필요한 로직을 여기에 추가할 수 있음
	//		newOwner = otherActor;  // 우선 소유자를 해당 액터로 설정
	//	}

	//	// Owner가 변경된 경우에만 설정
	//	if (newOwner != GetOwner())
	//	{
	//		SetOwner(newOwner);
	//		UE_LOG(LogTemp, Warning, TEXT("New owner set: %s"), *newOwner->GetName());
	//	}
	//}

	//DrawDebugSphere(GetWorld(), GetActorLocation(), SearchDistance, 16, FColor::Cyan, false, 0, 0, 1);
}




//bool ACJS_CountPlayerUIActor::ServerRPC_AddPlayerNum_Validate(int32 AddNum)
//{
//	return true;
//}


//
//void ACJS_CountPlayerUIActor::ServerRPC_ChangePlayerNum_Implementation(const int32 playerNum)
//{
//	SetCurPlayerNum(playerNum);
//	MulticastRPC_ChangePlayerNum(playerNum);
//}
//
//void ACJS_CountPlayerUIActor::MulticastRPC_ChangePlayerNum_Implementation(const int32 playerNum)
//{
//	CountPlayerUI->ShowPlayerNum(GetCurPlayerNum(), MaxPlayer);
//}



