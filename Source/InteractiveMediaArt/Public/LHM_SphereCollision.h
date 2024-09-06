// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LHM_SphereCollision.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ALHM_SphereCollision : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALHM_SphereCollision();

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* SphereCollision;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FTimerHandle DestroySelfTimerHandle;
	FTimerHandle DestroyOtherActorTimerHandle;

	void DestroySphereCollision();

	UFUNCTION()
	void OnSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void DestroyOtherActor(AActor* ActorToDestroy);


};
