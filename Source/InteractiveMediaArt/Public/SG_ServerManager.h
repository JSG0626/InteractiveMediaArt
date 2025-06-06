﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Templates/SharedPointer.h"
#include "Containers/Map.h"
#include "Engine/EngineTypes.h"
#include "SG_ServerManager.generated.h"

#define GET_NAME(X) #X

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	FString ServerIP = TEXT("192.168.0.77");
	
	UPROPERTY(EditDefaultsOnly)
	int32 ServerPort = 8090;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Python")
	FString PyDeafultPath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Python")
	FString pythonExePath = TEXT("C:\\Users\\Admin\\AppData\\Local\\Microsoft\\WindowsApps\\PythonSoftwareFoundation.Python.3.10_qbz5n2kfra8p0\\python.exe");
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Python")
	FString PyConnectServer = TEXT("TCP_response.py");

	class FSocket* ClientSocket;

	class TSharedPtr<class FInternetAddr> ServerAddr;

	TArray<TPair<int32, int32>> GetDatas;

	UPROPERTY(ReplicatedUsing = OnRep_Player)
	class ACJS_LobbyPlayer* Player;

	UFUNCTION()
	void OnRep_Player();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Object");
	class ASG_ArtPlayer* Me;

	void Active();

	UFUNCTION()
	void RunPythonScript(const FString& Path);

	UFUNCTION(BlueprintCallable)
	void CreateClient();

	bool  RecvAll(TArray<uint8>& OutData, uint32 Length, int32& BytesReceived);
	UFUNCTION(BlueprintCallable)
	void ReceiveData();
	void ReceiveRestData();


	UFUNCTION(BlueprintCallable)
	void Disconnect();

	uint32 ServerPID;
	FProcHandle ServerProcHandle;
	FString ReceivedJson;

	bool bReceiveSuccess = true;
	uint32 CurDataLength;
	uint32 TargetDataLength = 0;
	uint32 DataLength;

	// --------------------------------------------Multiplay--------------------------------------------

};