// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnvSettingWidget.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API UEnvSettingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableText* Path_Input;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* Path_Button;

	UFUNCTION()
	void SetPath();
};
