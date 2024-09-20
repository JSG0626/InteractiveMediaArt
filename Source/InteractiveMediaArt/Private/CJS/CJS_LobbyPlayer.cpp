// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_LobbyPlayer.h"
#include "CJS/CJS_PopUpBnt.h"
#include "CJS/CJS_AIChatbotBnt.h"
#include "CJS/CJS_MovePosBnt.h"
#include "CJS/CJS_CountPlayerUIActor.h"
#include "CJS/CJS_CountPlayerUI.h"
#include "CJS/CJS_CancelBtn.h"
#include "CJS/CJS_UIManager.h"

#include "ButtonExp.h"
#include "AimPoint.h"
#include "EscapeUI.h"
#include "SG_ArtPlayer.h"
#include "../IMA_GameModeBase.h"

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
#include "Net/UnrealNetwork.h"
#include "AudioCaptureComponent.h"
#include "NiagaraComponent.h"



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
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController (pc) is null in BeginPlay"));
		return;
	}

	// UI 매니저 인스턴스 생성 및 초기화	
	//if (pc && pc->IsLocalPlayerController()) // APlayerController를 사용하는 경우: IsLocalPlayerController()
	if (IsLocallyControlled())  // APawn, ACharacter 클래스의 함수로, 해당 폰이 로컬에서 제어되는지 확인
	{
		UIManager = NewObject<UCJS_UIManager>(this);
		if (UIManager)
		{
			UIManager->Initialize(GetWorld(), StartPanelFactory, EndPanelFactory, QuitUIFactory, pc);
			UE_LOG(LogTemp, Warning, TEXT("UIManager is initialized on local client"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UIManager is not set!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Not locally controlled, UIManager not initialized"));
	}

	// AimPointUI 위젯 생성
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

	// WBP_EscapeUI 위젯 생성
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
		//UE_LOG(LogTemp, Warning, TEXT("VoiceMeter Spawn"));

		// VoiceMeter1의 Niagara 컴포넌트 크기 조정
		UNiagaraComponent* NiagaraComponent1 = VoiceMeter1->FindComponentByClass<UNiagaraComponent>();
		if (NiagaraComponent1)
		{
			NiagaraComponent1->SetWorldScale3D(FVector(3.0f));  // 스케일을 3으로 설정
			//UE_LOG(LogTemp, Warning, TEXT("VoiceMeter1 Niagara Scale Set to 3"));
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("Failed to find NiagaraComponent1 in VoiceMeter1"));
		}

		// VoiceMeter2의 Niagara 컴포넌트 크기 조정
		UNiagaraComponent* NiagaraComponent2 = VoiceMeter2->FindComponentByClass<UNiagaraComponent>();
		if (NiagaraComponent2)
		{
			NiagaraComponent2->SetWorldScale3D(FVector(3.0f));  // 스케일을 3으로 설정
			//UE_LOG(LogTemp, Warning, TEXT("VoiceMeter2 Niagara Scale Set to 3"));
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("Failed to find NiagaraComponent in VoiceMeter2"));
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
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClick()"));

	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 1000.f;
	FHitResult Outhit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	// 디버그 선 그리기
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Outhit, Start, End, ECollisionChannel::ECC_GameTraceChannel4);
	
	if (bHit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Hit something!"));

		if (Outhit.Component.IsValid())
		{
			FString HitComponentName = Outhit.Component->GetName();
			//UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *HitComponentName);
		}

		AActor* HitActor = Outhit.GetActor();
		auto* hitComp = Outhit.GetComponent();
		if (hitComp)
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s,  %s"), *HitActor->GetName(), *hitComp->GetName()));
		if (HitActor)
		{
			FString HitActorName = HitActor->GetName();
			//UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActorName);

			if (HitActorName.Contains("BTN1_1_Single"))
			{
				//UE_LOG(LogTemp, Warning, TEXT("BTN1_1_Single Clicked"));
				ACJS_MovePosBnt* btn_SinglePlay = Cast<ACJS_MovePosBnt>(HitActor);
				if (btn_SinglePlay != nullptr)
				{
					//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClick()::btn_SinglePlay is OK"));
					MoveToArtPos(btn_SinglePlay);
				}
			}
			else if (HitActorName.Contains("BTN1_1_Multi"))
			{
				//UE_LOG(LogTemp, Warning, TEXT("BTN1_1_Multi Clicked"));
				ACJS_MovePosBnt* btn_MultiPlay = Cast<ACJS_MovePosBnt>(HitActor);
				if (btn_MultiPlay != nullptr)
				{
					//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClick()::btn_MultiPlay is OK"));
					ServerRPC_StartInteraction();

					btn_MultiPlay->SetActorHiddenInGame(true);
					btn_MultiPlay->SetActorEnableCollision(false);
					
					// CancelButton을 스폰하는 함수 호출
					SpawnCancelButton();
				}
			}
			else if (HitActorName.Contains("CancelBtn"))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Cancel_BTN Clicked"));
				ACJS_CancelBtn* btn_Cancel = Cast<ACJS_CancelBtn>(HitActor);
				if (btn_Cancel != nullptr)
				{
					//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClick()::btn_Cancel is OK"));
					OnCancelButtonClicked();
				}
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("btn_Cancel is null"));
				}
			}
			else if (HitActorName.Contains("BTN2_1"))
			{
				bExitBnt2_1 = true;
				//UE_LOG(LogTemp, Warning, TEXT("BTN2_1 Clicked"));
				ACJS_MovePosBnt* buttonArt2 = Cast<ACJS_MovePosBnt>(HitActor);
				if (buttonArt2 != nullptr)
				{
					/*GetWorld()->GetFirstPlayerController()->SetViewTarget(Cast<AActor>(buttonArt2->Art2_TargetCamera));
					FInputModeUIOnly UIOnlyMode;
					pc->SetInputMode(UIOnlyMode);

					HideAimPoint();
					ShowMouseCursor();
					UIManager->ShowQuitUI();

					StartExperience();

					EnableAudioCapture();*/

					if (!pc)
					{
						pc = Cast<APlayerController>(GetController());
						if (!pc)
						{
							UE_LOG(LogTemp, Error, TEXT("PlayerController (pc) is null in OnMouseClick"));
							return;
						}
					}

					pc->SetViewTarget(Cast<AActor>(buttonArt2->Art2_TargetCamera));
					FInputModeUIOnly UIOnlyMode;
					pc->SetInputMode(UIOnlyMode);

					HideAimPoint();
					ShowMouseCursor();
					UIManager->ShowQuitUI();
					StartExperience();
					EnableAudioCapture();
				}
			}
			else if (HitActorName.Contains("BTN1_2"))
			{
				//UE_LOG(LogTemp, Warning, TEXT("BTN1_2 Clicked"));
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
			else if (HitActorName.Contains("BTN1_3"))
			{
				//UE_LOG(LogTemp, Warning, TEXT("BTN1_3 Clicked"));
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

void ACJS_LobbyPlayer::MoveToArtPos(ACJS_MovePosBnt* button)
{
	if (button == nullptr)
	{
		//UE_LOG(LogTemp, Error, TEXT("MoveToArtPos: button is null"));
		return;
	}

	if (ArtPlayer == nullptr)
	{
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// ArtPlayer 생성
		ArtPlayer = GetWorld()->SpawnActor<ASG_ArtPlayer>(ArtPlayerFactory, button->Art1_Single_TargetTransform, params);

		// Art1_Single_TargetTransform의 값 로그 출력
		/*UE_LOG(LogTemp, Warning, TEXT("Art1_Single_TargetTransform: Location = %s, Rotation = %s, Scale = %s"),
			*button->Art1_Single_TargetTransform.GetLocation().ToString(),
			*button->Art1_Single_TargetTransform.GetRotation().Rotator().ToString(),
			*button->Art1_Single_TargetTransform.GetScale3D().ToString());*/

		if (ArtPlayer)
		{
			//UE_LOG(LogTemp, Warning, TEXT("ArtPlayer spawned successfully"));
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("Failed to spawn ArtPlayer"));
			return;
		}

		// 플레이어 컨트롤러 가져오기
		//APlayerController* pc = Cast<APlayerController>(GetController());
		if (pc && button->Art1_Single_TargetCamera)
		{
			// Art1_Single_TargetCamera의 값 로그 출력
			/*UE_LOG(LogTemp, Warning, TEXT("Art1_Single_TargetCamera: Name = %s, Location = %s"),
				*button->Art1_Single_TargetCamera->GetName(),
				*button->Art1_Single_TargetCamera->GetActorLocation().ToString());*/

			// 카메라 뷰 변경
			pc->SetViewTarget(Cast<AActor>(button->Art1_Single_TargetCamera));

			// 입력 모드 변경
			FInputModeUIOnly UIOnlyMode;
			pc->SetInputMode(UIOnlyMode);

			// 마우스 커서 및 에임 포인트 처리
			HideAimPoint();
			ShowMouseCursor();
			ShowEscapeUI();
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("MoveToArtPos: PlayerController or TargetCamera is null"));
		}
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
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::GetProjectSavedDir()::Project Saved Directory: %s"), *SavedDir);

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
	UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnExitBnt()"));
	if (bExitBnt2_1)
	{
		DisableAudioCapture();
		EndExperience();

		if (pc)
		{
			// 1. 원래 카메라로 복원
			pc->SetViewTarget(this);
			
			// 2. 입력 모드를 게임 전용으로 설정
			FInputModeGameOnly inputMode;
			pc->SetInputMode(inputMode);
			pc->bShowMouseCursor = false;
			pc->bEnableMouseOverEvents = false;
			
			// 3. Quit UI 제거
			if (UIManager)
			{
				UIManager->HideQuitUI();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UIManager is null in OnExitBnt"));
			}

			// 4. AimPoint 다시 표시 (필요하다면)
			ShowAimPoint();
			UE_LOG(LogTemp, Warning, TEXT("Returned to LobbyPlayer camera and original state"));

			bExitBnt2_1 = false;	
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerController (pc) is null in OnExitBnt"));
		}
	}
}

void ACJS_LobbyPlayer::ExitArt()
{
	// PlayerController 유효성 검사
	if (pc)
	{
		FInputModeGameOnly inputMode;
		pc->bShowMouseCursor = false;
		pc->bEnableMouseOverEvents = false;
		pc->SetInputMode(inputMode);
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("ExitArt: PlayerController (pc) is null"));
	}

	HideEscapeUI();

	// PlayerController 유효성 검사 후 ViewTarget 설정
	if (pc)
	{
		pc->SetViewTarget(this);
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("ExitArt: PlayerController (pc) is null when setting ViewTarget"));
	}

	ShowAimPoint();

	// ArtPlayer 유효성 검사 및 파괴
	if (ArtPlayer)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ExitArt: Destroying ArtPlayer: %s"), *ArtPlayer->GetName());
		ArtPlayer->Destroy();
		ArtPlayer = nullptr; // 포인터 초기화
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("ExitArt: ArtPlayer is null or already destroyed"));
	}
}

