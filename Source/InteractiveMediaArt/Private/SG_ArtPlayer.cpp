// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_ArtPlayer.h"
#include "Components/PoseableMeshComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SG_ServerManager.h"
#include "Components/SceneComponent.h"
#include "CJS/CJS_LobbyPlayer.h"
#include <InteractiveMediaArt/InteractiveMediaArt.h>
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraDataInterfaceExport.h"
// Sets default values
ASG_ArtPlayer::ASG_ArtPlayer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	//SceneComp->SetupAttachment(RootComponent);

	PoseableMeshComp = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMeshComp"));
	PoseableMeshComp->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	if (tempMesh.Succeeded())
	{
		PoseableMeshComp->SetSkeletalMesh(tempMesh.Object);
		PoseableMeshComp->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
		PoseableMeshComp->SetVisibility(false);
	}

	SmokeNiagaraOnHeadComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeNiagaraOnHeadComp"));
	//SmokeNiagaraComp->SetupAttachment(RootComponent);
	SmokeNiagaraOnHeadComp->SetupAttachment(PoseableMeshComp, FName("headSmokeSocket"));
	SmokeNiagaraOnHeadComp->SetRelativeLocation(FVector(0));

	SmokeNiagaraOnLHandComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeNiagaraOnLHandComp"));
	//SmokeNiagaraComp->SetupAttachment(RootComponent);
	SmokeNiagaraOnLHandComp->SetupAttachment(PoseableMeshComp, FName("hand_l_SmokeSocket"));
	SmokeNiagaraOnLHandComp->SetRelativeLocation(FVector(0));

	SmokeNiagaraOnRHandComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeNiagaraOnRHandComp"));
	//SmokeNiagaraComp->SetupAttachment(RootComponent);
	SmokeNiagaraOnRHandComp->SetupAttachment(PoseableMeshComp, FName("hand_r_SmokeSocket"));
	SmokeNiagaraOnRHandComp->SetRelativeLocation(FVector(0));

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempSmokeNiagara(TEXT("/Script/Niagara.NiagaraSystem'/Game/ArtProject/LHM/Effects/P_Smoke_01.P_Smoke_01'"));
	if (tempSmokeNiagara.Succeeded())
	{
		SmokeNiagaraOnHeadComp->SetAsset(tempSmokeNiagara.Object);
		SmokeNiagaraOnLHandComp->SetAsset(tempSmokeNiagara.Object);
		SmokeNiagaraOnRHandComp->SetAsset(tempSmokeNiagara.Object);
	}
	SmokeNiagaraOnHeadComp->bAutoActivate = false;
	SmokeNiagaraOnLHandComp->bAutoActivate = false;
	SmokeNiagaraOnRHandComp->bAutoActivate = false;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ASG_ArtPlayer::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("ASG_ArtPlayer::BeginPlay()"));
	PoseableMeshComp->SetRelativeScale3D(MeshScale);
	//SmokeNiagaraComp->SetWorldLocation(PoseableMeshComp->GetBoneLocation(FName("head")));
	MeshScale = FVector(3.5, 7, 3.5);
	InitLandmarkField();
	InitBones();

	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, [&]()
		{
			ActiveComponents();
		}, 2.0f, false);
}

void ASG_ArtPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (nullptr != ServerManager)
	{
		ServerManager->Destroy();
	}

	PRINTLOG(TEXT(""));
}

// Called every frame
void ASG_ArtPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SmokeNiagaraComp->SetWorldLocation(PoseableMeshComp->GetBoneLocation(FName("head")));

}

