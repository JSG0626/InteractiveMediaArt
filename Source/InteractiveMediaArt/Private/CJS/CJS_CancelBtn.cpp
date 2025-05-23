﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_CancelBtn.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "CJS/CJS_LobbyPlayer.h"

// Sets default values
ACJS_CancelBtn::ACJS_CancelBtn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = false;  // 복제 비활성화
	SetReplicates(false);

	CancleComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CancleComp"));
	CancleComp->SetupAttachment(RootComponent);
	// CancleComp 콜리전 설정 추가
	CancleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CancleComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel4);
	CancleComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CancleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Block);


	ConstructorHelpers::FObjectFinder<UStaticMesh> temp2Button(TEXT("/Script/Engine.StaticMesh'/Game/ArtProject/CJS/Blueprints/CJS_Button.CJS_Button'"));
	if (temp2Button.Succeeded())
	{
		CancleComp->SetStaticMesh(temp2Button.Object);
		CancleComp->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		CancleComp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
		CancleComp->SetVisibility(false);
		CancleComp->SetGenerateOverlapEvents(true);
	}

	VisibleBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CancleBoxComp"));
	VisibleBoxComp->SetupAttachment(CancleComp);
	VisibleBoxComp->SetRelativeLocation(FVector(400.f, 550.f, -100.f));  
	VisibleBoxComp->SetRelativeScale3D(FVector(15.f, 24.f, 3.f));
	VisibleBoxComp->SetCollisionProfileName(TEXT("OverlapAll"));
	VisibleBoxComp->SetGenerateOverlapEvents(true);

	//VisibleBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACJS_CancelBtn::OnOverlapBegin);
	//VisibleBoxComp->OnComponentEndOverlap.AddDynamic(this, &ACJS_CancelBtn::OnOverlapEnd);

	HideCancelBtn();
}

// Called when the game starts or when spawned
void ACJS_CancelBtn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACJS_CancelBtn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void ACJS_CancelBtn::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	CancleComp->SetVisibility(true);
//
//	ACJS_LobbyPlayer* Player = Cast<ACJS_LobbyPlayer>(OtherActor);
//
//	if (Player)
//	{
//		Player->ShowAimPoint();
//	}
//}
//
//void ACJS_CancelBtn::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	CancleComp->SetVisibility(false);
//
//	ACJS_LobbyPlayer* Player = Cast<ACJS_LobbyPlayer>(OtherActor);
//	if (Player)
//	{
//		Player->HideAimPoint();
//	}
//}

void ACJS_CancelBtn::HideCancelBtn()
{
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_CancelBtn::HideCancelBtn()"));
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ACJS_CancelBtn::ShowCancelBtn()
{	
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_CancelBtn::ShowCancelBtn()"));
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}


