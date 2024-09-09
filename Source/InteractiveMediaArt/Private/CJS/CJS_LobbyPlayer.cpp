// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_LobbyPlayer.h"
#include "CJS/CJS_PopUpBnt.h"
#include "CJS/CJS_AIChatbotBnt.h"

#include "ButtonExp.h"
#include "SG_ArtPlayer.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"
#include "Components/WidgetComponent.h"
#include "CJS/CJS_MovePosBnt.h"
#include "AimPoint.h"
#include "EscapeUI.h"
#include "../../../../Plugins/Runtime/AudioCapture/Source/AudioCapture/Public/AudioCaptureComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"


// Sets default values
ACJS_LobbyPlayer::ACJS_LobbyPlayer()
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

	// 스프링암을 생성해서 루트에 붙이고싶다.
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 0; // The camera follows at this distance behind the character   
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0, 0, 85);

	// 카메라를 생성해서 스프링암에 붙이고싶다.
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

// Called when the game starts or when spawned
void ACJS_LobbyPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 컨트롤러를 가져와서 캐스팅
	pc = Cast<APlayerController>(Controller);
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

	//AimPointUI 위젯 생성
	if (WBP_aimpoint)  // WBP_aimpoint가 올바르게 할당되어 있는지 확인
	{
		AimpointUI = CreateWidget<UAimPoint>(GetWorld(), WBP_aimpoint);
		if (AimpointUI)
		{
			AimpointUI->AddToViewport(true);
			AimpointUI->SetVisibility(ESlateVisibility::Hidden);
			//UE_LOG(LogTemp, Warning, TEXT("AimpointUI successfully created and added to viewport"));
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("Failed to create AimpointUI widget"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("WBP_aimpoint is not assigned! Please assign it in the Blueprint."));
	}

	if (WBP_EscapeUI)
	{
		EscapeUI = CreateWidget<UEscapeUI>(GetWorld(), WBP_EscapeUI);
		EscapeUI->AddToViewport();
		EscapeUI->SetVisibility(ESlateVisibility::Hidden);
	}

	// AudioCapture 설정
	FTransform VoiceMeterTransform1 = FTransform(FRotator(0, 0, 0), FVector(-5705, -1980, 670), FVector(1, 1, 1));
	FTransform VoiceMeterTransform2 = FTransform(FRotator(0, 0, 0), FVector(-5710, -1970, 670), FVector(1, 1, 1));
	VoiceMeter1 = GetWorld()->SpawnActor<AActor>(BP_VoiceMeterClass, VoiceMeterTransform1);
	VoiceMeter2 = GetWorld()->SpawnActor<AActor>(BP_VoiceMeterClass, VoiceMeterTransform2);
	if (VoiceMeter1 && VoiceMeter2)
	{
		UE_LOG(LogTemp, Warning, TEXT("VoiceMeter Spawn"));

		// VoiceMeter1의 Niagara 컴포넌트 크기 조정
		UNiagaraComponent* NiagaraComponent1 = VoiceMeter1->FindComponentByClass<UNiagaraComponent>();
		if (NiagaraComponent1)
		{
			NiagaraComponent1->SetWorldScale3D(FVector(3.0f));  // 스케일을 3으로 설정
			UE_LOG(LogTemp, Warning, TEXT("VoiceMeter1 Niagara Scale Set to 3"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find NiagaraComponent1 in VoiceMeter1"));
		}

		// VoiceMeter2의 Niagara 컴포넌트 크기 조정
		UNiagaraComponent* NiagaraComponent2 = VoiceMeter2->FindComponentByClass<UNiagaraComponent>();
		if (NiagaraComponent2)
		{
			NiagaraComponent2->SetWorldScale3D(FVector(3.0f));  // 스케일을 3으로 설정
			UE_LOG(LogTemp, Warning, TEXT("VoiceMeter2 Niagara Scale Set to 3"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find NiagaraComponent in VoiceMeter2"));
		}

		DisableAudioCapture();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn VoiceMeter1, 2 actor"));
		return;
	}
}

// Called every frame
void ACJS_LobbyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
		input->BindAction(IA_Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
		input->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		input->BindAction(IA_ClickBnt, ETriggerEvent::Started, this, &ACJS_LobbyPlayer::OnMouseClick);
		input->BindAction(IA_ClickBnt, ETriggerEvent::Completed, this, &ACJS_LobbyPlayer::OnMouseClickRelease);

		//UE_LOG(LogTemp, Warning, TEXT("Mouse Click Input Bound"));
	}
}

void ACJS_LobbyPlayer::OnMyActionMove(const struct FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}
void ACJS_LobbyPlayer::OnMyActionLook(const struct FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void ACJS_LobbyPlayer::OnMouseClick(const FInputActionInstance& Value)
{
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 1000.f;
	FHitResult Outhit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Outhit, Start, End, ECollisionChannel::ECC_GameTraceChannel4);
	if (bHit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Hit something!"));
		AActor* HitActor = Outhit.GetActor();
		auto* hitComp = Outhit.GetComponent();
		if (hitComp)
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s,  %s"), *HitActor->GetName(), *hitComp->GetName()));
		if (HitActor)
		{
			FString HitActorName = HitActor->GetName();
			//UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActorName);

			if (HitActorName.Contains("BNT1_1"))
			{
				//UE_LOG(LogTemp, Warning, TEXT("BNT1_1 Clicked"));
				AButtonExp* button1 = Cast<AButtonExp>(HitActor);
				if (button1 != nullptr)
				{
					if (nullptr == ArtPlayer)
					{
						FActorSpawnParameters params;
						params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
						ArtPlayer = GetWorld()->SpawnActor<ASG_ArtPlayer>(ArtPlayerFactory, button1->TargetTransform, params);
						pc->SetViewTarget(Cast<AActor>(button1->TargetCamera));

						FInputModeUIOnly UIOnlyMode;
						pc->SetInputMode(UIOnlyMode);
						HideAimPoint();
						ShowMouseCursor();
						ShowEscapeUI();
					}
				}
			}
			else if (HitActorName.Contains("BNT2_1"))
			{
				bExitBnt2_1 = true;
				//UE_LOG(LogTemp, Warning, TEXT("BNT2_1 Clicked"));
				ACJS_MovePosBnt* buttonArt2 = Cast<ACJS_MovePosBnt>(HitActor);
				if (buttonArt2 != nullptr)
				{
					GetWorld()->GetFirstPlayerController()->SetViewTarget(Cast<AActor>(buttonArt2->TargetCamera));
					FInputModeUIOnly UIOnlyMode;
					pc->SetInputMode(UIOnlyMode);

					HideAimPoint();
					ShowMouseCursor();
					ShowEscapeUI();

					EnableAudioCapture();
				}
			}
			else if (HitActorName.Contains("BNT1_2"))
			{
				//UE_LOG(LogTemp, Warning, TEXT("BNT1_2 Clicked"));
				ACJS_PopUpBnt* button2 = Cast<ACJS_PopUpBnt>(HitActor);
				if (button2 != nullptr)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Show PopUpUI"));
					if (!bPopUpUIShowing)
					{
						if (nullptr == button2->WidgetComp) return;
						button2->WidgetComp->SetVisibility(true);
						bPopUpUIShowing = true;
						//UE_LOG(LogTemp, Warning, TEXT("Overlap Begin - PopUpUIWidget shown"));
					}
					else
					{
						if (nullptr == button2->WidgetComp) return;
						button2->WidgetComp->SetVisibility(false);
						bPopUpUIShowing = false;
						//UE_LOG(LogTemp, Warning, TEXT("Overlap Begin - PopUpUIWidget hidden"));
					}

				}
			}
			else if (HitActorName.Contains("BNT1_3"))
			{
				//UE_LOG(LogTemp, Warning, TEXT("BNT1_3 Clicked"));
				ACJS_AIChatbotBnt* button3 = Cast<ACJS_AIChatbotBnt>(HitActor);
				if (button3 != nullptr)
				{
					AIChatbot(button3);
				}
			}
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Hit Actor is NULL"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("No Hit Detected"));
	}
}

void ACJS_LobbyPlayer::OnMouseClickRelease(const FInputActionInstance& Value)
{
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 1000.f;
	FHitResult Outhit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Outhit, Start, End, ECollisionChannel::ECC_GameTraceChannel4);
	if (bHit)
	{
		AActor* HitActor = Outhit.GetActor();

		if (HitActor)
		{
			FString HitActorName = HitActor->GetName();

			if (HitActorName.Contains("BNT1_3"))
			{
				// BNT1_3을 뗀 상태: 녹음 종료 및 저장
				ACJS_AIChatbotBnt* buttonexp = Cast<ACJS_AIChatbotBnt>(HitActor);
				if (buttonexp != nullptr)
				{
					VoiceRecord(buttonexp);
				}
			}
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Hit Actor is NULL"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("No Hit Detected"));
	}
}

FString ACJS_LobbyPlayer::GetProjectSavedDir()
{
	FString SavedDir = FPaths::ProjectSavedDir();

	// 로그 출력
	UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::GetProjectSavedDir()::Project Saved Directory: %s"), *SavedDir);

	return SavedDir;
}

void ACJS_LobbyPlayer::RemoveAimPoint()
{
	// AimPoint UI 끄기
	if (AimpointUI != nullptr) AimpointUI->SetVisibility(ESlateVisibility::Hidden);
}

void ACJS_LobbyPlayer::ShowMouseCursor()
{
	if (pc)
	{
		// 마우스 커서를 보이게 하고 카메라 회전을 비활성화
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

void ACJS_LobbyPlayer::ShowEscapeUI()
{
	if (nullptr == EscapeUI) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("ShowEscapeUI is Null")));
		return;
	}
	EscapeUI->SetVisibility(ESlateVisibility::Visible);

	if (EscapeUI != nullptr && WBP_EscapeUI != nullptr)
	{
		EscapeUI->AddToViewport();
		EscapeUI->Me = this;
	}
}

void ACJS_LobbyPlayer::HideEscapeUI()
{
	if (nullptr == EscapeUI) return;
	EscapeUI->SetVisibility(ESlateVisibility::Hidden);

}

void ACJS_LobbyPlayer::OnExitBnt()
{
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnExitBnt()"));
	if (bExitBnt2_1)
	{
		DisableAudioCapture();

		if (pc)
		{
			// 1. 원래 카메라로 복원
			pc->SetViewTarget(this);
			
			// 2. 입력 모드를 게임 전용으로 설정
			FInputModeGameOnly inputMode;
			pc->bShowMouseCursor = false;
			pc->bEnableMouseOverEvents = false;
			pc->SetInputMode(inputMode);

			// 3. Escape UI 제거
			if (EscapeUI)
			{
				HideEscapeUI();
			}

			// 4. AimPoint 다시 표시 (필요하다면)
			ShowAimPoint();
			//UE_LOG(LogTemp, Warning, TEXT("Returned to LobbyPlayer camera and original state"));

			bExitBnt2_1 = false;	
		}
	}
}

void ACJS_LobbyPlayer::ExitArt()
{
	FInputModeGameOnly inputMode;
	pc->bShowMouseCursor = false;
	pc->bEnableMouseOverEvents = false;
	pc->SetInputMode(inputMode);

	check(ArtPlayer);
	if (ArtPlayer)
	{
		ArtPlayer->Destroy();
	}
	HideEscapeUI();
	pc->SetViewTarget(this);
	ShowAimPoint();
}

void ACJS_LobbyPlayer::EnableAudioCapture()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::EnableAudioCapture()"));
	if (BP_VoiceMeterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeterClass is valid"));

		if (VoiceMeter1 && VoiceMeter2)
		{
			UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeter instance is valid"));

			UAudioCaptureComponent* AudioCapture1 = VoiceMeter1->FindComponentByClass<UAudioCaptureComponent>();
			UAudioCaptureComponent* AudioCapture2 = VoiceMeter2->FindComponentByClass<UAudioCaptureComponent>();
			if (AudioCapture1 && AudioCapture2)
			{
				UE_LOG(LogTemp, Warning, TEXT("AudioCapture component found"));

				// AutoActivate 활성화
				AudioCapture1->bAutoActivate = true;
				AudioCapture2->bAutoActivate = true;

				// 즉시 활성화
				AudioCapture1->Activate();
				AudioCapture2->Activate();
				UE_LOG(LogTemp, Warning, TEXT("AudioCapture has been activated"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AudioCapture component not found"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BP_VoiceMeter instance is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BP_VoiceMeterClass is null"));
	}
}
void ACJS_LobbyPlayer::DisableAudioCapture()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::DisableAudioCapture()"));
	if (BP_VoiceMeterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeterClass is valid"));

		if (VoiceMeter1 && VoiceMeter2)
		{
			UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeter instance is valid"));

			UAudioCaptureComponent* AudioCapture1 = VoiceMeter1->FindComponentByClass<UAudioCaptureComponent>();
			UAudioCaptureComponent* AudioCapture2 = VoiceMeter2->FindComponentByClass<UAudioCaptureComponent>();
			if (AudioCapture1 && AudioCapture2)
			{
				UE_LOG(LogTemp, Warning, TEXT("AudioCapture component found"));

				// AutoActivate 비활성화
				AudioCapture1->bAutoActivate = false;
				AudioCapture2->bAutoActivate = false;

				// 즉시 비활성화
				AudioCapture1->Deactivate();
				AudioCapture2->Deactivate();
				UE_LOG(LogTemp, Warning, TEXT("AudioCapture has been deactivated"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AudioCapture component not found"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BP_VoiceMeter instance is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BP_VoiceMeterClass is null"));
	}
}

void ACJS_LobbyPlayer::ShowAimPoint()
{
	AimpointUI->SetVisibility(ESlateVisibility::Visible);
}

void ACJS_LobbyPlayer::HideAimPoint()
{
	AimpointUI->SetVisibility(ESlateVisibility::Hidden);
}



//void ACJS_LobbyPlayer::AIChatbot(ACJS_AIChatbotBnt* buttonexp)
//{
//	// AI 챗봇 동작
//	// 블루프린트의 ActivateAIChatbot 함수 호출
//	UFunction* AIChatbotFunction = buttonexp->FindFunction(FName("ActivateAIChatbot"));
//	if (AIChatbotFunction)
//	{
//		buttonexp->ProcessEvent(AIChatbotFunction, nullptr);
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("Could not find ActivateAIChatbot function"));
//	}
//}
//void ACJS_LobbyPlayer::VoiceRecord(ACJS_AIChatbotBnt* buttonexp)
//{
//	// 블루프린트의 ActivateVoiceRecord 함수 호출 (음성 저장)
//	UFunction* StopRecordingFunction = buttonexp->FindFunction(FName("ActivateVoiceRecord"));
//	if (StopRecordingFunction)
//	{
//		buttonexp->ProcessEvent(StopRecordingFunction, nullptr);
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("Could not find ActivateVoiceRecord function"));
//	}
//}
//void ACJS_LobbyPlayer::HideMouseCursor()
//{
//	if (pc)
//	{
//		// 마우스 커서를 숨기고 카메라 회전을 활성화
//		pc->bShowMouseCursor = false;
//		pc->bEnableMouseOverEvents = true;
//		FInputModeGameOnly InputMode;
//		pc->SetInputMode(InputMode);
//
//		// 카메라 회전 활성화
//		bUseControllerRotationYaw = true;
//		GetCharacterMovement()->bOrientRotationToMovement = false;  // 이동할 때 플레이어의 방향을 카메라의 회전에 맞춤
//	}
//}

