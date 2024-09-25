// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_MultiPlayBTN.h"
#include "CJS/CJS_LobbyPlayer.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"



// Sets default values
ACJS_MultiPlayBTN::ACJS_MultiPlayBTN()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = false;  // 멀티 버튼은 복제되지 않도록 설정
	SetReplicates(false);

	ButtonComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonComp"));
	ButtonComp->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> temp1Button(TEXT("/Script/Engine.StaticMesh'/Game/ArtProject/CJS/Blueprints/CJS_Button.CJS_Button'"));
	if ( temp1Button.Succeeded() )
	{
		ButtonComp->SetStaticMesh(temp1Button.Object);
		ButtonComp->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		ButtonComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
		ButtonComp->SetVisibility(false);
		ButtonComp->SetGenerateOverlapEvents(true);
	}

	VisibleBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("VisibleBoxComp"));
	VisibleBoxComp->SetupAttachment(ButtonComp);
	VisibleBoxComp->SetRelativeLocation(FVector(400.f, 550.f, -100.f));
	VisibleBoxComp->SetRelativeScale3D(FVector(15.f, 24.f, 3.f));
	VisibleBoxComp->SetCollisionProfileName(TEXT("OverlapAll"));
	VisibleBoxComp->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void ACJS_MultiPlayBTN::BeginPlay()
{
	Super::BeginPlay();

	Art1_Multi_TargetCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector(2470, -2090, 750), FRotator(0, -90, 0));    // (X=2470.000000,Y=-2090.000000,Z=750.000000)
	auto* Art1_Multi_TargetCameraComp = Art1_Multi_TargetCamera->GetCameraComponent();
	Art1_Multi_TargetCameraComp->ProjectionMode = ECameraProjectionMode::Orthographic;
	Art1_Multi_TargetCameraComp->OrthoWidth = 3200.f;
	Art1_Multi1_TargetTransform = FTransform(FRotator(0, -90, 0), FVector(1650, -3580, 300));  // 왼쪽  
	Art1_Multi2_TargetTransform = FTransform(FRotator(0, -90, 0), FVector(3217, 3580, 210));  // 오른쪽 (X=3217.214946,Y=-3580.000000,Z=210.000014)
	
}

// Called every frame
void ACJS_MultiPlayBTN::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