void ACJS_LobbyPlayer::EnableAudioCapture()
{
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::EnableAudioCapture()"));
	if (BP_VoiceMeterClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeterClass is valid"));

		if (VoiceMeter1 && VoiceMeter2)
		{
			//UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeter instance is valid"));

			UAudioCaptureComponent* AudioCapture1 = VoiceMeter1->FindComponentByClass<UAudioCaptureComponent>();
			UAudioCaptureComponent* AudioCapture2 = VoiceMeter2->FindComponentByClass<UAudioCaptureComponent>();
			if (AudioCapture1 && AudioCapture2)
			{
				//UE_LOG(LogTemp, Warning, TEXT("AudioCapture component found"));

				// AutoActivate 활성화
				AudioCapture1->bAutoActivate = true;
				AudioCapture2->bAutoActivate = true;

				// 즉시 활성화
				AudioCapture1->Activate();
				AudioCapture2->Activate();
				//UE_LOG(LogTemp, Warning, TEXT("AudioCapture has been activated"));
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("AudioCapture component not found"));
			}
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("BP_VoiceMeter instance is null"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("BP_VoiceMeterClass is null"));
	}
}
void ACJS_LobbyPlayer::DisableAudioCapture()
{
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::DisableAudioCapture()"));
	if (BP_VoiceMeterClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeterClass is valid"));

		if (VoiceMeter1 && VoiceMeter2)
		{
			//UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeter instance is valid"));

			UAudioCaptureComponent* AudioCapture1 = VoiceMeter1->FindComponentByClass<UAudioCaptureComponent>();
			UAudioCaptureComponent* AudioCapture2 = VoiceMeter2->FindComponentByClass<UAudioCaptureComponent>();
			if (AudioCapture1 && AudioCapture2)
			{
				//UE_LOG(LogTemp, Warning, TEXT("AudioCapture component found"));

				// AutoActivate 비활성화
				AudioCapture1->bAutoActivate = false;
				AudioCapture2->bAutoActivate = false;

				// 즉시 비활성화
				AudioCapture1->Deactivate();
				AudioCapture2->Deactivate();
				//UE_LOG(LogTemp, Warning, TEXT("AudioCapture has been deactivated"));
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("AudioCapture component not found"));
			}
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("BP_VoiceMeter instance is null"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("BP_VoiceMeterClass is null"));
	}
}

