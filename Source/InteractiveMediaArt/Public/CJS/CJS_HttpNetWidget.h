// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CJS_HttpNetWidget.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API UCJS_HttpNetWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class ACJS_HttpNetActor* HttpNetActor;

	UFUNCTION(BlueprintCallable)
	void OnMyClickSendPost();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WavFilePath;
	UFUNCTION(BlueprintCallable)
	void SetHttpNetActor(class ACJS_HttpNetActor* actor);

	FString ServerURL = "https://adapted-charmed-panda.ngrok-free.app/voice_stts";
};
