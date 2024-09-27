// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CJS_PopUpBnt.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ACJS_PopUpBnt : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACJS_PopUpBnt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* ButtonComp;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* VisibleBoxComp;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class UUserWidget> PopUpUIClass;
	 UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    class UWidgetComponent* WidgetComp;

	//UPROPERTY(EditDefaultsOnly)
	//class UBoxComponent* ClickPopUpUIComp;
	
	//UFUNCTION()
    //void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    //UFUNCTION()
    //void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


};
