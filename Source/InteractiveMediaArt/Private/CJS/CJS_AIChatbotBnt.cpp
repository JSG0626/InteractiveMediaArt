// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_AIChatbotBnt.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

// Sets default values
ACJS_AIChatbotBnt::ACJS_AIChatbotBnt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	ButtonComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonComp"));
	ButtonComp->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempButton(TEXT("/Script/Engine.StaticMesh'/Game/ArtProject/CJS/Blueprints/CJS_Button.CJS_Button'"));
	if (tempButton.Succeeded())
	{
		ButtonComp->SetStaticMesh(tempButton.Object);
		//ButtonComp->SetRelativeScale3D(FVector(0.7, 0.7, 0.1));
		//ButtonComp->SetRelativeRotation(FRotator(0, 90, 0));
		ButtonComp->SetVisibility(false);
		ButtonComp->SetGenerateOverlapEvents(true);
	}

	VisibleBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("VisibleBoxComp"));
	VisibleBoxComp->SetupAttachment(ButtonComp);
	//VisibleBoxComp->SetRelativeLocation(FVector(-585, 390, 2140));
	//VisibleBoxComp->SetRelativeScale3D(FVector(17.5f, 2.5f, 75));
	VisibleBoxComp->SetCollisionProfileName(TEXT("OverlapAll"));
	VisibleBoxComp->SetGenerateOverlapEvents(true);

	//VisibleBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACJS_AIChatbotBnt::OnOverlapBegin);
	//VisibleBoxComp->OnComponentEndOverlap.AddDynamic(this, &ACJS_AIChatbotBnt::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ACJS_AIChatbotBnt::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACJS_AIChatbotBnt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void ACJS_AIChatbotBnt::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor && (OtherActor != this) && OtherComp)
//	{
//		ACharacter* pc = Cast<ACharacter>(OtherActor);
//		if (pc)
//		{
//			ButtonComp->SetVisibility(true);
//		}
//	}
//}
//
//void ACJS_AIChatbotBnt::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	if (OtherActor && (OtherActor != this) && OtherComp)
//	{
//		ACharacter* pc = Cast<ACharacter>(OtherActor);
//		if (pc)
//		{
//			ButtonComp->SetVisibility(false);
//		}
//	}
//}

