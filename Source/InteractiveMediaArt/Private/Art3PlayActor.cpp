// Fill out your copyright notice in the Description page of Project Settings.


#include "Art3PlayActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AArt3PlayActor::AArt3PlayActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;

	this->Tags.Add(FName("collider"));

	SphereComp->SetMobility(EComponentMobility::Movable);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	SphereComp->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (tempMesh.Succeeded())
	{
		SphereComp->SetStaticMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void AArt3PlayActor::BeginPlay()
{
	Super::BeginPlay();
	
	//ShowMouseCursor();

	//pc = UGameplayStatics::GetPlayerController(this, 0);
	//pc = Cast<APlayerController>();
	//auto* pc = GetWorld()->GetFirstPlayerController();

	
	/*if (pc)
	{
		pc->GetMousePosition(PreviousMousePosition.X, PreviousMousePosition.Y);
	}*/
	
}

// Called every frame
void AArt3PlayActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (pc == nullptr) return;
	if (pc)
	{
		FVector2D CurrentMousePosition;
		pc->GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y);
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("%s, %s"), *CurrentMousePosition.ToString(), *SphereComp->GetComponentLocation().ToString()));
		FVector2D MouseDelta = CurrentMousePosition - PreviousMousePosition;

		MoveSphereCompWithMouse(MouseDelta);

		// 마우스 위치 업데이트
		PreviousMousePosition = CurrentMousePosition;
	}
}

void AArt3PlayActor::MoveSphereCompWithMouse(FVector2D MouseDelta)
{
	//if (SphereComp)
	//{
	//	FVector NewLocation = SphereComp->GetComponentLocation();
	//	NewLocation.Y += MouseDelta.X * 0.8; // Y축 이동 (좌우)
	//	NewLocation.Z -= MouseDelta.Y * 0.8; // Z축 이동 (위아래)

	//	// SphereComp의 위치를 업데이트
	//	SphereComp->SetWorldLocation(NewLocation);

	//	UE_LOG(LogTemp, Warning, TEXT("New SphereComp Location: %s"), *NewLocation.ToString());
	//}

	if (SphereComp && pc)
	{
		FVector WorldLocation;
		FVector WorldDirection;
		pc->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

		// 특정 거리만큼 Z축으로 이동 (예: 1000 units)
		float Distance = 500.0f;
		FVector NewLocation = WorldLocation + (WorldDirection * Distance);

		// SphereComp의 현재 Z 위치를 유지
		NewLocation.X = SphereComp->GetComponentLocation().X;

		// SphereComp의 위치를 업데이트
		SphereComp->SetWorldLocation(NewLocation);

		//UE_LOG(LogTemp, Warning, TEXT("New SphereComp Location: %s"), *NewLocation.ToString());
	}
}

void AArt3PlayActor::SetPreviousPosition()
{
	pc->GetMousePosition(PreviousMousePosition.X, PreviousMousePosition.Y);
}