// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_MovePosBnt.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Camera/CameraActor.h"
#include "AimPoint.h"

// Sets default values
ACJS_MovePosBnt::ACJS_MovePosBnt()
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
	VisibleBoxComp->SetRelativeLocation(FVector(520, 130, 1640));  
	VisibleBoxComp->SetRelativeScale3D(FVector(15, 2.5, 50));
	VisibleBoxComp->SetCollisionProfileName(TEXT("OverlapAll"));
	VisibleBoxComp->SetGenerateOverlapEvents(true);

	VisibleBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACJS_MovePosBnt::OnOverlapBegin);
	VisibleBoxComp->OnComponentEndOverlap.AddDynamic(this, &ACJS_MovePosBnt::OnOverlapEnd);

	AimpointUI = CreateWidget<UAimPoint>(GetWorld(), WBP_aimpoint);
}

// Called when the game starts or when spawned
void ACJS_MovePosBnt::BeginPlay()
{
	Super::BeginPlay();
	
	TargetCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector(-5710, -1470, 850), FRotator(0, -90, 0));

	TargetLocation = TargetCamera->GetActorLocation();
	TargetRotation = TargetCamera->GetActorRotation();
	TargetTransform = FTransform(TargetRotation, TargetLocation);
}

// Called every frame
void ACJS_MovePosBnt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACJS_MovePosBnt::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		ACharacter* pc = Cast<ACharacter>(OtherActor);
		if (pc)
		{
			ButtonComp->SetVisibility(true);
		}

		if (AimpointUI != nullptr && WBP_aimpoint != nullptr)
		{
			AimpointUI->AddToViewport();
		}
	}
}

void ACJS_MovePosBnt::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		ACharacter* pc = Cast<ACharacter>(OtherActor);
		if (pc)
		{
			ButtonComp->SetVisibility(false);
		}

		if (AimpointUI != nullptr && WBP_aimpoint != nullptr)
		{
			AimpointUI->RemoveFromParent();
		}
	}
}

