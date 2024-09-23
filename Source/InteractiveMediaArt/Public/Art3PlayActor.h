// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Art3PlayActor.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API AArt3PlayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArt3PlayActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* SphereComp;

	UPROPERTY()
	class APlayerController* pc;

	UPROPERTY()
	FVector2D PreviousMousePosition;

	void SetPreviousPosition();
	void MoveSphereCompWithMouse(FVector2D MouseDelta);

};
