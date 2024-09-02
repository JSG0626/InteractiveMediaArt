// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_JsonUtilityLibrary.h"

void USG_JsonUtilityLibrary::MediaPipeJsonParse(const FString& Json, const TArray<FString>& Landmarks, TArray<TPair<float, float>> & OutCoordinates)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Json: %s"), *Json));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Json);
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
				float y = ValueObject->GetNumberField(TEXT("y"));
				OutCoordinates.Add({x, y});
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Desrialize failed")));
	}
}