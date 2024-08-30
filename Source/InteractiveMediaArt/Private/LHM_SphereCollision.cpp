// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM_SphereCollision.h"
#include "Components/SphereComponent.h"

// Sets default values
ALHM_SphereCollision::ALHM_SphereCollision()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetSimulatePhysics(true);
	SphereCollision->SetEnableGravity(false);
	SphereCollision->SetMassOverrideInKg(NAME_None, 10.f);

	SphereCollision->OnComponentHit.AddDynamic(this, &ALHM_SphereCollision::OnSphereHit);
}

// Called when the game starts or when spawned
void ALHM_SphereCollision::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(DestroySelfTimerHandle, this, &ALHM_SphereCollision::DestroySphereCollision, 1.1f, false);
}

// Called every frame
void ALHM_SphereCollision::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALHM_SphereCollision::DestroySphereCollision()
{
	if (SphereCollision != nullptr)
	{
		SphereCollision->DestroyComponent(true);
		UE_LOG(LogTemp, Warning, TEXT(" Destroy SphereCollision "));
	}
}

void ALHM_SphereCollision::OnSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		GetWorldTimerManager().SetTimer(DestroyOtherActorTimerHandle, FTimerDelegate::CreateUObject(this, &ALHM_SphereCollision::DestroyOtherActor, OtherActor), 1.0f, false);
		UE_LOG(LogTemp, Warning, TEXT(" On Sphere Hit, Destroy Other Actor "));
	}
}

void ALHM_SphereCollision::DestroyOtherActor(AActor* ActorToDestroy)
{
	if (ActorToDestroy) ActorToDestroy->Destroy();
}

