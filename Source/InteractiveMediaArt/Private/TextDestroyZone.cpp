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

	DestroyZone->OnComponentHit.AddDynamic(this, &ATextDestroyZone::DestroyText);
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

}

// Called every frame
void ATextDestroyZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATextDestroyZone::DestroyText(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		if (HitComp != nullptr) HitComp->DestroyComponent(true);
		UE_LOG(LogTemp, Warning, TEXT("Destroy Article Text"));
	}
}

