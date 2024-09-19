// Fill out your copyright notice in the Description page of Project Settings.


#include "IMA_GameModeBase.h"
#include "GameFramework/Actor.h"
#include "CJS/CJS_CountPlayerUIActor.h"

void AIMA_GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		const APlayerController* pc = Cast<APlayerController>(iter->Get());
		
		if (pc && SharedCamera)
		{
			//pc->SetViewTarget(SharedCamera);
		}
	}

	// CountPlayerUIActorClass가 유효한지 확인하고 스폰
	if (CountPlayerUIActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		// CountPlayerUIActorClass를 기반으로 스폰
		CountPlayerUIActor = GetWorld()->SpawnActor<ACJS_CountPlayerUIActor>(CountPlayerUIActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CountPlayerUIActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("CountPlayerUIActor successfully spawned"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn CountPlayerUIActor"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CountPlayerUIActorClass is not set!"));
	}
}
