// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CJS_CancelBtn.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ACJS_CancelBtn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACJS_CancelBtn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Cancel BTN
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* CancleComp;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* VisibleBoxComp;

	//UFUNCTION()
    //void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    //UFUNCTION()
    //void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void HideCancelBtn();
	UFUNCTION()
	void ShowCancelBtn();


};
