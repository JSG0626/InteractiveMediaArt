// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Art1_Manager.h"
#include "CJS/CJS_LobbyPlayer.h"
#include "SG_ArtPlayer.h"
#include "Net/UnrealNetwork.h"
#include "../InteractiveMediaArt.h"
// Sets default values
#include "Kismet/KismetSystemLibrary.h"
ASG_Art1_Manager::ASG_Art1_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void ASG_Art1_Manager::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &ASG_Art1_Manager::CountDownTick, 1.0f, true);
}

// Called every frame
void ASG_Art1_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASG_Art1_Manager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASG_Art1_Manager, RestTime);

}

void ASG_Art1_Manager::InitPlayer(ACJS_LobbyPlayer* P1, ACJS_LobbyPlayer* P2)
{
	Player1 = P1;
	Player2 = P2;


}

void ASG_Art1_Manager::InitArtPlayer()
{
	Player1->ArtPlayer->ArtManager = this;
	Player2->ArtPlayer->ArtManager = this;
}

void ASG_Art1_Manager::CountDownTick()
{
	if ( RestTime > 0 )
	{
		RestTime--;
		MulticastRPC_UpdateUI();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
		// 게임 종료 로직

		GetScore();
		FTimerHandle handle;

		GetWorldTimerManager().SetTimer(handle, this, &ASG_Art1_Manager::GameEnd, 2.0f, false);
	}
}

void ASG_Art1_Manager::OnRep_RestTime()
{
	MulticastRPC_UpdateUI();
}

void ASG_Art1_Manager::MulticastRPC_UpdateUI_Implementation()
{
	for( auto It = GetWorld()->GetPlayerControllerIterator(); It; It++ )
	{
		auto* pc = Cast<APlayerController>(*It);
		if ( pc )
		{
			auto* Player = Cast<ACJS_LobbyPlayer>(pc->GetPawn());
			if ( Player )
			{
				auto* ArtPlayer = Player->ArtPlayer;
				if ( ArtPlayer )
				{
					ArtPlayer->UpdateMainUI(RestTime);
				}
			}
		}
	}
}

void ASG_Art1_Manager::ServerRPC_GameEnd_Implementation()
{
	check(Player1); if ( nullptr == Player1 ) return;
	Player1->MulticastRPC_ReturnToCamera();
	Player1->ArtPlayer->Destroy();
	check(Player2); if ( nullptr == Player2 ) return;
	Player2->MulticastRPC_ReturnToCamera();
	Player2->ArtPlayer->Destroy();
	
}

void ASG_Art1_Manager::GameEnd()
{
	ServerRPC_GameEnd();
}

void ASG_Art1_Manager::GetScore()
{
	if ( HasAuthority() )
	{
		check(Player1); if ( nullptr == Player1 ) return;
		check(Player2); if ( nullptr == Player2 ) return;

		int32 Player1Score = 0;
		int32 Player2Score = 0;

		FVector Start1(1700, -3500, 940);
		FVector End1(1700, -3700, 940);
		FVector HalfSize(750, 1000, 1000);
		ETraceTypeQuery traceChannel = ETraceTypeQuery::TraceTypeQuery3;
		TArray<AActor*> actorsToIgnore;
		TArray<FHitResult> hitInfos1;

		FVector Start2(3260, -3500, 940);
		FVector End2(3260, -3700, 940);
		TArray<FHitResult> hitInfos2;

		UKismetSystemLibrary::BoxTraceMulti(GetWorld(), Start1, End1, HalfSize, FRotator(0), traceChannel, false, actorsToIgnore, EDrawDebugTrace::ForDuration, hitInfos1, true, FColor::Red);
		UKismetSystemLibrary::BoxTraceMulti(GetWorld(), Start2, End2, HalfSize, FRotator(0), traceChannel, false, actorsToIgnore, EDrawDebugTrace::ForDuration, hitInfos2, true, FColor::Red);

		Player1Score = hitInfos1.Num();
		Player2Score = hitInfos2.Num();

		if ( Player1Score >= Player2Score )
		{
			Player1->MulticastRPC_ShowArt1WinUI();
			Player2->MulticastRPC_ShowArt1LoseUI();
		}
		else
		{
			Player1->MulticastRPC_ShowArt1LoseUI();
			Player2->MulticastRPC_ShowArt1WinUI();
		}

		PRINTLOG(TEXT("Player1Score: %d, 2Score: %d"), Player1Score, Player2Score);
	}
}

