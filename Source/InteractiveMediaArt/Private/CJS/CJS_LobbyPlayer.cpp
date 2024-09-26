// Fill out your copyright notice in the Description page of Project Settings.


#include "CJS/CJS_LobbyPlayer.h"
#include "CJS/CJS_PopUpBnt.h"
#include "CJS/CJS_AIChatbotBnt.h"
#include "CJS/CJS_MovePosBnt.h"
#include "CJS/CJS_MultiPlayBTN.h"
#include "CJS/CJS_CancelBtn.h"
#include "CJS/CJS_CountPlayerUIActor.h"
#include "CJS/CJS_CountPlayerUI.h"
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
#include <InteractiveMediaArt/InteractiveMediaArt.h>
#include "LHM_MoveArt3Btn.h"
#include "Art3PlayActor.h"
#include "LHM_Art3UI.h"




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

	if ( IsLocallyControlled() )
	{
		GetMesh()->SetVisibility(false);
	}
	// 컨트롤러를 가져와서 캐스팅
	pc = Cast<APlayerController>(Controller);
	//2. 캐스팅 성공했다면 
	if ( pc )
	{
		// 3. 그 객체를 이용해서 EnhancedInputLocalPlayerSubsystem을 가져온다.
		UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		// 4. SubSys를 이용해서 AddMappingContext를 한다.
		if ( subSys )
		{
			subSys->AddMappingContext(IMC_LobbyPlayer, 0);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ACJS_LobbyPlayer::BeginPlay()::PlayerController (pc) is null in BeginPlay"));
		return;
	}


	// UI 매니저 인스턴스 생성 및 초기화	
	//if (pc && pc->IsLocalPlayerController()) // APlayerController를 사용하는 경우: IsLocalPlayerController()
	if ( IsLocallyControlled() )  // APawn, ACharacter 클래스의 함수로, 해당 폰이 로컬에서 제어되는지 확인
	{
		UIManager = NewObject<UCJS_UIManager>(this);
		if ( UIManager )
		{
			UIManager->Initialize(GetWorld(), StartPanelFactory, EndPanelFactory, QuitUIFactory, pc);
			//UIManager->Initialize(GetWorld(), StartPanelFactory, EndPanelFactory);
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
	if ( WBP_aimpoint )  // WBP_aimpoint가 올바르게 할당되어 있는지 확인
	{
		AimpointUI = CreateWidget<UAimPoint>(GetWorld(), WBP_aimpoint);
		if ( AimpointUI )
		{
			AimpointUI->AddToViewport(true);
			//AimpointUI->SetVisibility(ESlateVisibility::Hidden);
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
	if ( WBP_EscapeUI )
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
	if ( VoiceMeter1 && VoiceMeter2 )
	{
		//UE_LOG(LogTemp, Warning, TEXT("VoiceMeter Spawn"));

		// VoiceMeter1의 Niagara 컴포넌트 크기 조정
		UNiagaraComponent* NiagaraComponent1 = VoiceMeter1->FindComponentByClass<UNiagaraComponent>();
		if ( NiagaraComponent1 )
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
		if ( NiagaraComponent2 )
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


	// 아트3 UI
	if ( WBP_Art3UI )
	{
		Art3UI = CreateWidget<ULHM_Art3UI>(GetWorld(), WBP_Art3UI);
		Art3UI->AddToViewport();
		Art3UI->SetVisibility(ESlateVisibility::Hidden);
	}


	// 멀티 버튼 스폰
	if ( IsLocallyControlled() )
	{
		//SpawnMultiButton();
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
	if ( input )
	{
		input->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ACJS_LobbyPlayer::OnMyActionMove);
		input->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ACJS_LobbyPlayer::OnMyActionLook);
		input->BindAction(IA_Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
		input->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		input->BindAction(IA_ClickBnt, ETriggerEvent::Started, this, &ACJS_LobbyPlayer::OnMouseClick);
		input->BindAction(IA_ClickBnt, ETriggerEvent::Completed, this, &ACJS_LobbyPlayer::OnMouseClickRelease);

		UE_LOG(LogTemp, Warning, TEXT("Mouse Click Input Bound"));
	}
}

void ACJS_LobbyPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	pc = Cast<APlayerController>(NewController);

	if ( IsLocallyControlled() )
	{
		UIManager = NewObject<UCJS_UIManager>(this);
		if ( UIManager )
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
		UE_LOG(LogTemp, Warning, TEXT("Not locally controlled, UIManager not initialized"));
	}

	//SpawnMultiButton();
}
void ACJS_LobbyPlayer::OnMyActionMove(const struct FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if ( Controller != nullptr )
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

	if ( Controller != nullptr )
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void ACJS_LobbyPlayer::OnMouseClick(const FInputActionInstance& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClick()"));

	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 1000.f;
	FHitResult Outhit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	// 디버그 선 그리기
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Outhit, Start, End, ECollisionChannel::ECC_GameTraceChannel4);

	if ( bHit )
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit something!"));

		if ( Outhit.Component.IsValid() )
		{
			FString HitComponentName = Outhit.Component->GetName();
			UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *HitComponentName);
		}

		AActor* HitActor = Outhit.GetActor();
		auto* hitComp = Outhit.GetComponent();
		if ( hitComp )
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s,  %s"), *HitActor->GetName(), *hitComp->GetName()));
		if ( HitActor )
		{
			FString HitActorName = HitActor->GetName();
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActorName);

			if ( HitActorName.Contains("BTN1_1_Single") )
			{
				//UE_LOG(LogTemp, Warning, TEXT("BTN1_1_Single Clicked"));
				ACJS_MovePosBnt* btn_SinglePlay = Cast<ACJS_MovePosBnt>(HitActor);
				if ( btn_SinglePlay != nullptr )
				{
					//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClick()::btn_SinglePlay is OK"));
					MoveToArtPos(btn_SinglePlay);
				}
			}
			else if ( HitActorName.Contains("BTN1_1_Multi") )
			{
				UE_LOG(LogTemp, Warning, TEXT("BTN1_1_Multi Clicked"));
				ACJS_MultiPlayBTN* btn_MultiPlay = Cast<ACJS_MultiPlayBTN>(HitActor);
				if ( btn_MultiPlay != nullptr )
				{
					//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClick()::btn_MultiPlay is OK"));
					// 
					// 서버에 상호작용 시작을 알림
					ServerRPC_StartInteraction();

					btn_MultiPlay->SetActorHiddenInGame(true);
					btn_MultiPlay->SetActorEnableCollision(false);

					// CancelButton을 스폰하는 함수 호출
					SpawnCancelButton();
				}


				// 히트된 액터가 자신의 멀티 버튼인지 확인
				//if ( HitActor == MultiButton )
				//{
				//	// 서버에 상호작용 시작을 알림
				//	ServerRPC_StartInteraction();

				//	MultiButton->SetActorHiddenInGame(true);
				//	MultiButton->SetActorEnableCollision(false);

				//	// CancelButton을 스폰하는 함수 호출
				//	SpawnCancelButton();
				//}
				//else
				//{
				//	UE_LOG(LogTemp, Warning, TEXT("Hit MultiButton does not belong to this player."));
				//}
			}
			else if ( HitActorName.Contains("CancelBtn") )
			{
				UE_LOG(LogTemp, Warning, TEXT("Cancel_BTN Clicked"));
				ACJS_CancelBtn* btn_Cancel = Cast<ACJS_CancelBtn>(HitActor);
				if ( btn_Cancel != nullptr )
				{
					UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClick()::btn_Cancel is OK"));
					OnCancelButtonClicked();  
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("btn_Cancel is null"));
				}
			}
			else if ( HitActorName.Contains("BTN2_1") )
			{
				bExitBnt2_1 = true;
				//UE_LOG(LogTemp, Warning, TEXT("BTN2_1 Clicked"));
				ACJS_MovePosBnt* buttonArt2 = Cast<ACJS_MovePosBnt>(HitActor);
				if ( buttonArt2 != nullptr )
				{
					if ( !pc )
					{
						pc = Cast<APlayerController>(GetController());
						if ( !pc )
						{
							//UE_LOG(LogTemp, Error, TEXT("PlayerController (pc) is null in OnMouseClick"));
							return;
						}
					}

					pc->SetViewTarget(Cast<AActor>(buttonArt2->Art2_TargetCamera));
					FInputModeUIOnly UIOnlyMode;
					pc->SetInputMode(UIOnlyMode);

					HideAimPoint();
					ShowMouseCursor();

					StartExperience();
					//UIManager->ShowQuitUI();
					EnableAudioCapture();
				}
			}
			else if ( HitActorName.Contains("BTN3_1") )
			{
				//UE_LOG(LogTemp, Warning, TEXT("BTN3_1 Clicked"));
				ALHM_MoveArt3Btn* btn_Art3Play = Cast<ALHM_MoveArt3Btn>(HitActor);
				if ( btn_Art3Play != nullptr )
				{
					if ( Art3UI )
					{
						UE_LOG(LogTemp, Warning, TEXT("Art3UI"));
						HideAimPoint();
						Art3UI->SetVisible();

						// 입력 모드 변경
						FInputModeUIOnly InputMode;
						pc->SetInputMode(InputMode);
					}
					UE_LOG(LogTemp, Warning, TEXT("Hit"));
					FTimerHandle timer1;
					FTimerHandle timer2;
					GetWorld()->GetTimerManager().SetTimer(timer1, FTimerDelegate::CreateUObject(this, &ACJS_LobbyPlayer::MoveToArt3, btn_Art3Play), 10.0f, false);
					GetWorld()->GetTimerManager().SetTimer(timer2, this, &ACJS_LobbyPlayer::SpawnArt3PlayActor, 10.0f, false);
					//MoveToArt3(btn_Art3Play);
					//SpawnArt3PlayActor();
				}
			}
			else if ( HitActorName.Contains("BTN1_2") )
			{
				//UE_LOG(LogTemp, Warning, TEXT("BTN1_2 Clicked"));
				ACJS_PopUpBnt* button2 = Cast<ACJS_PopUpBnt>(HitActor);
				if ( button2 != nullptr )
				{
					//UE_LOG(LogTemp, Warning, TEXT("Show PopUpUI"));
					if ( !bPopUpUIShowing )
					{
						if ( nullptr == button2->WidgetComp ) return;
						button2->WidgetComp->SetVisibility(true);
						bPopUpUIShowing = true;
						//UE_LOG(LogTemp, Warning, TEXT("Overlap Begin - PopUpUIWidget shown"));
					}
					else
					{
						if ( nullptr == button2->WidgetComp ) return;
						button2->WidgetComp->SetVisibility(false);
						bPopUpUIShowing = false;
						//UE_LOG(LogTemp, Warning, TEXT("Overlap Begin - PopUpUIWidget hidden"));
					}

				}
			}
			else if ( HitActorName.Contains("BTN1_3") )
			{
				//UE_LOG(LogTemp, Warning, TEXT("BTN1_3 Clicked"));
				ACJS_AIChatbotBnt* button3 = Cast<ACJS_AIChatbotBnt>(HitActor);
				if ( button3 != nullptr )
				{
					//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClick()::button3 is existed"));
					AIChatbot(button3);
					//UE_LOG(LogTemp, Warning, TEXT("AIChatbot(button3 set"));
				}
				else
				{
					//UE_LOG(LogTemp, Error, TEXT("ACJS_LobbyPlayer::OnMouseClick()::button3 is null"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClick()::Hit Actor is NULL"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClick()::No Hit Detected"));
	}
}


void ACJS_LobbyPlayer::SpawnArt3PlayActor()
{
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::SpawnArt3PlayActor()"));
	TSubclassOf<AActor> Art3PlayActor = AArt3PlayActor::StaticClass();
	AArt3PlayActor* SpawnedActor = GetWorld()->SpawnActor<AArt3PlayActor>(Art3PlayActor, FTransform(FVector(5960, -1570, 960)));
	//APlayerController* pc = CastChecked<APlayerController>(GetController());
	SpawnedActor->pc = pc;
	SpawnedActor->SetPreviousPosition();

	// 스폰된 액터를 AArt3PlayActor로 캐스팅하고 저장
	SpawnedArt3PlayActor = Cast<AArt3PlayActor>(SpawnedActor);

	if ( SpawnedArt3PlayActor == nullptr )
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnedArt3PlayActor is null!"));
	}
}

void ACJS_LobbyPlayer::MoveToArt3(ALHM_MoveArt3Btn* button)
{
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::MoveToArt3()"));
	if ( button == nullptr )
	{
		return;
	}

	// 플레이어 컨트롤러 가져오기
	//APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc && button->Art3_TargetCamera )
	{
		// 카메라 뷰 변경
		pc->SetViewTarget(Cast<AActor>(button->Art3_TargetCamera));

		// 입력 모드 변경
		//FInputModeUIOnly InputMode;
		//pc->SetInputMode(InputMode);

		// 마우스 커서 및 에임 포인트 처리
		HideAimPoint();
		ShowMouseCursor();
		ShowEscapeUI();
	}
}

void ACJS_LobbyPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACJS_LobbyPlayer, ArtPlayer);
}

