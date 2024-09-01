// Fill out your copyright notice in the Description page of Project Settings.

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
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	FString ServerIP = TEXT("127.0.0.1");

	UPROPERTY(EditDefaultsOnly)
	int32 ServerPort = 23456;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Python")
	FString PyDeafultPath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Python")
	FString PyConnectServer = TEXT("ConnectServer.py");

	class FSocket* ClientSocket;

	class TSharedPtr<class FInternetAddr> ServerAddr;

	TArray<TPair<int32, int32>> GetDatas;

	UPROPERTY(BlueprintReadOnly, Category = "Object");
	class ASG_Player* Me;

	UFUNCTION()
	void RunPythonScript(const FString& Path);

	UFUNCTION(BlueprintCallable)
	void CreateClient(FString IP, int32 Port);

	UFUNCTION(BlueprintCallable)
	void ReceiveData();

	UFUNCTION(BlueprintCallable)
	void Disconnect();

	void SendDataToPlayer(int32 data);

	uint32 ServerPID;
	FProcHandle ServerProcHandle;
};
