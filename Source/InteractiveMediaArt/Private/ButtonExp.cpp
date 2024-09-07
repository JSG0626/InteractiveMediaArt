// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonExp.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Camera/CameraActor.h"
#include "AimPoint.h"

// Sets default values
AButtonExp::AButtonExp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootBoxComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootBoxComp"));
	RootBoxComp->SetupAttachment(RootComponent);

	ButtonExperience = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonExperience"));
	ButtonExperience->SetupAttachment(RootBoxComp);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempButton1 (TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	if (tempButton1.Succeeded())
	{
		ButtonExperience->SetStaticMesh(tempButton1.Object);
		ButtonExperience->SetRelativeScale3D(FVector(0.7, 0.7, 0.1));
		ButtonExperience->SetRelativeRotation(FRotator(0,90,90));
		ButtonExperience->SetVisibility(false);
		ButtonExperience->SetGenerateOverlapEvents(true);
	}

	ButtonDescription = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonDescription"));
	ButtonDescription->SetupAttachment(RootBoxComp);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempButton2(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	if (tempButton2.Succeeded())
	{
		ButtonDescription->SetStaticMesh(tempButton2.Object);
		ButtonDescription->SetRelativeLocation(FVector(0,0,100));
		ButtonDescription->SetRelativeScale3D(FVector(0.7, 0.7, 0.1));
		ButtonDescription->SetRelativeRotation(FRotator(0, 90, 90));
		ButtonDescription->SetVisibility(false);
		ButtonDescription->SetGenerateOverlapEvents(true);
	}

	ButtonChatBot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonChatBot"));
	ButtonChatBot->SetupAttachment(RootBoxComp);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempButton3(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	if (tempButton3.Succeeded())
	{
		ButtonChatBot->SetStaticMesh(tempButton3.Object);
		ButtonChatBot->SetRelativeLocation(FVector(0, 0, 200));
		ButtonChatBot->SetRelativeScale3D(FVector(0.7, 0.7, 0.1));
		ButtonChatBot->SetRelativeRotation(FRotator(0, 90, 90));
		ButtonChatBot->SetVisibility(false);
		ButtonChatBot->SetGenerateOverlapEvents(true);
	}

	VisibleBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("VisibleBoxComp"));
	VisibleBoxComp->SetupAttachment(RootBoxComp);
	VisibleBoxComp->SetRelativeLocation(FVector(-320, -390, -20));
	VisibleBoxComp->SetRelativeScale3D(FVector(11, 12, 3));
	VisibleBoxComp->SetCollisionProfileName(TEXT("OverlapAll"));
	VisibleBoxComp->SetGenerateOverlapEvents(true);

	VisibleBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AButtonExp::OnOverlapBegin);
	VisibleBoxComp->OnComponentEndOverlap.AddDynamic(this, &AButtonExp::OnOverlapEnd);

	AimpoiontUI = CreateWidget<UAimPoint>(GetWorld(), WBP_aimpoint);
}

// Called when the game starts or when spawned
void AButtonExp::BeginPlay()
{
	Super::BeginPlay();

	
	TargetCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector(2130,-2150,840),FRotator(0,-90,0));
	
}

// Called every frame
void AButtonExp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AButtonExp::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		ACharacter* pc = Cast<ACharacter>(OtherActor);
		if (pc)
		{
			ButtonExperience->SetVisibility(true);
			ButtonDescription->SetVisibility(true);
			ButtonChatBot->SetVisibility(true);
			
			if (AimpoiontUI != nullptr)
			{
				AimpoiontUI->AddToViewport(true);
			}
			
		}
	}

}

void AButtonExp::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		ACharacter* pc = Cast<ACharacter>(OtherActor);
		if (pc)
		{
			ButtonExperience->SetVisibility(false);
			ButtonDescription->SetVisibility(false);
			ButtonChatBot->SetVisibility(false);

			if (AimpoiontUI != nullptr)
			{
				AimpoiontUI->RemoveFromViewport();
			}
		}
	}
}