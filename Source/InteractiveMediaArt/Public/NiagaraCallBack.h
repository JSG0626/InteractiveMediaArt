// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraCallBack.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ANiagaraCallBack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANiagaraCallBack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void ReceiveParticleData(const TArray<FBasicParticleData>& Data);

private:
	UPROPERTY(EditAnywhere)
	UNiagaraComponent* NiagaraCompnent;

private:
	void PerformSphereTrace(const FVector& Start, const FVector& End, float Radius);
	void SetHitComp();

};
