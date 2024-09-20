// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM_ArticleText_Single.h"

// Sets default values
ALHM_ArticleText_Single::ALHM_ArticleText_Single()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComp"));
    RootComp->SetupAttachment(RootComponent);

    for (int32 i = 2; i <= MeshCount; i++)
    {
        FString MeshName = FString::Printf(TEXT("%s%03d"), *BaseMeshName, i);
        UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(*MeshName);

        if (MeshComponent)
        {
            FString MeshPath = FString::Printf(TEXT("/Game/ArtProject/HRC/Article/Article_1/%s.%s"), *MeshName, *MeshName);
            UStaticMesh* LoadedMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *MeshPath));
            if (LoadedMesh)
            {
                MeshComponent->SetStaticMesh(LoadedMesh);
            }

            FString MaterialPath = FString::Printf(TEXT("/Game/ArtProject/HRC/Article/Article_2/Material.Material"));
            UMaterial* LoadedMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, *MaterialPath));
            if(LoadedMaterial)
            {
                MeshComponent->SetMaterial(0, LoadedMaterial);
            }

            MeshComponent->SetupAttachment(RootComp);
            //MeshComponent->SetRelativeScale3D(FVector(1, 1, 1));
            MeshComponent->SetVisibility(true);
            MeshComponent->SetCastShadow(false);

            MeshComponent->SetGenerateOverlapEvents(true);
            MeshComponent->SetCollisionProfileName(TEXT("Text"));
            MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            MeshComponent->SetSimulatePhysics(true);
            MeshComponent->SetEnableGravity(false);
            MeshComponent->SetMassOverrideInKg(NAME_None, 10.f);

            TextComponents.Add(MeshComponent);
        }
    }
}

// Called when the game starts or when spawned
void ALHM_ArticleText_Single::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALHM_ArticleText_Single::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

