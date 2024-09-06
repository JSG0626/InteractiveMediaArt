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

public:

	UPROPERTY(EditDefaultsOnly)
	int32 MeshCount = 716;

	UPROPERTY(EditDefaultsOnly)
	FString BaseMeshName = TEXT("text2_News_");

	// Static Mesh Components Array
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<UStaticMeshComponent*> TextComponents;
	
};
