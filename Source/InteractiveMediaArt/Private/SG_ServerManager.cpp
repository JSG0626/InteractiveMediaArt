// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_ServerManager.h"
#include "Modules/ModuleManager.h"
#include <Runtime/Online/WebSockets/Public/WebSocketsModule.h>
#include <Runtime/Online/WebSockets/Public/IWebSocket.h>

// Sets default values
ASG_ServerManager::ASG_ServerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASG_ServerManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASG_ServerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASG_ServerManager::ConnectToSocket(const FString& url)
{
	if (FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		WebSocket = FWebSocketsModule::Get().CreateWebSocket(url);
	}
}

void ASG_ServerManager::OnWebSocketConnected()
{
}

void ASG_ServerManager::OnWebSocketMessage(const FString& message)
{
}

void ASG_ServerManager::OnWebSocketClosed(int32 statusCode, const FString& reason, bool bWasClean)
{
}

void ASG_ServerManager::SendData(const FString& jsonData)
{
}

