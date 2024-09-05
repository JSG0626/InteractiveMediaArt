// Fill out your copyright notice in the Description page of Project Settings.


#include "NiagaraCallBack.h"
#include "NiagaraDataInterfaceExport.h"

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

void ANiagaraCallBack::ReceiveParticleData(const TArray<FBasicParticleData>& ParticleData)
{
	for (int32 i = 0; i < ParticleData.Num(); ++i)
	{
		// 각 입자에 대한 데이터 가져오기
		const FBasicParticleData& Particle = ParticleData[i];
		
		// 위치와 속도
		FVector Position = Particle.Position;
		FVector Velocity = Particle.Velocity;

		// 콘솔에 출력
        //UE_LOG(LogTemp, Log, TEXT("Particle Index: %d, Position: %s, Velocity: %s"), i, *Position.ToString(), *Velocity.ToString());
		
		// 시작과 끝 위치 설정
		FVector Start = Position;
		FVector End = Velocity; // 속도를 반영한 끝 위치

		// 구형 트레이스 수행
		PerformSphereTrace(Start, End, 10.0f); // 반지름 10.0f
	}
}

void ANiagaraCallBack::PerformSphereTrace(const FVector& Start, const FVector& End, float Radius)
{
	FHitResult HitResult;
	FCollisionQueryParams Params;

	// 트레이스 수행
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(Radius), Params);

	// 디버그용 출력
	if ( bHit ) DrawDebugSphere(GetWorld(), HitResult.Location, Radius, 12, FColor::Red, false, 2.0f);

	if (bHit)
	{
		auto hitcomp = HitResult.GetComponent();
		if (hitcomp && hitcomp->IsSimulatingPhysics())
		{
			float mass = hitcomp->GetMass();
			FVector force = HitResult.ImpactPoint - HitResult.TraceEnd;
			force.Normalize();
			force *= 1.f * mass;
			hitcomp->AddImpulse(force);

			// TimerHandle
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ANiagaraCallBack::SetHitComp, 1.0f, false);
		}
	}
}

void ANiagaraCallBack::SetHitComp()
{
	FHitResult HitResult;
	auto hitcomp = HitResult.GetComponent();
	if (hitcomp && hitcomp->IsSimulatingPhysics())
	{
		hitcomp->SetSimulatePhysics(false);
		hitcomp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		hitcomp->SetVisibility(false);
	}
}

