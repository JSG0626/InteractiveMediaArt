// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_PopUpBnt.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "../InteractiveMediaArt.h"
#include "CJS/CJS_LobbyPlayer.h"


// Sets default values
ACJS_PopUpBnt::ACJS_PopUpBnt()
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
	//VisibleBoxComp->SetRelativeLocation(FVector(-585, 260, 2140));
	//VisibleBoxComp->SetRelativeScale3D(FVector(17.5f, 2.5f, 75));
	VisibleBoxComp->SetCollisionProfileName(TEXT("OverlapAll"));
	VisibleBoxComp->SetGenerateOverlapEvents(true);

	//VisibleBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACJS_PopUpBnt::OnOverlapBegin);
	//VisibleBoxComp->OnComponentEndOverlap.AddDynamic(this, &ACJS_PopUpBnt::OnOverlapEnd);

	// 위젯 컴포넌트 생성 (3D 월드에서 배치되는 위젯)
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetupAttachment(ButtonComp);
	WidgetComp->SetWidgetSpace(EWidgetSpace::World); // 3D 공간에 배치
	WidgetComp->SetRelativeLocation(FVector(-585, -60, 800)); // 위젯 위치 조정 
	WidgetComp->SetRelativeRotation(FRotator(270, 90, 0)); // 위젯 위치 조정
	WidgetComp->SetRelativeScale3D(FVector(1, 1.5, 10));
	WidgetComp->SetDrawSize(FVector2D(1000, 1000)); // 위젯 크기 설정
	WidgetComp->SetVisibility(false); // 처음에는 보이지 않게 설정


}

// Called when the game starts or when spawned
void ACJS_PopUpBnt::BeginPlay()
{
	Super::BeginPlay();
	
	// 팝업 UI 위젯 설정
	if (PopUpUIClass)
	{
		WidgetComp->SetWidgetClass(PopUpUIClass); // 위젯 클래스를 설정
		WidgetComp->SetVisibility(false); // 처음엔 숨김 상태
		UE_LOG(LogTemp, Warning, TEXT("Widget class set and initialized"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PopUpUIClass is not set in the editor!"));
	}
}

// Called every frame
void ACJS_PopUpBnt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void ACJS_PopUpBnt::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	PRINTLOG(TEXT("OnOverlapBegin"));
//	if (OtherActor)
//	{
//		auto* player = Cast<ACJS_LobbyPlayer>(OtherActor);
//		if (player)
//		{
//			auto* pc = Cast<APlayerController>(player->Controller);
//			if (pc)
//			{
//				if (GetWorld()->GetFirstPlayerController() == pc)
//				{
//					ButtonComp->SetVisibility(true);
//					UE_LOG(LogTemp, Warning, TEXT("ACJS_PopUpBnt::OnOverlapBegin"));
//				}
//			}
//
//		}
//	
//	}
//
//	//if ((OtherActor == this) && OtherComp)
//	//{
//	//	ACharacter* pc = Cast<ACharacter>(OtherActor);
//	//	if (pc)
//	//	{
//	//		ButtonComp->SetVisibility(true);
//	//		//WidgetComp->SetVisibility(true);
//	//		UE_LOG(LogTemp, Warning, TEXT("Overlap Begin - Button shown"));
//	//	}
//	//}
//}
//
//void ACJS_PopUpBnt::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//
//	PRINTLOG(TEXT("OnOverlapEnd"));
//	if (OtherActor)
//	{
//		auto* player = Cast<ACJS_LobbyPlayer>(OtherActor);
//		if (player)
//		{
//			auto* pc = Cast<APlayerController>(player->Controller);
//			if (pc)
//			{
//				if (GetWorld()->GetFirstPlayerController() == pc)
//				{
//					ButtonComp->SetVisibility(false);
//					UE_LOG(LogTemp, Warning, TEXT("ACJS_PopUpBnt::OnOverlapEnd"));
//				}
//			}
//		}
//	}
//
//	//if (OtherActor && (OtherActor == this) && OtherComp)
//	//{
//	//	ACharacter* pc = Cast<ACharacter>(OtherActor);
//	//	if (pc)
//	//	{
//	//		ButtonComp->SetVisibility(false);
//	//		//WidgetComp->SetVisibility(false);
//	//		UE_LOG(LogTemp, Warning, TEXT("Overlap End - Button and Widget hidden"));
//	//	}
//	//}
//}

