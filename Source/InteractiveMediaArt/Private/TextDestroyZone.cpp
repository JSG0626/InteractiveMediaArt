// Fill out your copyright notice in the Description page of Project Settings.


#include "TextDestroyZone.h"
#include "Components/BoxComponent.h"

// Sets default values
ATextDestroyZone::ATextDestroyZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DestroyZone = CreateDefaultSubobject<UBoxComponent>(TEXT("DestroyZone"));
	DestroyZone->SetupAttachment(RootComponent);
	DestroyZone->SetCollisionProfileName(TEXT("DestroyZone"));

}

// Called when the game starts or when spawned
void ATextDestroyZone::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		SetReplicateMovement(true);
		SetReplicates(true);
	}
	DestroyZone->OnComponentBeginOverlap.AddDynamic(this, &ATextDestroyZone::DestroyText);

}

// Called every frame
void ATextDestroyZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATextDestroyZone::DestroyText(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("DestroyText"));
	if (HasAuthority())
	{
		check(OtherComp); if (nullptr == OtherComp) return;

		OtherComp->DestroyComponent(true);
		UE_LOG(LogTemp, Warning, TEXT("Destroy Article Text"));
	}
}

