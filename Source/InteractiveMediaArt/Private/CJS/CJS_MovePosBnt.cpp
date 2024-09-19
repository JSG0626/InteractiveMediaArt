// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_MovePosBnt.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Camera/CameraActor.h"
#include "AimPoint.h"
#include "CJS/CJS_LobbyPlayer.h"
#include "Camera/CameraComponent.h"

// Sets default values
ACJS_MovePosBnt::ACJS_MovePosBnt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	ButtonComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonComp"));
	ButtonComp->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> temp1Button(TEXT("/Script/Engine.StaticMesh'/Game/ArtProject/CJS/Blueprints/CJS_Button.CJS_Button'"));
	if (temp1Button.Succeeded())
	{
		ButtonComp->SetStaticMesh(temp1Button.Object);
		//ButtonComp->SetRelativeScale3D(FVector(0.5));
		//ButtonComp->SetRelativeRotation(FRotator(0, 90, 0));
		ButtonComp->SetVisibility(false);
		ButtonComp->SetGenerateOverlapEvents(true);
	}

	VisibleBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("VisibleBoxComp"));
	VisibleBoxComp->SetupAttachment(ButtonComp);
	//VisibleBoxComp->SetRelativeLocation(FVector(520, 130, 1640));  
	//VisibleBoxComp->SetRelativeScale3D(FVector(15, 2.5, 50));
	VisibleBoxComp->SetCollisionProfileName(TEXT("OverlapAll"));
	VisibleBoxComp->SetGenerateOverlapEvents(true);

	VisibleBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACJS_MovePosBnt::OnOverlapBegin);
	VisibleBoxComp->OnComponentEndOverlap.AddDynamic(this, &ACJS_MovePosBnt::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ACJS_MovePosBnt::BeginPlay()
{
	Super::BeginPlay();
		
	Art1_Single_TargetCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector(-1870, -2310, 810), FRotator(0, -90, 0));   // (X=-1870.000000,Y=-2310.000000,Z=810.000000)
	auto* Art1_Single_TargetCameraComp = Art1_Single_TargetCamera->GetCameraComponent();
	Art1_Single_TargetCameraComp->ProjectionMode = ECameraProjectionMode::Orthographic;
	Art1_Single_TargetCameraComp->OrthoWidth = 3200.f;
	Art1_Single_TargetTransform = FTransform(FRotator(0, -90, 0), FVector(1897, 3652, 210));   // (X=-1896.847102,Y=-3651.908160,Z=210.000006)


	Art1_Multi_TargetCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector(2470, -2090, 750), FRotator(0, -90, 0));    // (X=2470.000000,Y=-2090.000000,Z=750.000000)
	auto* Art1_Multi_TargetCameraComp = Art1_Multi_TargetCamera->GetCameraComponent();
	Art1_Multi_TargetCameraComp->ProjectionMode = ECameraProjectionMode::Orthographic;
	Art1_Multi_TargetCameraComp->OrthoWidth = 3200.f;
	Art1_Multi1_TargetTransform = FTransform(FRotator(0, -90, 0), FVector(1650, -3580, 300));  // 왼쪽  
	Art1_Multi2_TargetTransform = FTransform(FRotator(0, -90, 0), FVector(3217, 3580, 210));  // 오른쪽 (X=3217.214946,Y=-3580.000000,Z=210.000014)
	
	Art2_TargetCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector(-5710, -1470, 850), FRotator(0, -90, 0));
	FVector targetLocation = Art2_TargetCamera->GetActorLocation();
	FRotator targetRotation = Art2_TargetCamera->GetActorRotation();
	Art2_TargetTransform = FTransform(targetRotation, targetLocation);
}

// Called every frame
void ACJS_MovePosBnt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACJS_MovePosBnt::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ButtonComp->SetVisibility(true);

	ACJS_LobbyPlayer* Player = Cast<ACJS_LobbyPlayer>(OtherActor);

	if (Player)
	{
		Player->ShowAimPoint();
	}
}

void ACJS_MovePosBnt::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ButtonComp->SetVisibility(false);

	ACJS_LobbyPlayer* Player = Cast<ACJS_LobbyPlayer>(OtherActor);
	if (Player)
	{
		Player->HideAimPoint();
	}
}