void ACJS_LobbyPlayer::SpawnArtPlayer(FTransform TargetTransform)
{
	ServerRPC_SpawnArtPlayer_Implementation(TargetTransform);
	ArtPlayer->SpawnServerManager();
	//ArtPlayer->MulticastRPC_SpawnServerManager();
}

void ACJS_LobbyPlayer::ServerRPC_SpawnArtPlayer_Implementation(FTransform TargetTransform)
{
	PRINTLOG(TEXT("ServerRPC_SpawnArtPlayer"));
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	check(ArtPlayerFactory); if ( nullptr == ArtPlayerFactory ) return;
	ArtPlayer = GetWorld()->SpawnActor<ASG_ArtPlayer>(ArtPlayerFactory, TargetTransform, params);
	ArtPlayer->SetReplicates(true);
	ArtPlayer->Player = this;
	ArtPlayer->OnRep_Player();
}

void ACJS_LobbyPlayer::MulticastRPC_ReturnToCamera_Implementation()
{
	if ( pc && pc->IsLocalPlayerController() )
	{
		PRINTLOG(TEXT("if (pc && pc->IsLocalPlayerController() )"));
		pc->SetViewTarget(this);
		FInputModeGameOnly InputModeData;
		pc->SetInputMode(InputModeData);

		ShowAimPoint();
		pc->bShowMouseCursor = false;
		HideEscapeUI();
	}
}

