// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM_ArticleText.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

// Sets default values
ALHM_ArticleText::ALHM_ArticleText()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Static Mesh 컴포넌트 생성
    for (int32 i = 2; i <= MeshCount; i++)
    {
        FString MeshName = FString::Printf(TEXT("%s%03d"), *BaseMeshName, i);
        UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(*MeshName);

        if (MeshComponent)
        {
            // 메시 경로 생성
            FString MeshPath = FString::Printf(TEXT("/Game/ArtProject/HRC/Article/%s.%s"), *MeshName, *MeshName);

            // 메시 에셋 로드
            UStaticMesh* LoadedMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *MeshPath));
            if (LoadedMesh)
            {
                MeshComponent->SetStaticMesh(LoadedMesh);
            }

            MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
            MeshComponent->SetRelativeScale3D(FVector(0.1, 0.1, 0.1));
            MeshComponent->SetVisibility(true);

            MeshComponent->SetGenerateOverlapEvents(true);
            MeshComponent->SetCollisionProfileName(TEXT("Text"));
            MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            MeshComponent->SetSimulatePhysics(true);
            MeshComponent->SetEnableGravity(false);
            MeshComponent->SetMassOverrideInKg(NAME_None, 10.f);
            //MeshComponent->

            // 배열에 추가
            TextComponents.Add(MeshComponent);
        }
    }
}
// Called when the game starts or when spawned
void ALHM_ArticleText::BeginPlay()
{
	Super::BeginPlay();
}

void ALHM_ArticleText::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called every frame
void ALHM_ArticleText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

