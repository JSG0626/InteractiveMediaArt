// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_IMA_GameInstance.h"

void USG_IMA_GameInstance::Init()
{
}

void USG_IMA_GameInstance::Host()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Host")));
}

void USG_IMA_GameInstance::Join(const FString& Address)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));
}
