// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CJS_MovePosBnt.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ACJS_MovePosBnt : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACJS_MovePosBnt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* ButtonComp;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* VisibleBoxComp;

	UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	//FVector TargetLocation;
	//FRotator TargetRotation;
	FTransform Art1_Single_TargetTransform;
	FTransform Art1_Multi1_TargetTransform;
	FTransform Art1_Multi2_TargetTransform;

	FTransform Art2_TargetTransform;

	UPROPERTY(EditDefaultsOnly)
	class ACameraActor* Art1_Single_TargetCamera;
	UPROPERTY(EditDefaultsOnly)
	class ACameraActor* Art1_Multi_TargetCamera;
	UPROPERTY(EditDefaultsOnly)
	class ACameraActor* Art2_TargetCamera;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class ACameraActor> cameraFactory;


	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class ASG_ArtPlayer> ArtPlayerFactory;

};
