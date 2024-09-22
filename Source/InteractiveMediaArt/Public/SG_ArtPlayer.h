// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SG_ArtPlayer.generated.h"

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
class INTERACTIVEMEDIAART_API ASG_ArtPlayer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASG_ArtPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* SceneComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UPoseableMeshComponent* PoseableMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraComponent* SmokeNiagaraOnHeadComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraComponent* SmokeNiagaraOnLHandComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraComponent* SmokeNiagaraOnRHandComp;

	UPROPERTY(Replicated, BlueprintReadOnly)
	class ASG_ServerManager* ServerManager;

	UPROPERTY(ReplicatedUsing = OnRep_Player, EditDefaultsOnly)
	class ACJS_LobbyPlayer* Player;

	UFUNCTION()
	void OnRep_Player();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ASG_ServerManager> ServerManagerFactory;
	TArray<FString> Landmarks;
	TArray<FName> Bones;

	void InitLandmarkField();
	void InitBones();
	void SpawnServerManager();
	//TArray<TPair<float, float>> TargetJointLocations;
	UPROPERTY()
	TArray<FVector> TargetJointLocations;
	void SetJointPosition(const TArray<FVector>& JointPosition);


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector MeshScale = FVector(5, 5, 5);

	void ActiveComponents();
	// --------------------------------------------RPC--------------------------------------------
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetJointPosition(const TArray<FVector>& JointPosition);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SetJointPosition(const TArray<FVector>& JointPosition);

	UFUNCTION(Server, Reliable)
	void ServerRPC_ActiveComponents();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_ActiveComponents();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerRPC_HitLetter(const TArray<struct FBasicParticleData>& Datas);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SpawnServerManager();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SpawnServerManager();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_HitLetter_AddImpulse(class UPrimitiveComponent* HitComp, const FVector& Force);
};
