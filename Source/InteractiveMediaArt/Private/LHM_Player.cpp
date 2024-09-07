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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	
}

// Called when the game starts or when spawned
void ALHM_Player::BeginPlay()
{
	Super::BeginPlay();

	//ShowMouseCursor();


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
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALHM_Player::Look);

		EnhancedInputComponent->BindAction(LeftMouseButtonAction, ETriggerEvent::Started, this, &ALHM_Player::OnMouseClick);



		EnhancedInputComponent->BindAction(LeftMouseButtonAction, ETriggerEvent::Completed, this, &ALHM_Player::OnMouseRelease);


	}
}



void ALHM_Player::OnMouseClick(const struct FInputActionInstance& Instance)
{
	//isMouseButtonDown = true;

	FVector start = FollowCamera->GetComponentLocation();
	FVector end = start + FollowCamera->GetForwardVector() * 1000.f;
	FHitResult outhit;
	if (GetWorld()->LineTraceSingleByChannel(outhit, start, end, ECollisionChannel::ECC_Visibility))
	{
		auto* hitActor = outhit.GetActor();
		AButtonExp* buttonexp = CastChecked<AButtonExp>(hitActor);
		if (buttonexp != nullptr)
		{
			GetWorld()->SpawnActor<ASG_ArtPlayer>(ASG_ArtPlayer::StaticClass(), buttonexp->TargetTransform);
			GetWorld()->GetFirstPlayerController()->SetViewTarget(Cast<AActor>(buttonexp->TargetCamera));

			//buttonexp->AimpointUI->RemoveFromViewport();

			ShowMouseCursor();
			ShowEscapeUI();
		}
	}
	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 1);
}

void ALHM_Player::OnMouseRelease(const struct FInputActionInstance& Instance)
{
	//isMouseButtonDown = false;
}

void ALHM_Player::ShowEscapeUI()
{	
	EscapeUI = CreateWidget<UEscapeUI>(GetWorld(), WBP_EscapeUI);
	if (EscapeUI != nullptr && WBP_EscapeUI != nullptr)
	{
		EscapeUI->AddToViewport(true);
	}
}

void ALHM_Player::ShowMouseCursor()
{
	if (APlayerController* pc = CastChecked<APlayerController>(GetController()))
	{
		pc->bShowMouseCursor = true;
		pc->bEnableMouseOverEvents = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		pc->SetInputMode(InputMode);
	}
}