void ACJS_LobbyPlayer::StartExperience()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::StartExperience()"));
	if (UIManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::StartExperience()::UIManager is OK"));
		UIManager->ShowStartPanel();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ACJS_LobbyPlayer::StartExperience()::UIManager is null"));
	}
}
void ACJS_LobbyPlayer::EndExperience()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::EndExperience()"));
	if (UIManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::EndExperience()::UIManager is OK"));
		UIManager->ShowEndPanel();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ACJS_LobbyPlayer::EndExperience()::UIManager is null"));
	}
}

void ACJS_LobbyPlayer::ServerRPC_StartInteraction_Implementation()
{
	if (auto GM = GetWorld()->GetAuthGameMode<AIMA_GameModeBase>())
	{
		if(GM->CountPlayerUIActor) 
			GM->CountPlayerUIActor->ServerRPC_AddPlayerNum(this, 1); 
	}
}

void ACJS_LobbyPlayer::ClientRPC_MultiplaySetting_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT(" ACJS_LobbyPlayer::ClientRPC_MultiplaySetting_Implementation()"));

	// PlayerController 초기화
	if (pc == nullptr)
	{
		pc = Cast<APlayerController>(GetController());
		if (pc == nullptr)
		{
			//UE_LOG(LogTemp, Error, TEXT("ClientRPC_MultiplaySetting: pc is null"));
			return;
		}
	}

	// 클라이언트에서 카메라 액터 생성
	FVector CameraLocation = FVector(2470, -2090, 750);
	FRotator CameraRotation = FRotator(0, -90, 0);

	ACameraActor* LocalTargetCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), CameraLocation, CameraRotation);
	if (LocalTargetCamera)
	{
		auto* CameraComp = LocalTargetCamera->GetCameraComponent();
		CameraComp->ProjectionMode = ECameraProjectionMode::Orthographic;
		CameraComp->OrthoWidth = 3200.f;

		// 카메라 뷰 변경
		pc->SetViewTarget(LocalTargetCamera);

		// 입력 모드 변경
		FInputModeUIOnly UIOnlyMode;
		pc->SetInputMode(UIOnlyMode);

		// 마우스 커서 및 에임 포인트 처리
		HideAimPoint();
		ShowMouseCursor();
		ShowEscapeUI();

		//UE_LOG(LogTemp, Warning, TEXT("ClientRPC_MultiplaySetting: LocalTargetCamera created and view set"));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("ClientRPC_MultiplaySetting: Failed to spawn LocalTargetCamera"));
	}
}

