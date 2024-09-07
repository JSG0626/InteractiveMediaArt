// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "CJS_HttpNetActor.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ACJS_HttpNetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACJS_HttpNetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY()
	//ACJS_HttpNetActor* HttpNetActorInstance;


	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> HttpNetUIFactory;
	UPROPERTY()
	class UCJS_HttpNetWidget* HttpNetUI;

	//UPROPERTY(EditAnywhere)
	//FString ReceiveFileSavePath;


	void ReqPostWaveFile(FString url, FString wavFilePath);
	void OnResPostWaveFile(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void PlaySoundFromFile(const FString& FilePath);
	FString GenerateSavePath();

};
