// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LHM_ArticleText.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ALHM_ArticleText : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALHM_ArticleText();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override; // Construction Script

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void CreateStaticMesh();

	UPROPERTY(EditDefaultsOnly)
	int32 MeshCount = 1014;

	UPROPERTY(EditDefaultsOnly)
	FString BaseMeshName = TEXT("Article_Text_");

};