void ACJS_LobbyPlayer::MulticastRPC_ShowArt1WinUI_Implementation()
{
	if ( IsLocallyControlled() )
	{	
		PRINTLOG(TEXT("%s"), *GetName());
		check(WBP_Art1Win); if ( nullptr == WBP_Art1Win ) return;

		Art1WinUI = CreateWidget(GetWorld(), WBP_Art1Win);
		Art1WinUI->AddToViewport();

		FTimerHandle handle;
		GetWorldTimerManager().SetTimer(handle, [&]()
		{
			Art1WinUI->RemoveFromParent();
		}, 5.0f, false);
	}
}

void ACJS_LobbyPlayer::MulticastRPC_ShowArt1LoseUI_Implementation()
{
	if ( IsLocallyControlled() )
	{
		PRINTLOG(TEXT("%s"), *GetName());
		check(WBP_Art1Lose); if ( nullptr == WBP_Art1Lose ) return;

		Art1LoseUI = CreateWidget(GetWorld(), WBP_Art1Lose);
		Art1LoseUI->AddToViewport();

		FTimerHandle handle;
		GetWorldTimerManager().SetTimer(handle, [&]()
		{
			Art1LoseUI->RemoveFromParent();
		}, 5.0f, false);
	}
}

void ACJS_LobbyPlayer::MoveToArtPos(ACJS_MovePosBnt* button)
{
	if ( button == nullptr )
	{
		//UE_LOG(LogTemp, Error, TEXT("MoveToArtPos: button is null"));
		return;
	}

	if ( ArtPlayer == nullptr )
	{
		// ArtPlayer 생성
		SpawnArtPlayer(button->Art1_Single_TargetTransform);
		ClientRPC_MultiplaySetting();

		if ( ArtPlayer )
		{
			UE_LOG(LogTemp, Warning, TEXT("ArtPlayer spawned successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn ArtPlayer"));
			return;
		}

		if ( pc && button->Art1_Single_TargetCamera )
		{
			// Art1_Single_TargetCamera의 값 로그 출력
			/*UE_LOG(LogTemp, Warning, TEXT("Art1_Single_TargetCamera: Name = %s, Location = %s"),
			   *button->Art1_Single_TargetCamera->GetName(),
			   *button->Art1_Single_TargetCamera->GetActorLocation().ToString());*/

			   // 카메라 뷰 변경
			pc->SetViewTarget(Cast<AActor>(button->Art1_Single_TargetCamera));

			// 입력 모드 변경
			//FInputModeUIOnly UIOnlyMode;
			//pc->SetInputMode(UIOnlyMode);

			// 마우스 커서 및 에임 포인트 처리
			HideAimPoint();
			if ( pc )
			{
				// 마우스 커서를 보이게 하고 카메라 회전을 비활성화
				pc->bShowMouseCursor = true;
				pc->bEnableMouseOverEvents = true;
				FInputModeUIOnly InputMode;
				pc->SetInputMode(InputMode);

				// 카메라 회전 비활성화
				bUseControllerRotationYaw = false;
			}
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
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClickRelease()"));

	// 카메라 위치에서 라인트레이스를 시작할 위치와 방향 설정
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 1000.f;
	//UE_LOG(LogTemp, Warning, TEXT("라인트레이스 시작 위치: %s, 종료 위치: %s"), *Start.ToString(), *End.ToString());

	// 라인트레이스 설정
	FHitResult Outhit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // 플레이어 자신은 충돌 무시

	// 라인트레이스 실행
	bool bHit = GetWorld()->LineTraceSingleByChannel(Outhit, Start, End, ECollisionChannel::ECC_GameTraceChannel4);
	if ( bHit )
	{
		//UE_LOG(LogTemp, Warning, TEXT("라인트레이스 히트 감지됨"));

		// 충돌한 액터 가져오기
		AActor* HitActor = Outhit.GetActor();
		if ( HitActor )
		{
			FString HitActorName = HitActor->GetName();
			//UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActorName);

			// BNT1_3 버튼 액터인지 확인
			if ( HitActorName.Contains("BTN1_3") )
			{
				//UE_LOG(LogTemp, Warning, TEXT("BNT1_3 버튼 감지됨, 녹음 종료 및 저장 진행"));

				// ACJS_AIChatbotBnt로 캐스팅
				ACJS_AIChatbotBnt* buttonexp = Cast<ACJS_AIChatbotBnt>(HitActor);
				if ( buttonexp != nullptr )
				{
					//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClickRelease::buttonexp는 유효합니다"));
					VoiceRecord(buttonexp);
					//UE_LOG(LogTemp, Warning, TEXT("VoiceRecord(buttonexp) 호출 완료"));
				}
				else
				{
					//UE_LOG(LogTemp, Error, TEXT("ACJS_LobbyPlayer::OnMouseClickRelease::buttonexp 캐스팅 실패"));
				}
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Hit Actor가 BNT1_3이 아님"));
			}
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("ACJS_LobbyPlayer::OnMouseClickRelease::Hit Actor가 NULL"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("ACJS_LobbyPlayer::OnMouseClickRelease::라인트레이스에서 히트 감지되지 않음"));
	}

	//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnMouseClickRelease() 끝"));
}

FString ACJS_LobbyPlayer::GetProjectSavedDir()
{
	FString SavedDir = FPaths::ProjectSavedDir();

	// 로그 출력
	//UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::GetProjectSavedDir()::Project Saved Directory: %s"), *SavedDir);

	return SavedDir;
}

//void ACJS_LobbyPlayer::RemoveAimPoint()
//{
//	// AimPoint UI 끄기
//	if ( AimpointUI != nullptr ) AimpointUI->SetVisibility(ESlateVisibility::Hidden);
//}

void ACJS_LobbyPlayer::ShowMouseCursor()
{
	if ( pc )
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
	if ( nullptr == EscapeUI )
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("ShowEscapeUI is Null")));
		return;
	}
	EscapeUI->SetVisibility(ESlateVisibility::Visible);

	if ( EscapeUI != nullptr && WBP_EscapeUI != nullptr )
	{
		EscapeUI->AddToViewport();
		EscapeUI->Me = this;
	}
}

