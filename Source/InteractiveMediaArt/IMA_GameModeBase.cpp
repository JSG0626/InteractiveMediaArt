// Fill out your copyright notice in the Description page of Project Settings.


#include "IMA_GameModeBase.h"
#include "GameFramework/Actor.h"

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
}
