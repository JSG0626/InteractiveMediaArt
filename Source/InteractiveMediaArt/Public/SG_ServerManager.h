// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SG_ServerManager.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ASG_ServerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASG_ServerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ConnectToSocket(const FString& url);
	void OnWebSocketConnected();
	void OnWebSocketMessage(const FString& message);
	void OnWebSocketClosed(int32 statusCode, const FString& reason, bool bWasClean);
	void SendData(const FString& jsonData);

private:
	TSharedPtr<class IWebSocket> WebSocket;
};
