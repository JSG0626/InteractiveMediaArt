// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorActor.h"
#include "Components/SphereComponent.h"
#include "ExhibitionGameInstance.h"
#include "EixtWidget.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ADoorActor::ADoorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->SetSphereRadius(400);
	SphereComp->SetCollisionProfileName(TEXT("Door"));

	LeftDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorComp"));
	LeftDoor->SetupAttachment(SphereComp);
	LeftDoor->SetRelativeLocation(FVector(-40, 0.824719, 0.222105));

	RightDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorComp"));
	RightDoor->SetupAttachment(SphereComp);
	RightDoor->SetRelativeLocation(FVector(120, 4.779947, 0.332034));
	RightDoor->SetRelativeRotation(FRotator(0, 180, 0));

	SetActorTickEnabled(false);
}

// Called when the game starts or when spawned
void ADoorActor::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ADoorActor::OnOverlapBegin);

	if (WBP_ExitWidget)
	{
		ExitWidget = CreateWidget<UEixtWidget>(GetWorld(), WBP_ExitWidget);
		ExitWidget->SetDoorActor(this);
	}
}

// Called every frame
void ADoorActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	OpenDoor(DeltaSeconds);
	CloseDoor(DeltaSeconds);
}

void ADoorActor::OpenDoor(float DeltaSeconds)
{
	if (bExit == true)
	{
		FRotator LeftDoorCurrentRotation = LeftDoor->GetRelativeRotation();
		FRotator RightDoorCurrentRotation = RightDoor->GetRelativeRotation();
		FRotator LeftDoorTargetRotation = FRotator(0.0f, -140.0f, 0.0f);
		FRotator RightDoorTargetRotation = FRotator(0.0f, 300.0f, 0.0f);
		float LerpAlpha = DeltaSeconds * 1.f;

		FRotator LeftDoorNewRotation = FMath::Lerp(LeftDoorCurrentRotation, LeftDoorTargetRotation, LerpAlpha);
		FRotator RightDoorNewRotation = FMath::Lerp(RightDoorCurrentRotation, RightDoorTargetRotation, LerpAlpha);

		LeftDoor->SetRelativeRotation(LeftDoorNewRotation);
		RightDoor->SetRelativeRotation(RightDoorNewRotation);
	}
}

void ADoorActor::CloseDoor(float DeltaSeconds)
{
	if (bExit == false)
	{
		FRotator LeftDoorCurrentRotation = LeftDoor->GetRelativeRotation();
		FRotator RightDoorCurrentRotation = RightDoor->GetRelativeRotation();
		FRotator LeftDoorTargetRotation = FRotator(0.0f, 0.0f, 0.0f);
		FRotator RightDoorTargetRotation = FRotator(0.0f, 180.0f, 0.0f);
		float LerpAlpha = DeltaSeconds * 1.f;

		FRotator LeftDoorNewRotation = FMath::Lerp(LeftDoorCurrentRotation, LeftDoorTargetRotation, LerpAlpha);
		FRotator RightDoorNewRotation = FMath::Lerp(RightDoorCurrentRotation, RightDoorTargetRotation, LerpAlpha);

		LeftDoor->SetRelativeRotation(LeftDoorNewRotation);
		RightDoor->SetRelativeRotation(RightDoorNewRotation);
	}
}

void ADoorActor::Exit()
{
	bExit = true;
	SetActorTickEnabled(true);
}

void ADoorActor::Return()
{
	bExit = false;
}

void ADoorActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Exit();
	// 1초 후에 CallExitUI 함수 호출
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADoorActor::CallExitUI, 1.0f, false);
}

void ADoorActor::CallExitUI()
{
	if (ExitWidget)
	{
		ExitWidget->VisibleExitUI();
	}
}

