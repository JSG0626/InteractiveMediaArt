// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"
#include "ButtonExp.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API AButtonExp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AButtonExp();

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

	//UPROPERTY(EditDefaultsOnly)
	//class AArticleMap* ArticleMap;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class UUserWidget> WBP_aimpoint;

	UPROPERTY()
	class UAimPoint* AimpointUI;


	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class ASG_ArtPlayer> ArtPlayerFactory;


	FVector TargetLocation = FVector(1260, -3580, 180);
	FRotator TargetRotation = FRotator(0, -90, 0);
	FTransform TargetTransform = FTransform(TargetRotation, TargetLocation);

	UPROPERTY(EditDefaultsOnly)
	class ACameraActor* TargetCamera;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class ACameraActor> cameraFactory;

	
	

};