void ASG_ArtPlayer::InitLandmarkField()
{
	Landmarks.Add(TEXT("NOSE"));
	Landmarks.Add(TEXT("LEFT_SHOULDER"));
	Landmarks.Add(TEXT("RIGHT_SHOULDER"));
	Landmarks.Add(TEXT("LEFT_ELBOW"));
	Landmarks.Add(TEXT("RIGHT_ELBOW"));
	Landmarks.Add(TEXT("LEFT_WRIST"));
	Landmarks.Add(TEXT("RIGHT_WRIST"));
	Landmarks.Add(TEXT("LEFT_PINKY"));
	Landmarks.Add(TEXT("RIGHT_PINKY"));
	Landmarks.Add(TEXT("LEFT_INDEX"));
	Landmarks.Add(TEXT("RIGHT_INDEX"));
	Landmarks.Add(TEXT("LEFT_THUMB"));
	Landmarks.Add(TEXT("RIGHT_THUMB"));
	Landmarks.Add(TEXT("LEFT_HIP"));
	Landmarks.Add(TEXT("RIGHT_HIP"));
	Landmarks.Add(TEXT("LEFT_KNEE"));
	Landmarks.Add(TEXT("RIGHT_KNEE"));
	Landmarks.Add(TEXT("LEFT_ANKLE"));
	Landmarks.Add(TEXT("RIGHT_ANKLE"));
	Landmarks.Add(TEXT("LEFT_HEEL"));
	Landmarks.Add(TEXT("RIGHT_HEEL"));
	Landmarks.Add(TEXT("LEFT_FOOT_INDEX"));
	Landmarks.Add(TEXT("RIGHT_FOOT_INDEX"));
}

void ASG_ArtPlayer::InitBones()
{
	Bones.Add(TEXT("head"));
	Bones.Add(TEXT("upperarm_l"));
	Bones.Add(TEXT("upperarm_r"));
	Bones.Add(TEXT("lowerarm_l"));
	Bones.Add(TEXT("lowerarm_r"));
	Bones.Add(TEXT("hand_l"));
	Bones.Add(TEXT("hand_r"));
	Bones.Add(TEXT("pinky_01_l"));
	Bones.Add(TEXT("pinky_01_r"));
	Bones.Add(TEXT("index_01_l"));
	Bones.Add(TEXT("index_01_r"));
	Bones.Add(TEXT("thumb_01_l"));
	Bones.Add(TEXT("thumb_01_r"));
	Bones.Add(TEXT("thigh_l"));
	Bones.Add(TEXT("thigh_r"));
	Bones.Add(TEXT("calf_l"));
	Bones.Add(TEXT("calf_r"));
	Bones.Add(TEXT("foot_l"));
	Bones.Add(TEXT("foot_r"));
	Bones.Add(TEXT("ball_l"));
	Bones.Add(TEXT("ball_r"));
	Bones.Add(TEXT("MyBoneEMPTY"));
	Bones.Add(TEXT("MyBoneEMPTY"));
}

void ASG_ArtPlayer::SpawnServerManager()
{
	check(ServerManagerFactory); if (nullptr == ServerManagerFactory) return;

	PRINTLOG(TEXT("SpawnManager"));
	ServerManager = GetWorld()->SpawnActor<ASG_ServerManager>(ServerManagerFactory);
	if (nullptr == ServerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs is nullptr"), GET_NAME(ServerManager));
		return;
	}
	ServerManager->SetOwner(GetOwner());
	ServerManager->Me = this;

	ServerManager->Active();
	/*if (Me->IsLocallyControlled())
	{

	}*/
}

void ASG_ArtPlayer::SetJointPosition(const TArray<FVector>& JointPosition)
{
	PRINTLOG(TEXT("SetJointPosition"));
	ServerRPC_SetJointPosition(JointPosition);
}

void ASG_ArtPlayer::ActiveComponents()
{
	ServerRPC_ActiveComponents();
}

void ASG_ArtPlayer::ServerRPC_SetJointPosition_Implementation(const TArray<FVector>& JointPosition)
{
	PRINTLOG(TEXT("ServerRPC_SetJointPosition_Implementation"));
	MulticastRPC_SetJointPosition(JointPosition);
}

