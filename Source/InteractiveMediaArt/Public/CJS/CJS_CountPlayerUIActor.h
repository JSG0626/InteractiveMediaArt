// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CJS_CountPlayerUIActor.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ACJS_CountPlayerUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACJS_CountPlayerUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// 작품 1의 체험 인원 표시 UI
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class UUserWidget> WBP_CountPlayerUI;
	UPROPERTY()
	class UCJS_CountPlayerUI* CountPlayerUI;

	UPROPERTY(ReplicatedUsing = AddPlayerNum, EditDefaultsOnly, Category = "PlayerNum") // 현재 참여 인원 변수 동기화
	int32 CurPlayer;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerNum")
	int32 MaxPlayer = 2;

	UFUNCTION()
	void AddPlayerNum();
	void SubPlayerNum();
	void InitCountPlayerUiActor();
};
