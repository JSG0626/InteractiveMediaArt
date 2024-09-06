// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SG_TupleUtilityLibrary.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API USG_TupleUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	public:
	static TTuple<float, float, float> AddTuple(TTuple<float, float, float>& A, TTuple<float, float, float>& B);
};
