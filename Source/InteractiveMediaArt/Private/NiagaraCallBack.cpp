// Fill out your copyright notice in the Description page of Project Settings.


#include "NiagaraCallBack.h"
#include "NiagaraTypes.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "NiagaraDataSet.h"
#include "NiagaraDataInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraSystem.h"

// Sets default values
ANiagaraCallBack::ANiagaraCallBack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	NiagaraCompnent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
}

// Called when the game starts or when spawned
void ANiagaraCallBack::BeginPlay()
{
	Super::BeginPlay();
	
	// callback
	if (NiagaraCompnent)
	{
		NiagaraCompnent->SetVariableObject("User.BPCallBack",this);
	}
}

// Called every frame
void ANiagaraCallBack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void ANiagaraCallBack::ReceiveParticleData(const struct TArray<FNiagaraParticleData>& ParticleData)
//{
//	for (int32 i = 0; i < ParticleData.GetNumParticles(); ++i)
//	{
//		FVector Position = ParticleData.GetPosition(i);
//		FVector Velocity = ParticleData.GetVelocity(i);
//
//		// 콘솔에 출력
//		UE_LOG(LogTemp, Log, TEXT("Position: %s, Velocity: %s"), *Position.ToString(), *Velocity.ToString());
//	}
//}


