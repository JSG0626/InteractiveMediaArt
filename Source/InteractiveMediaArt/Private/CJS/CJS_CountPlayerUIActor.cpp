// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_CountPlayerUIActor.h"
#include "CJS/CJS_CountPlayerUI.h"
#include "CJS/CJS_LobbyPlayer.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "../InteractiveMediaArt.h"
#include <SG_Art1_Manager.h>

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

	// 현재 Actor의 역할 출력
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("CountPlayerUIActor has authority on %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CountPlayerUIActor is a client instance on %s"), *GetName());
	}

	// 서버 권한이 있는지 확인
	//auto MyOwner = GetOwner<ACJS_LobbyPlayer>();
	//if (MyOwner && MyOwner->IsLocallyControlled())
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


	Art1_Multi_TargetCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector(2470, -2090, 750), FRotator(0, -90, 0));    // (X=2470.000000,Y=-2090.000000,Z=750.000000)
	auto* Art1_Multi_TargetCameraComp = Art1_Multi_TargetCamera->GetCameraComponent();
	Art1_Multi_TargetCameraComp->ProjectionMode = ECameraProjectionMode::Orthographic;
	Art1_Multi_TargetCameraComp->OrthoWidth = 3200.f;
	Art1_Multi1_TargetTransform = FTransform(FRotator(0, -90, 0), FVector(1650, -3580, 200));  // 왼쪽  
	Art1_Multi2_TargetTransform = FTransform(FRotator(0, -90, 0), FVector(3220, -3580, 200));  // 오른쪽 (X=3217.214946,Y=-3580.000000,Z=210.000014)

	// 이동할 위치와 카메라 설정
	TargetTransforms.Add(Art1_Multi1_TargetTransform);
	TargetTransforms.Add(Art1_Multi2_TargetTransform);
}

// Called every frame
void ACJS_CountPlayerUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//PrintNetLog();
	//FindOwner();


	// Replication 상태 로그 출력
	if (HasAuthority())
	{
		UE_LOG(LogTemp, VeryVerbose, TEXT("CountPlayerUIActor has authority on %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, VeryVerbose, TEXT("CountPlayerUIActor is a client instance on %s"), *GetName());
	}
}


void ACJS_CountPlayerUIActor::InitCountPlayerUiActor(int32 curPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_CountPlayerUI::InitCountPlayerUiActor()"));

	UpdatePlayerNum(curPlayer);  // Start with 0 players
}


void ACJS_CountPlayerUIActor::ServerRPC_RemovePlayerNum_Implementation(ACJS_LobbyPlayer* Player, int32 RemoveNum)
{
	UE_LOG(LogTemp, Warning, TEXT("UCJS_CountPlayerUI::ServerRPC_RemovePlayerNum_Implementation()"));
	if (Player)
	{
		ClickedPlayers.Remove(Player);
		CurPlayer -= RemoveNum;

		if (CurPlayer <= 0)
		{
			CurPlayer = 0;
		}

		// 플레이어 제거 후 ClickedPlayers와 CurPlayer 값 출력
		UE_LOG(LogTemp, Warning, TEXT("After removing player:"));
		UE_LOG(LogTemp, Warning, TEXT("ClickedPlayers.Num() = %d"), ClickedPlayers.Num());
		UE_LOG(LogTemp, Warning, TEXT("CurPlayer = %d"), CurPlayer);

		// ClickedPlayers 배열의 내용 출력
		for (int32 i = 0; i < ClickedPlayers.Num(); ++i)
		{
			if (ClickedPlayers[i])
			{
				FString PlayerName = ClickedPlayers[i]->GetName();
				UE_LOG(LogTemp, Warning, TEXT("ClickedPlayers[%d]: %s"), i, *PlayerName);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ClickedPlayers[%d]: nullptr"), i);
			}
		}

		// 클라이언트와 동기화
		OnRep_CurPlayer();  //---> 자동 호출 안 됨?
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ServerRPC_RemovePlayerNum_Implementation: Player is null"));
	}	
}


void ACJS_CountPlayerUIActor::ServerRPC_AddPlayerNum_Implementation(ACJS_LobbyPlayer* Player, int32 AddNum)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::ServerRPC_AddPlayerNum_Implementation() playerName: %s, Addnum: %d"), *Player->GetName(), AddNum);
	//SetOwner(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::ServerRPC_AddPlayerNum_Implementation() if (player)"));
		// 플레이어 정보를 배열에 저장
		ClickedPlayers.Add(Player);

		// 플레이어 정보 로그 출력
		FString PlayerName = Player->GetName();
		FString UniqueNetIdStr = TEXT("Unknown");

		if (APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()))
		{
			if (PlayerController->PlayerState && PlayerController->PlayerState->GetUniqueId().IsValid())
			{
				UniqueNetIdStr = PlayerController->PlayerState->GetUniqueId()->ToString();
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Player added: Name = %s, UniqueNetId = %s"), *PlayerName, *UniqueNetIdStr);

		// 플레이어 정보 로그 출력
		/*FString PlayerName = Player->GetName();
		APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());
		int32 PlayerID = PlayerController ? PlayerController->GetUniqueID() : -1;

		UE_LOG(LogTemp, Warning, TEXT("Player added: Name = %s, PlayerID = %d"), *PlayerName, PlayerID);*/


		// Update the player count on the server
		UE_LOG(LogTemp, Warning, TEXT("CurPlayer += AddNum;"));
		CurPlayer += AddNum;
		if (CurPlayer > MaxPlayer)
		{
			CurPlayer = MaxPlayer;
		}
		// Sync the value with clients
		OnRep_CurPlayer();  //----------------------> 자동으로 호출 안 됨
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ServerRPC_AddPlayerInfo_Implementation: Player is null"));
	}
	
	// 최대 인원수에 도달했을 때 처리
	if (CurPlayer == MaxPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("	if (CurPlayer == MaxPlayer) "));
		StartInteractiveExperience();
	}
}
bool ACJS_CountPlayerUIActor::ServerRPC_AddPlayerNum_Validate(ACJS_LobbyPlayer* Player, int32 AddNum)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::ServerRPC_AddPlayerNum_Validate()"));
	return true;  // 기본적으로 유효성 검사는 항상 true로 설정
}