void ACJS_LobbyPlayer::SpawnCancelButton()
{
	if (CancelButton == nullptr)
	{
		if (CancelButtonFactory != nullptr)
		{
			// 스폰 위치와 회전을 지정합니다.
			FVector SpawnLocation = FVector(-140.0f, -710.0f, 100.0f);
			FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

			// 스폰 파라미터 설정
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// CancelBtn 스폰
			CancelButton = GetWorld()->SpawnActor<ACJS_CancelBtn>(CancelButtonFactory, SpawnLocation, SpawnRotation, SpawnParams);
			if (CancelButton)
			{
				CancelButton->ShowCancelBtn();
				//UE_LOG(LogTemp, Warning, TEXT("CancelButton spawned at location: %s"), *SpawnLocation.ToString());
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("Failed to spawn ACJS_CancelBtn."));
			}
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("CancelButtonClass is not set."));
		}
	}
	else
	{
		// 이미 스폰되어 있으면 보이도록 설정
		CancelButton->SetActorHiddenInGame(false);
	}
}

void ACJS_LobbyPlayer::OnCancelButtonClicked()
{
	// CancelButton 제거
	if (CancelButton)
	{
		//CancelButton->HideCancelBtn();
		CancelButton->Destroy();
		CancelButton = nullptr;
	}

	// 멀티 버튼 다시 보이게 설정
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("MultiButton"), FoundActors);

	if (FoundActors.Num() > 0)
	{
		ACJS_MovePosBnt* btn_MultiPlay = Cast<ACJS_MovePosBnt>(FoundActors[0]);
		if (btn_MultiPlay)
		{
			btn_MultiPlay->SetActorHiddenInGame(false);
			btn_MultiPlay->SetActorEnableCollision(true);
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("No actors with tag 'MultiButton' found."));
	}

	// 서버에 RPC 호출하여 대기 취소 처리
	ServerRPC_CancelInteraction();
}

void ACJS_LobbyPlayer::ServerRPC_CancelInteraction_Implementation()
{
	// 서버에서 플레이어를 대기 취소 목록에서 제거
	if (auto GM = GetWorld()->GetAuthGameMode<AIMA_GameModeBase>())
	{
		if (GM->CountPlayerUIActor)
		{
			GM->CountPlayerUIActor->ServerRPC_RemovePlayerNum(this, 1);
		}
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

