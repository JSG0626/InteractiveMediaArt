// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CJS_MultiPlayBTN.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ACJS_MultiPlayBTN : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACJS_MultiPlayBTN();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Multi BTN
	UPROPERTY()
	class UStaticMeshComponent* ButtonComp;
	UPROPERTY()
	class UBoxComponent* VisibleBoxComp;


	UPROPERTY(EditDefaultsOnly)
	class ACameraActor* Art1_Multi_TargetCamera;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class ACameraActor> cameraFactory;

	FTransform Art1_Multi1_TargetTransform;
	FTransform Art1_Multi2_TargetTransform;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class ASG_ArtPlayer> ArtPlayerFactory;

};
