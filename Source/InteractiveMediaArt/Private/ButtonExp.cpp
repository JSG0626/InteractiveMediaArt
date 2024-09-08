// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonExp.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Camera/CameraActor.h"
#include "AimPoint.h"
#include "Blueprint/UserWidget.h"
#include "CJS/CJS_LobbyPlayer.h"
#include "Camera/CameraComponent.h"

// Sets default values
AButtonExp::AButtonExp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ButtonComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonComp"));
	ButtonComp->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempButton(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	if (tempButton.Succeeded())
	{
		ButtonComp->SetStaticMesh(tempButton.Object);
		ButtonComp->SetRelativeScale3D(FVector(0.7, 0.7, 0.1));
		ButtonComp->SetRelativeRotation(FRotator(0, 90, 0));
		ButtonComp->SetVisibility(false);
		ButtonComp->SetGenerateOverlapEvents(true);
	}

	VisibleBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("VisibleBoxComp"));
	VisibleBoxComp->SetupAttachment(ButtonComp);
	VisibleBoxComp->SetRelativeLocation(FVector(-585, 260, 2140));
	VisibleBoxComp->SetRelativeScale3D(FVector(17.5f, 2.5f, 75));
	VisibleBoxComp->SetCollisionProfileName(TEXT("OverlapAll"));
	VisibleBoxComp->SetGenerateOverlapEvents(true);

	VisibleBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AButtonExp::OnOverlapBegin);
	VisibleBoxComp->OnComponentEndOverlap.AddDynamic(this, &AButtonExp::OnOverlapEnd);

}

// Called when the game starts or when spawned
void AButtonExp::BeginPlay()
{
	Super::BeginPlay();

	
	TargetCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector(2470,-2150,840),FRotator(0,-90,0));
	auto* TargetCameraComp = TargetCamera->GetCameraComponent();
	TargetCameraComp->ProjectionMode = ECameraProjectionMode::Orthographic;
	TargetCameraComp->OrthoWidth = 3200.f;
	
}

// Called every frame
void AButtonExp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AButtonExp::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Player = Cast<ACJS_LobbyPlayer>(OtherActor);
	if (Player)
	{
		Player->ShowAimPoint();
	}

	ButtonComp->SetVisibility(true);
	//ButtonDescription->SetVisibility(true);
	//ButtonChatBot->SetVisibility(true);
	// 
	//if (OtherActor && (OtherActor != this) && OtherComp)
	//{
	//	ACharacter* pc = Cast<ACharacter>(OtherActor);
	//	if (pc)
	//	{
	//		
	//		
	//		if (AimpointUI != nullptr && WBP_aimpoint != nullptr)
	//		{
	//			AimpointUI->AddToViewport();
	//		}
	//	}
	//}
}

void AButtonExp::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto Player = Cast<ACJS_LobbyPlayer>(OtherActor);
	if (Player)
	{
		Player->HideAimPoint();
	}
	ButtonComp->SetVisibility(false);

	//if (OtherActor && (OtherActor != this) && OtherComp)
	//{
	//	ACharacter* pc = Cast<ACharacter>(OtherActor);
	//	if (pc)
	//	{
	//		ButtonComp->SetVisibility(false);
	//		//ButtonDescription->SetVisibility(false);
	//		//ButtonChatBot->SetVisibility(false);

	//		if (AimpointUI != nullptr && WBP_aimpoint != nullptr)
	//		{
	//			AimpointUI->RemoveFromParent();
	//		}
	//	}
	//}
}