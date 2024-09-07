//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
//#include "LHM_NewPlayer.generated.h"
//
///**
// * 
// */
//UCLASS()
//class INTERACTIVEMEDIAART_API ALHM_NewPlayer : public ATP_ThirdPersonCharacter
//{
//	GENERATED_BODY()
//	
//
//public:
//	// Sets default values for this character's properties
//	ALHM_NewPlayer();
//
//protected:
//
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//
//
//public:
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//public:
//
//	UPROPERTY(EditDefaultsOnly)
//	TSubclassOf <class UUserWidget> WBP_aimpoint;
//
//	UPROPERTY(EditDefaultsOnly)
//	class UAimPoint* AimpoiontUI;
//
//	UPROPERTY(EditDefaultsOnly)
//	TSubclassOf <class ASG_ArtPlayer> ArtPlayerFactory;
//	
//	void OnMouseClick(const struct FInputActionInstance& Instance);
//	void OnMouseRelease(const struct FInputActionInstance& Instance);
//};