void ACJS_CountPlayerUIActor::OnRep_CurPlayer()
{
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::OnRep_CurPlayer() called on %s"), *GetOwner()->GetName());

	// 클라이언트에서 해당 함수가 호출되었음을 확인
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_CurPlayer() called on client without authority."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_CurPlayer() called on server: %s"), *GetName());
	}

	UpdatePlayerNum(CurPlayer);
}
void ACJS_CountPlayerUIActor::UpdatePlayerNum(int32 NewPlayerNum)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::UpdatePlayerNum() called on %s with NewPlayerNum: %d"), *GetName(), NewPlayerNum);
	if (CountPlayerUI)
	{
		CountPlayerUI->ShowPlayerNum(NewPlayerNum, MaxPlayer);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CountPlayerUI is null on %s"), *GetName());
	}
}


void ACJS_CountPlayerUIActor::StartInteractiveExperience()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::StartInteractiveExperience()"));

	// 플레이어 수 확인
	if (ClickedPlayers.Num() != MaxPlayer)
	{
		//UE_LOG(LogTemp, Error, TEXT("StartInteractiveExperience: Expected 2 players, but got %d"), ClickedPlayers.Num());
		return;
	}

	auto* Art1_Manager = GetWorld()->SpawnActor<ASG_Art1_Manager>(ASG_Art1_Manager::StaticClass());

	Art1_Manager->InitPlayer(ClickedPlayers[0], ClickedPlayers[1]);

	for (int32 i = 0; i < ClickedPlayers.Num(); ++i)
	{
		ACJS_LobbyPlayer* Player = ClickedPlayers[i];
		if (Player)
		{
			// 플레이어의 현재 위치 로그 출력
			FString PlayerName = Player->GetName();
			FVector CurrentLocation = Player->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Player %s current location: %s"), *PlayerName, *CurrentLocation.ToString());

			// 서버에서 플레이어 위치 이동
			//Player->SetActorTransform(TargetTransforms[i]);
			Player->ServerRPC_SpawnArtPlayer(TargetTransforms[i]);
			PRINTLOG(TEXT("Player->ServerRPC_SpawnArtPlayer(%s);"), *TargetTransforms[i].ToString());
			// 플레이어의 새로운 위치 로그 출력
			FVector NewLocation = Player->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Player %s moved to new location: %s"), *PlayerName, *NewLocation.ToString());

			// 클라이언트에서 카메라 및 UI 설정을 위해 ClientRPC 호출
			Player->ClientRPC_MultiplaySetting();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("StartInteractiveExperience: Player is null"));
		}
	}
	Art1_Manager->InitArtPlayer();
	// 클릭한 플레이어 목록 초기화 전에 로그 출력
	UE_LOG(LogTemp, Warning, TEXT("Before clearing ClickedPlayers:"));
	UE_LOG(LogTemp, Warning, TEXT("ClickedPlayers.Num() = %d"), ClickedPlayers.Num());
	for (int32 i = 0; i < ClickedPlayers.Num(); ++i)
	{
		if (ClickedPlayers[i])
		{
			FString PlayerName = ClickedPlayers[i]->GetName();
			UE_LOG(LogTemp, Warning, TEXT("ClickedPlayers[%d]: %s"), i, *PlayerName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ClickedPlayers[%d]: null"), i);
		}
	}

	// 클릭한 플레이어 목록 초기화
	ClickedPlayers.Empty();
	CurPlayer = 0;

	// 클릭한 플레이어 목록 초기화 후에 로그 출력
	UE_LOG(LogTemp, Warning, TEXT("After clearing ClickedPlayers:"));
	UE_LOG(LogTemp, Warning, TEXT("ClickedPlayers.Num() = %d"), ClickedPlayers.Num());

	// 클라이언트 동기화
	OnRep_CurPlayer();  //---> 자동으로 불리도록 해 보기 -->안 됨
}



