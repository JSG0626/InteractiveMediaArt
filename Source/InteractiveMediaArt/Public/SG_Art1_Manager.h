// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SG_Art1_Manager.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ASG_Art1_Manager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASG_Art1_Manager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void InitPlayer(class ACJS_LobbyPlayer* P1, class ACJS_LobbyPlayer* P2);
	void InitArtPlayer();
	void CountDownTick();

	FTimerHandle CountDownTimerHandle;
	UPROPERTY(ReplicatedUsing = OnRep_RestTime)
	float RestTime = 60;

	UFUNCTION()
	void OnRep_RestTime();

	UPROPERTY(Replicated)
	class ACJS_LobbyPlayer* Player1;

	UPROPERTY(Replicated)
	class ACJS_LobbyPlayer* Player2;


	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_UpdateUI();

	UFUNCTION(Server, Reliable)
	void ServerRPC_GameEnd();
private:
	void GameEnd();
	void GetScore();
};
