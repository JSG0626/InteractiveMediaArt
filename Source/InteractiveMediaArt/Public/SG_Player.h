// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SG_Player.generated.h"


enum ELandmark
{
	NOSE = 0, 
	LEFT_SHOULDER,
	RIGHT_SHOULDER,
	LEFT_ELBOW,
	RIGHT_ELBOW,
	LEFT_WRIST,
	RIGHT_WRIST,
	LEFT_PINKY,
	RIGHT_PINKY,
	LEFT_INDEX,
	RIGHT_INDEX,
	LEFT_THUMB,
	RIGHT_THUMB,
	LEFT_HIP,
	RIGHT_HIP,
	LEFT_KNEE,
	RIGHT_KNEE,
	LEFT_ANKLE,
	RIGHT_ANKLE,
	LEFT_HEEL,
	RIGHT_HEEL,
	LEFT_FOOT_INDEX,
	RIGHT_FOOT_INDEX
};

UCLASS()
class INTERACTIVEMEDIAART_API ASG_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASG_Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UPoseableMeshComponent* PoseableMeshComp;

	UPROPERTY(BlueprintReadOnly)
	class ASG_ServerManager* ServerManager;

	TArray<FString> Landmarks;
	TArray<FName> Bones;

	void InitLandmarkField();
	void InitBones();

	//TArray<TPair<float, float>> TargetJointLocations;
	UPROPERTY()
	TArray<FVector> TargetJointLocations;
	void SetJointPosition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraComponent* SmokeNiagaraComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MeshScale = 5.0f;
};
