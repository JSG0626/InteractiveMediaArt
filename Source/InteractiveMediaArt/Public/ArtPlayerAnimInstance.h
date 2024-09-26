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
	virtual void NativeBeginPlay() override;

	UPROPERTY()
	class ASG_ArtPlayer* Me;

	UPROPERTY(BlueprintReadOnly, Category = Position)
	FVector hand_l_position;

	UPROPERTY(BlueprintReadOnly, Category = Position)
	FVector hand_r_position;

	UPROPERTY(BlueprintReadOnly, Category = Position)
	FVector foot_l_position;

	UPROPERTY(BlueprintReadOnly, Category = Position)
	FVector foot_r_position;

	UPROPERTY(BlueprintReadOnly, Category = Position)
	FVector head_position;

	UPROPERTY(BlueprintReadOnly, Category = Position)
	FVector pelvis_position;

	UPROPERTY(BlueprintReadOnly, Category = Position)
	FVector spine5_position;

};
