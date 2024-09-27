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
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class UArtPlayerAnimInstance* Anim;

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* SceneComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category = VFX)
	TSubclassOf<class UNiagaraSystem> SmokeFactory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraComponent* SmokeNiagaraOnHeadComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraComponent* SmokeNiagaraOnLHandComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraComponent* SmokeNiagaraOnRHandComp;

	UPROPERTY(Replicated, BlueprintReadOnly)
	class ASG_ServerManager* ServerManager;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Player, EditDefaultsOnly)
	class ACJS_LobbyPlayer* Player;

	UPROPERTY(BlueprintReadOnly)
	class APlayerController* pc;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> WBP_Art1_Entrance;

	class UUserWidget* EntranceUI;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> WBP_Art1_Main;

	class USG_Art1_Main* MainUI;

	UPROPERTY()
	class ASG_Art1_Manager* ArtManager;

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
	void SetJointPosition(TArray<FVector>& JointPosition);


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector MeshScale = FVector(5, 5, 5);

	void ActiveComponents();

	void InitEntranceUI();
	void InitMainUI();
	void UpdateMainUI(int32 RestTime);

	inline void PreprocessJointPosition(const FVector& Root, FVector& DestPosition, const FVector& SrcPosition, float x_scale, float z_scale);
	FVector root_Position;

	FVector lowerarm_l_Position;
	FVector lowerarm_r_Position;
	FVector hand_l_Position;
	FVector hand_r_Position;
	FVector foot_l_Position;
	FVector foot_r_Position;
	FVector head_Position;
	FVector spine5_Position;
	FVector pelvis_Position;

	// --------------------------------------------RPC--------------------------------------------
	UFUNCTION(Server, UnReliable)
	void ServerRPC_SetJointPosition(const TArray<FVector>& JointPosition);

	UFUNCTION(NetMulticast, UnReliable)
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

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPC_HitLetter_AddImpulse(class UPrimitiveComponent* HitComp, const FVector& Force);
};
