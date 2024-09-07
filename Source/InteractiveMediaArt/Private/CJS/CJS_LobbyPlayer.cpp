// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_LobbyPlayer.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ACJS_LobbyPlayer::ACJS_LobbyPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스프링암을 생성해서 루트에 붙이고싶다.
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetRelativeLocation(FVector(0, 40, 80));
	SpringArmComp->TargetArmLength = 200;

	// 카메라를 생성해서 스프링암에 붙이고싶다.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
}

// Called when the game starts or when spawned
void ACJS_LobbyPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// 컨트롤러를 가져와서 캐스팅
	auto* pc = Cast<APlayerController>(Controller);
	//2. 캐스팅 성공했다면 
	if (pc)
	{
		// 3. 그 객체를 이용해서 EnhancedInputLocalPlayerSubsystem을 가져온다.
		UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		// 4. SubSys를 이용해서 AddMappingContext를 한다.
		if (subSys)
		{
			subSys->AddMappingContext(IMC_LobbyPlayer, 0);
		}
	}
}

// Called every frame
void ACJS_LobbyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 회전 방향으로 이동하고싶다.
	// 1. ControlRotation 을 이용해서 Transform을 만들고
	FTransform ttt = FTransform(GetControlRotation());
	Direction = ttt.TransformVector(Direction);
	Direction.Z = 0;
	Direction.Normalize();
	// 2. TransformDirection기능 이용해서 방향을 만들어서
	// 3. 그 방향으로 이동
	AddMovementInput(Direction);
	Direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void ACJS_LobbyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (input)
	{
		input->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ACJS_LobbyPlayer::OnMyActionMove);
		input->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ACJS_LobbyPlayer::OnMyActionLook);
		input->BindAction(IA_Jump, ETriggerEvent::Started, this, &ACJS_LobbyPlayer::OnMyActionJump);
	}
}

void ACJS_LobbyPlayer::OnMyActionMove(const struct FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();
	Direction.X = v.X;
	Direction.Y = v.Y;
	Direction.Normalize();
}

void ACJS_LobbyPlayer::OnMyActionLook(const struct FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();

	AddControllerPitchInput(-v.Y);
	AddControllerYawInput(v.X);
}

void ACJS_LobbyPlayer::OnMyActionJump(const FInputActionValue& Value)
{
	Jump();
}

