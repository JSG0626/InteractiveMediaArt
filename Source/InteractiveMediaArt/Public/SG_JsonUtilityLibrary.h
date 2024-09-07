// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SG_JsonUtilityLibrary.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIVEMEDIAART_API USG_JsonUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static bool MediaPipeJsonParse(const FString& Json, const TArray<FString>& Landmarks, TArray<FVector>& OutCoordinate);
};
