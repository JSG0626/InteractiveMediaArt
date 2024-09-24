// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM_MoveArt3Btn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CJS/CJS_LobbyPlayer.h"
#include "Camera/CameraActor.h"
// 현민!!! Love U~ <33333 ㅋㅋ


// Sets default values
ALHM_MoveArt3Btn::ALHM_MoveArt3Btn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ButtonComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonComp"));
	ButtonComp->SetupAttachment(RootComponent);
	ButtonComp->SetRelativeScale3D(FVector(0.5));

	ConstructorHelpers::FObjectFinder<UStaticMesh> temp1Button(TEXT("/Script/Engine.StaticMesh'/Game/ArtProject/CJS/Blueprints/CJS_Button.CJS_Button'"));
	if (temp1Button.Succeeded())
	{
		ButtonComp->SetStaticMesh(temp1Button.Object);
		ButtonComp->SetVisibility(true);
		ButtonComp->SetGenerateOverlapEvents(true);
	}

	VisibleBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("VisibleBoxComp"));
	VisibleBoxComp->SetupAttachment(ButtonComp);
	VisibleBoxComp->SetCollisionProfileName(TEXT("OverlapAll"));
	VisibleBoxComp->SetGenerateOverlapEvents(true);
	VisibleBoxComp->SetRelativeLocation(FVector(316.118776, 821.842175, -110));
	VisibleBoxComp->SetRelativeScale3D(FVector(15, 24, 3));

	//VisibleBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ALHM_MoveArt3Btn::OnOverlapBegin);
	//VisibleBoxComp->OnComponentEndOverlap.AddDynamic(this, &ALHM_MoveArt3Btn::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ALHM_MoveArt3Btn::BeginPlay()
{
	Super::BeginPlay();
	
	Art3_TargetCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector(5540, -1160, 735), FRotator(0));
	FVector targetLocation = Art3_TargetCamera->GetActorLocation();
	FRotator targetRotation = Art3_TargetCamera->GetActorRotation();
	Art3_TargetTransform = FTransform(targetRotation, targetLocation);
}

// Called every frame
void ALHM_MoveArt3Btn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void ALHM_MoveArt3Btn::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	ButtonComp->SetVisibility(true);
//
//	ACJS_LobbyPlayer* Player = Cast<ACJS_LobbyPlayer>(OtherActor);
//
//	if (Player)
//	{
//		Player->ShowAimPoint();
//	}
//}
//
//void ALHM_MoveArt3Btn::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	ButtonComp->SetVisibility(false);
//
//	ACJS_LobbyPlayer* Player = Cast<ACJS_LobbyPlayer>(OtherActor);
//	if (Player)
//	{
//		Player->HideAimPoint();
//	}
//}