// 네트워크 동기화 하는 부분
void ACJS_CountPlayerUIActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// CurPlayer가 네트워크 동기화가 되도록 설정
	DOREPLIFETIME(ACJS_CountPlayerUIActor, CurPlayer);
}







// Multicast 함수로 모든 클라이언트에 값 전달
//void ACJS_CountPlayerUIActor::MulticastRPC_UpdatePlayerNum_Implementation(int32 NewPlayerNum)
//{
//	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::MulticastRPC_UpdatePlayerNum_Implementation()"));
//
//	// 서버와 클라이언트를 구분하여 출력
//	if (HasAuthority())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("This is the server broadcasting the player number update."));
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("This is a client receiving the updated player number."));
//	}
//
//	UpdatePlayerNum(NewPlayerNum);
//}


//void ACJS_CountPlayerUIActor::AddPlayerNum()
//{
//	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::AddPlayerNum()"));
//	SetCurPlayerNum(1);
//	//CountPlayerUI->ShowPlayerNum(CurPlayer, MaxPlayer);
//}
//int32 ACJS_CountPlayerUIActor::GetCurPlayerNum()
//{
//	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::GetCurPlayerNum()"));
//	return CurPlayer;
//}
//void ACJS_CountPlayerUIActor::SetCurPlayerNum(int32 addNum)
//{
//	UE_LOG(LogTemp, Warning, TEXT("ACJS_CountPlayerUIActor::SetCurPlayerNum()"));
//	CurPlayer += addNum;
//	ServerRPC_AddPlayerNum(CurPlayer);
//}



//void ACJS_CountPlayerUIActor::PrintNetLog()
//{
	//const FString conStr = GetNetConnection() ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	//const FString ownerName = GetOwner() ? GetOwner()->GetName() : TEXT("No Owner");

	//FString logStr = FString::Printf(TEXT("Connection : %s\nOwner Name : %s\nLocal Role : %s\nRemote Role : %s"), *conStr, *ownerName, *LOCALROLE, *REMOTEROLE);
	//FVector loc = GetActorLocation() + GetActorUpVector() * 30;
	//DrawDebugString(GetWorld(), loc, logStr, nullptr, FColor::Yellow, 0, true, 1.f);
//}
//void ACJS_CountPlayerUIActor::FindOwner()
//{
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
//}




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



