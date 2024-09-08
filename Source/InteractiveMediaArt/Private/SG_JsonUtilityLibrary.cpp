// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_JsonUtilityLibrary.h"

const float WORLD_SCALE_X = 300;
const float WORLD_SCALE_Y = 100;
const float WORLD_SCALE_Z = 200;

bool USG_JsonUtilityLibrary::MediaPipeJsonParse(const FString& Json, class ASG_ArtPlayer* ArtPlayer, const TArray<FString>& Landmarks, TArray<FVector>& OutCoordinates )
{
	if (nullptr == ArtPlayer) return;
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Json: %s"), *Json));
	//UE_LOG(LogTemp, Warning, TEXT("Json: %s"), *Json);
	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(Json);
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());

	if (FJsonSerializer::Deserialize(jsonReader, jsonObject))
	{
		for (const auto& data : jsonObject->Values)
		{
			const FString& Key = data.Key;
			TSharedPtr<FJsonObject> ValueObject = data.Value->AsObject();

			if (ValueObject)
			{
				float x = ValueObject->GetNumberField(TEXT("x"));
				float z = ValueObject->GetNumberField(TEXT("y"));
				float y = ValueObject->GetNumberField(TEXT("z"));

				x = -(x - 0.5f);
				y = 0.5f - y;
				z = 0.5f - z;

				x *= WORLD_SCALE_X;
				y *= WORLD_SCALE_Y;
				z *= WORLD_SCALE_Z;

				OutCoordinates.Add(FVector(x, y, z));
			}
		}
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("error: Desrialize failed"));
//		UE_LOG(LogTemp, Warning, TEXT("Json: %s"), *Json);
	}
	return false;
}