// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM_ArticleText.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

// Sets default values
ALHM_ArticleText::ALHM_ArticleText()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}
// Called when the game starts or when spawned
void ALHM_ArticleText::BeginPlay()
{
	Super::BeginPlay();
}

void ALHM_ArticleText::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	CreateStaticMesh();
}

// Called every frame
void ALHM_ArticleText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALHM_ArticleText::CreateStaticMesh()
{
    // ���� ������Ʈ ����
    TArray<UActorComponent*> Components;
    GetComponents(Components); // TArray�� ��������

    for (UActorComponent* Component : Components)
    {
        if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Component))
        {
            MeshComp->DestroyComponent();
        }
    }

    for (int32 i = 2; i <= MeshCount; i++)
    {
        // Static Mesh ������Ʈ ����
        UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this);
        if (MeshComponent)
        {
            // �޽� �̸� ����
            FString MeshName = FString::Printf(TEXT("%s%03d"), *BaseMeshName, i);

            // �̸� �ߺ� Ȯ��
            while (FindObject<UStaticMeshComponent>(ANY_PACKAGE, *MeshName))
            {
                i++;
                MeshName = FString::Printf(TEXT("%s%03d"), *BaseMeshName, i);
            }
            
            MeshComponent->Rename(*MeshName);

            // �޽� �ε�
            FString MeshPath = FString::Printf(TEXT("/Script/Engine.StaticMesh'/Game/ArtProject/HRC/Article/%s.%s'"), *MeshName, *MeshName);
            UStaticMesh* LoadedMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *MeshPath));
            if (LoadedMesh)
            {
                MeshComponent->SetStaticMesh(LoadedMesh);
            }

            // ������Ʈ �ʱ�ȭ
            MeshComponent->RegisterComponent();
            MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        }
    }
}