void ASG_ArtPlayer::MulticastRPC_SetJointPosition_Implementation(const TArray<FVector>& JointPosition)
{
	check(PoseableMeshComp); if (nullptr == PoseableMeshComp) return;
	PRINTLOG(TEXT("MulticastRPC_SetJointPosition_Implementation"));
	FVector CurLocation = GetActorLocation();
	CurLocation.Z += 500;
	for (int32 i = 0; i < JointPosition.Num(); i++)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s %s"), *Bones[i].ToString(), *Landmarks[i]));

		float x = JointPosition[i].X * MeshScale.X;
		//float y = TargetJointLocations[i].Y * MeshScale.Y;
		float z = JointPosition[i].Z * MeshScale.Z;

		// 관절의 본을 가져옵니다.
		FTransform JointTransform = PoseableMeshComp->GetBoneTransform(PoseableMeshComp->GetBoneIndex(Bones[i]));
		FVector JointLocation = JointTransform.GetLocation();
		// 새로운 위치를 설정합니다.
		//FVector newLocation = FVector(TargetJointLocations[i].Key, JointTransform.GetLocation().Y, TargetJointLocations[i].Value);
		FVector newLocation = CurLocation + FVector(x, 0, z);
		newLocation.Y = CurLocation.Y;
		// 본의 변환을 설정합니다.
		PoseableMeshComp->SetBoneLocationByName(Bones[i], newLocation, EBoneSpaces::WorldSpace);
	}

	FVector spine_04_loc = CurLocation + (JointPosition[ELandmark::LEFT_SHOULDER] + JointPosition[ELandmark::RIGHT_SHOULDER] +
		JointPosition[ELandmark::LEFT_HIP] + JointPosition[ELandmark::RIGHT_HIP]) / 4 * MeshScale;
	spine_04_loc.Y = CurLocation.Y;
	PoseableMeshComp->SetBoneLocationByName(TEXT("spine_04"), spine_04_loc, EBoneSpaces::WorldSpace);

	FVector pelvis_loc = CurLocation + (JointPosition[ELandmark::LEFT_HIP] + JointPosition[ELandmark::RIGHT_HIP]) / 2 * MeshScale;
	pelvis_loc.Y = CurLocation.Y;
	PoseableMeshComp->SetBoneLocationByName(TEXT("pelvis"), pelvis_loc, EBoneSpaces::WorldSpace);

	GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("curLocation: %s, pelvis_loc: %s"), *CurLocation.ToString(), *pelvis_loc.ToString()));
}

void ASG_ArtPlayer::ServerRPC_ActiveComponents_Implementation()
{
	MulticastRPC_ActiveComponents();
}

void ASG_ArtPlayer::MulticastRPC_ActiveComponents_Implementation()
{
	PoseableMeshComp->SetVisibility(true);
	SmokeNiagaraOnHeadComp->Activate(true);
	SmokeNiagaraOnLHandComp->Activate(true);
	SmokeNiagaraOnRHandComp->Activate(true);
	UE_LOG(LogTemp, Warning, TEXT("PoseableMeshComp->Setvisibility: %d"), PoseableMeshComp->GetVisibleFlag());
}

void ASG_ArtPlayer::ServerRPC_HitLetter_Implementation(const TArray<FBasicParticleData>& Datas)
{
	
	const float SphereTraceRadius = 100;
	const float ForceMinValue = -300;
	const float ForceMaxValue = 300;
	const float ForceZValue = 500;

	for (auto data : Datas)
	{
		float X = data.Position.X;
		float Y = data.Position.Y;
		float Z = data.Position.Z;

		ETraceTypeQuery traceChannel = ETraceTypeQuery::TraceTypeQuery3;
		TArray<AActor*> actorsToIgnore;
		TArray<FHitResult> hitInfos;
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), FVector(X, Y + 50, Z), FVector(X, Y - 100, Z), SphereTraceRadius, traceChannel,
		false, actorsToIgnore, EDrawDebugTrace::None, hitInfos, true);

		for (auto hitInfo : hitInfos)
		{
			auto hitComp = hitInfo.GetComponent();
			if (hitComp)
			{
				FVector force = FVector(FMath::FRandRange(ForceMinValue, ForceMaxValue), 0, ForceZValue);
				hitComp->AddImpulse(force);
			}
		}
	}
}


