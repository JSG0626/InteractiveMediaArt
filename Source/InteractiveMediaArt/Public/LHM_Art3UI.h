// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LHM_Art3UI.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API ULHM_Art3UI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	void SetVisible();
	void SetHidden();
};
