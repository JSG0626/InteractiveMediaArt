// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LHM_ArticleText_Single.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ALHM_ArticleText_Single : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALHM_ArticleText_Single();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* RootComp;

	UPROPERTY(EditDefaultsOnly)
	int32 MeshCount = 1014;

	UPROPERTY(EditDefaultsOnly)
	FString BaseMeshName = TEXT("Article_Text_");
	
	// Static Mesh Components Array
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<UStaticMeshComponent*> TextComponents;
};