void ACJS_LobbyPlayer::HideEscapeUI()
{
	if ( nullptr == EscapeUI ) return;
	EscapeUI->SetVisibility(ESlateVisibility::Hidden);

}

void ACJS_LobbyPlayer::OnExitBnt()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnExitBnt()"));
	if ( bExitBnt2_1 )
	{
		DisableAudioCapture();
		//EndExperience();

		if ( pc )
		{
			// 1. 원래 카메라로 복원
			pc->SetViewTarget(this);

			// 2. 입력 모드를 게임 전용으로 설정
			FInputModeGameOnly inputMode;
			pc->bShowMouseCursor = false;
			pc->bEnableMouseOverEvents = false;
			pc->SetInputMode(inputMode);

			// 3. Escape UI 제거
			if ( EscapeUI )
			{
				UIManager->HideQuitUI();
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("UIManager is null in OnExitBnt"));
			}
			//UE_LOG(LogTemp, Warning, TEXT("Returned to LobbyPlayer camera and original state"));

			// 4. AimPoint 다시 표시 (필요하다면)
			//ShowAimPoint();
		}

		bExitBnt2_1 = false;

		ShowAimPoint();
	}
}

void ACJS_LobbyPlayer::ExitArt()
{
	// PlayerController 유효성 검사
	if ( pc )
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
	if ( pc )
	{
		pc->SetViewTarget(this);
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("ExitArt: PlayerController (pc) is null when setting ViewTarget"));
	}

	ShowAimPoint();

	// ArtPlayer 유효성 검사 및 파괴
	if ( ArtPlayer )
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
	if ( BP_VoiceMeterClass )
	{
		//UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeterClass is valid"));

		if ( VoiceMeter1 && VoiceMeter2 )
		{
			//UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeter instance is valid"));

			UAudioCaptureComponent* AudioCapture1 = VoiceMeter1->FindComponentByClass<UAudioCaptureComponent>();
			UAudioCaptureComponent* AudioCapture2 = VoiceMeter2->FindComponentByClass<UAudioCaptureComponent>();
			if ( AudioCapture1 && AudioCapture2 )
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
	if ( BP_VoiceMeterClass )
	{
		//UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeterClass is valid"));

		if ( VoiceMeter1 && VoiceMeter2 )
		{
			//UE_LOG(LogTemp, Warning, TEXT("BP_VoiceMeter instance is valid"));

			UAudioCaptureComponent* AudioCapture1 = VoiceMeter1->FindComponentByClass<UAudioCaptureComponent>();
			UAudioCaptureComponent* AudioCapture2 = VoiceMeter2->FindComponentByClass<UAudioCaptureComponent>();
			if ( AudioCapture1 && AudioCapture2 )
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
	if ( UIManager )
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
	if ( UIManager )
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
	UE_LOG(LogTemp, Warning, TEXT(" ACJS_LobbyPlayer::ServerRPC_StartInteraction_Implementation()"));
	if ( auto GM = GetWorld()->GetAuthGameMode<AIMA_GameModeBase>() )
	{
		if ( GM->CountPlayerUIActor )
		{
			GM->CountPlayerUIActor->ServerRPC_AddPlayerNum(this, 1);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CountPlayerUIActor is null in GameMode."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get GameMode."));
	}
}

void ACJS_LobbyPlayer::ClientRPC_MultiplaySetting_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT(" ACJS_LobbyPlayer::ClientRPC_MultiplaySetting_Implementation()"));

	// PlayerController 초기화
	if ( pc == nullptr )
	{
		pc = Cast<APlayerController>(GetController());
		if ( pc == nullptr )
		{
			UE_LOG(LogTemp, Error, TEXT("ClientRPC_MultiplaySetting: pc is null"));
			return;
		}
	}

	// 클라이언트에서 카메라 액터 생성
	FVector CameraLocation = FVector(2470, -2090, 750);
	FRotator CameraRotation = FRotator(0, -90, 0);

	ACameraActor* LocalTargetCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), CameraLocation, CameraRotation);
	if ( LocalTargetCamera )
	{
		auto* CameraComp = LocalTargetCamera->GetCameraComponent();
		CameraComp->ProjectionMode = ECameraProjectionMode::Orthographic;
		CameraComp->OrthoWidth = 3200.f;

		// 카메라 뷰 변경
		pc->SetViewTarget(LocalTargetCamera);

		// 마우스 커서 및 에임 포인트 처리
		HideAimPoint();
		if ( pc )
		{
			// 마우스 커서를 보이게 하고 카메라 회전을 비활성화
			pc->bShowMouseCursor = true;
			pc->bEnableMouseOverEvents = true;
			FInputModeGameOnly InputMode;
			pc->SetInputMode(InputMode);

			// 카메라 회전 비활성화
			bUseControllerRotationYaw = false;
		}
		UE_LOG(LogTemp, Warning, TEXT("ClientRPC_MultiplaySetting: LocalTargetCamera created and view set"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ClientRPC_MultiplaySetting: Failed to spawn LocalTargetCamera"));
	}
}



void ACJS_LobbyPlayer::SpawnMultiButton()
{
	if ( MultiButtonFactory )
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SpawnLocation = FVector(-140.0f, -800.0f, 140.0f); // 원하는 위치로 설정
		FRotator SpawnRotation = FRotator(0.0f, 90.0f, 0.0f); // 원하는 회전으로 설정

		MultiButton = GetWorld()->SpawnActor<ACJS_MultiPlayBTN>(MultiButtonFactory, SpawnLocation, SpawnRotation, SpawnParams);

		if ( MultiButton )
		{
			UE_LOG(LogTemp, Warning, TEXT("MyMultiButton spawned for %s"), *GetName());
			// 멀티 버튼의 Owner를 설정
			MultiButton->SetOwner(this);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn MyMultiButton for %s"), *GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MultiButtonFactory is not set for %s"), *GetName());
	}
}

