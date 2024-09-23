// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LHM_MoveArt3Btn.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ALHM_MoveArt3Btn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALHM_MoveArt3Btn();

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

	FTransform Art3_TargetTransform;

	UPROPERTY(EditDefaultsOnly)
	class ACameraActor* Art3_TargetCamera;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class ACameraActor> cameraFactory;

};
