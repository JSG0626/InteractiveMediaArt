// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Player.h"
#include "SG_ServerManager.h"
// Sets default values
ASG_Player::ASG_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASG_Player::BeginPlay()
{
	Super::BeginPlay();
	
	ServerManager = GetWorld()->SpawnActor<ASG_ServerManager>(ASG_ServerManager::StaticClass());
	check(ServerManager); if (nullptr == ServerManager) return;
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("ServerManager Spawn")));
	ServerManager->Me = this;
}

// Called every frame
void ASG_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASG_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