void ACJS_LobbyPlayer::SpawnCancelButton()
{
	if ( !IsLocallyControlled() )
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnCancelButton() called on server for %s"), *GetName());
		return;  // 클라이언트에서만 실행
	}

	UE_LOG(LogTemp, Warning, TEXT("SpawnCancelButton() called on client for %s"), *GetName());


	if ( CancelButton == nullptr )
	{
		UE_LOG(LogTemp, Error, TEXT("ACJS_LobbyPlayer::SpawnCancelButton()::CancelButton is null"));
		if ( CancelButtonFactory != nullptr )
		{
			// 스폰 위치와 회전을 지정합니다.
			FVector SpawnLocation = FVector(-170.0f, -650.0f, 140.0f);
			FRotator SpawnRotation = FRotator(0.0f, -360.0f, 160.0f);

			// 스폰 파라미터 설정
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// CancelBtn 스폰
			CancelButton = GetWorld()->SpawnActor<ACJS_CancelBtn>(CancelButtonFactory, SpawnLocation, SpawnRotation, SpawnParams);
			if ( CancelButton )
			{
				CancelButton->ShowCancelBtn();
				UE_LOG(LogTemp, Warning, TEXT("CancelButton spawned at location: %s"), *SpawnLocation.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn ACJS_CancelBtn."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CancelButtonClass is not set."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::SpawnCancelButton()::CancelButton is already exsisted"));
		// 이미 스폰되어 있으면 보이도록 설정
		CancelButton->SetActorHiddenInGame(false);
		CancelButton->SetActorEnableCollision(true);
	}
}

void ACJS_LobbyPlayer::OnCancelButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnCancelButtonClicked()"));

	if ( !IsLocallyControlled() ) return;  // 클라이언트에서만 실행

	UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::OnCancelButtonClicked() for %s"), *GetName());

	// CancelButton 제거
	RemoveCancelButton();

	// 서버에 RPC 호출하여 대기 취소 처리
	ServerRPC_CancelInteraction();
}

void ACJS_LobbyPlayer::RemoveCancelButton()
{
	UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::RemoveCancelButton()"));
	
	// CancelButton 제거
	if ( CancelButton )
	{
		//CancelButton->HideCancelBtn();
		CancelButton->Destroy();
		CancelButton = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("CancelButton destroyed for %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ACJS_LobbyPlayer::RemoveCancelButton()::CancelButton is null"));
	}

	// 멀티 버튼 다시 보이게 설정
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("MultiButton"), FoundActors);

	if ( FoundActors.Num() > 0 )
	{
		ACJS_MultiPlayBTN* btn_MultiPlay = Cast<ACJS_MultiPlayBTN>(FoundActors[0]);
		if ( btn_MultiPlay )
		{
			btn_MultiPlay->SetActorHiddenInGame(false);
			btn_MultiPlay->SetActorEnableCollision(true);
			UE_LOG(LogTemp, Warning, TEXT("MultiButton shown for %s"), *GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No actors with tag 'MultiButton' found for %s."), *GetName());
	}

	// 자신의 멀티 버튼 다시 보이게 설정
	/*if ( MultiButton )
	{
		MultiButton->SetActorHiddenInGame(false);
		MultiButton->SetActorEnableCollision(true);
		UE_LOG(LogTemp, Warning, TEXT("MyMultiButton shown for %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MyMultiButton is null for %s."), *GetName());
	}*/
}


void ACJS_LobbyPlayer::ServerRPC_CancelInteraction_Implementation()
{
	// 서버에서 플레이어를 대기 취소 목록에서 제거
	if ( auto GM = GetWorld()->GetAuthGameMode<AIMA_GameModeBase>() )
	{
		if ( GM->CountPlayerUIActor )
		{
			GM->CountPlayerUIActor->ServerRPC_RemovePlayerNum(this, 1);
		}
	}
}

void ACJS_LobbyPlayer::MulticastRPC_ResetCancelButtonState_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("MulticastRPC_ResetCancelButtonState called for %s"), *GetName());

	// 취소 버튼이 있으면 제거
	RemoveCancelButton();

}

//void ACJS_LobbyPlayer::ClientRPC_ResetCancelButtonState_Implementation()
//{
//	UE_LOG(LogTemp, Warning, TEXT("ClientRPC_CancelButton called for %s"), *GetName());
//
//	// 취소 버튼이 있으면 제거
//	RemoveCancelButton();
//}


void ACJS_LobbyPlayer::ShowAimPoint()
{
	AimpointUI->SetVisibility(ESlateVisibility::Visible);
}
void ACJS_LobbyPlayer::HideAimPoint()
{
	AimpointUI->SetVisibility(ESlateVisibility::Hidden);
}

