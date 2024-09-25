// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ArtPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API UArtPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
