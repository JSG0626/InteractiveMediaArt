// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IMA_GameModeBase.generated.h"

/**
 * 
 */

UCLASS()
class INTERACTIVEMEDIAART_API AIMA_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	ACameraActor* SharedCamera;


	// LobbyPlayer NetActor ----------------------------------------
	UPROPERTY()
	class ACJS_CountPlayerUIActor* CountPlayerUIActor;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACJS_CountPlayerUIActor> CountPlayerUIActorClass;
};
