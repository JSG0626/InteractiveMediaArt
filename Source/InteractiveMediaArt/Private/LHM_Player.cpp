// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM_Player.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputAction.h"
#include "LHM_SphereCollision.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
#include "ButtonExp.h"
#include "SG_ArtPlayer.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"
#include "EscapeUI.h"
#include "AimPoint.h"
#include "Templates/SubclassOf.h"
#include "Art3PlayActor.h"
#include "LHM_MoveArt3Btn.h"



// Sets default values
ALHM_Player::ALHM_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	//FollowCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	
}

// Called when the game starts or when spawned
void ALHM_Player::BeginPlay()
{
	Super::BeginPlay();

	//ShowMouseCursor();
	pc = Cast<APlayerController>(GetController());

}

// Called every frame
void ALHM_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ALHM_Player::Move(const struct FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	this->AddMovementInput(ForwardDirection, MovementVector.Y);
	this->AddMovementInput(RightDirection, MovementVector.X);
}

void ALHM_Player::Look(const struct FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// add yaw and pitch input to controller
	this->AddControllerYawInput(LookAxisVector.X);
	this->AddControllerPitchInput(LookAxisVector.Y);
}

// Called to bind functionality to input
void ALHM_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ALHM_Player::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ALHM_Player::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALHM_Player::Move);

		// Looking
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALHM_Player::Look);




		// ========================================================== Art3 여기서부터 ==========================================================

		
		EnhancedInputComponent->BindAction(LeftMouseButtonAction, ETriggerEvent::Started, this, &ALHM_Player::OnMouseClick);

		// ========================================================== Art3 여기까지 ==========================================================



	}
}

// ========================================================== Art3 여기서부터 ==========================================================

void ALHM_Player::ShowMouseCursor()
{
	//if (APlayerController* pc = CastChecked<APlayerController>(GetController())) 
	if ( pc )
	{
		pc->bShowMouseCursor = true;
		pc->bEnableMouseOverEvents = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		pc->SetInputMode(InputMode);

		// 카메라 회전 비활성화
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;  // 이동할 때 플레이어의 방향이 움직이는 방향을 향하게 함
	}
}

void ALHM_Player::SpawnArt3PlayActor()
{
	TSubclassOf<AActor> Art3PlayActor = AArt3PlayActor::StaticClass();
	AArt3PlayActor* SpawnedActor = GetWorld()->SpawnActor<AArt3PlayActor>(Art3PlayActor, FTransform(FVector(5960, -1570, 960))); 
	//APlayerController* pc = CastChecked<APlayerController>(GetController());
	SpawnedActor->pc = pc;
	SpawnedActor->SetPreviousPosition();

	// 스폰된 액터를 AArt3PlayActor로 캐스팅하고 저장
	SpawnedArt3PlayActor = Cast<AArt3PlayActor>(SpawnedActor);

	if (SpawnedArt3PlayActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnedArt3PlayActor is null!"));
	}
}

// ========================================================== Art3 여기서까지 ==========================================================


void ALHM_Player::OnMouseClick(const struct FInputActionValue& Value)
{
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 1000.f;
	FHitResult Outhit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	// 디버그 선 그리기
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Outhit, Start, End, ECollisionChannel::ECC_Visibility);

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit something!"));

		if (Outhit.Component.IsValid())
		{
			FString HitComponentName = Outhit.Component->GetName();
			UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *HitComponentName);
		}

		AActor* HitActor = Outhit.GetActor();
		auto* hitComp = Outhit.GetComponent();
		if (hitComp)
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s,  %s"), *HitActor->GetName(), *hitComp->GetName()));
		if (HitActor)
		{
			FString HitActorName = HitActor->GetName();
			//UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActorName);





// ========================================================== Art3 여기서부터 ==========================================================

			if (HitActorName.Contains("BTN3_1"))
			{
				ALHM_MoveArt3Btn* btn_Art3Play = Cast<ALHM_MoveArt3Btn>(HitActor);
				if (btn_Art3Play != nullptr)
				{
					
					MoveToArt3(btn_Art3Play);
					SpawnArt3PlayActor();

				}
			}

// ========================================================== Art3 여기까지 ==========================================================

		}
	}
}



// ========================================================== Art3 여기서부터 ==========================================================

void ALHM_Player::MoveToArt3(ALHM_MoveArt3Btn* button)
{
	if (button == nullptr)
	{
		return;
	}

	// 플레이어 컨트롤러 가져오기
	//APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc && button->Art3_TargetCamera)
	{
		// 카메라 뷰 변경
		pc->SetViewTarget(Cast<AActor>(button->Art3_TargetCamera));

		// 입력 모드 변경
		FInputModeUIOnly InputMode;
		pc->SetInputMode(InputMode);

		// 마우스 커서 및 에임 포인트 처리
		//HideAimPoint();
		ShowMouseCursor();
		//ShowEscapeUI();
	}

}

// ========================================================== Art3 여기까지 ==========================================================
