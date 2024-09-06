// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_Player.h"
#include "SG_ServerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PoseableMeshComponent.h"
// Sets default values
ASG_Player::ASG_Player()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PoseableMeshComp = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMeshComp"));
	PoseableMeshComp->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	if (tempMesh.Succeeded())
	{
		PoseableMeshComp->SetSkeletalMesh(tempMesh.Object);
		PoseableMeshComp->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
}

// Called when the game starts or when spawned
void ASG_Player::BeginPlay()
{
	Super::BeginPlay();

	InitLandmarkField();
	InitBones();

	ServerManager = GetWorld()->SpawnActor<ASG_ServerManager>(ASG_ServerManager::StaticClass());

	check(ServerManager);
	if (nullptr == ServerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs is nullptr"), GET_NAME(ServerManager));
		UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
	}

	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("ServerManager Spawn")));
	ServerManager->Me = this;
}

// Called every frame
void ASG_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASG_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASG_Player::InitLandmarkField()
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

void ASG_Player::InitBones()
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
	Bones.Add(TEXT("calf_knee_l"));
	Bones.Add(TEXT("calf_knee_r"));
	Bones.Add(TEXT("foot_l"));
	Bones.Add(TEXT("foot_r"));
	Bones.Add(TEXT("ball_l"));
	Bones.Add(TEXT("ball_r"));
	Bones.Add(TEXT("MyBoneEMPTY"));
	Bones.Add(TEXT("MyBoneEMPTY"));
}

void ASG_Player::SetJointPosition()
{
	check(PoseableMeshComp); if (nullptr == PoseableMeshComp) return;
	const float WORLD_SCALE_X = 180;
	const float WORLD_SCALE_Y = 150;

	for (int32 i = 0; i < TargetJointLocations.Num(); i++)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s %s"), *Bones[i].ToString(), *Landmarks[i]));
		
		float x = -(TargetJointLocations[i].Key - 0.5f);
		float z = 0.5f - TargetJointLocations[i].Value;
		/*
		float x = -(TargetJointLocations[i].Get<0>() - 0.5f);
		float y = 0.5f - TargetJointLocations[i].Get<1>();
		float z = 0.5f - TargetJointLocations[i].Get<2>();
		*/
		x *= WORLD_SCALE_X;
		z *= WORLD_SCALE_Y;
		// 관절의 본을 가져옵니다.
		FTransform JointTransform = PoseableMeshComp->GetBoneTransform(PoseableMeshComp->GetBoneIndex(Bones[i]));
		FVector JointLocation = JointTransform.GetLocation();
		// 새로운 위치를 설정합니다.
		//FVector newLocation = FVector(TargetJointLocations[i].Key, JointTransform.GetLocation().Y, TargetJointLocations[i].Value);
		FVector newLocation = FVector(x, JointLocation.Y, z);
		JointTransform.SetLocation(newLocation);

		// 본의 변환을 설정합니다.
		PoseableMeshComp->SetBoneTransformByName(Bones[i], JointTransform, EBoneSpaces::WorldSpace);
	}

	TargetJointLocations.Empty();
}

