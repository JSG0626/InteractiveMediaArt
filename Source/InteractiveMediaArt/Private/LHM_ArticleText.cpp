// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM_ArticleText.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

// Sets default values
ALHM_ArticleText::ALHM_ArticleText()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComp"));
    RootComp->SetupAttachment(RootComponent);
    bReplicates = true;

    // Static Mesh ������Ʈ ����
    for (int32 i = 1; i <= MeshCount; i++)
    {
        FString MeshName = FString::Printf(TEXT("%s%03d"), *BaseMeshName, i);
        UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(*MeshName);

        if (MeshComponent)
        {
            // �޽� ��� ����
            FString MeshPath = FString::Printf(TEXT("/Game/ArtProject/HRC/Article/Article_2/%s.%s"), *MeshName, *MeshName);
            //UE_LOG(LogTemp,Warning,TEXT("%s"),*MeshPath);

            // �޽� ���� �ε�
            UStaticMesh* LoadedMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *MeshPath));
            if (LoadedMesh)
            {
                MeshComponent->SetStaticMesh(LoadedMesh);
            }

            MeshComponent->SetupAttachment(RootComp);
            MeshComponent->SetRelativeScale3D(FVector(1, 10, 1));
            MeshComponent->SetVisibility(true);
            MeshComponent->SetCastShadow(false);

            MeshComponent->SetGenerateOverlapEvents(true);
            MeshComponent->SetCollisionProfileName(TEXT("Text"));
            MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            MeshComponent->SetSimulatePhysics(true);
            MeshComponent->SetEnableGravity(false);
            MeshComponent->SetMassOverrideInKg(NAME_None, 10.f);
            // JSG
            //MeshComponent->SetIsReplicated(true);
            //MeshComponent->

            // �迭�� �߰�
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

