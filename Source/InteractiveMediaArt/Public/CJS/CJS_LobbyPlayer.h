// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "UObject/CoreNet.h"
#include "CJS_LobbyPlayer.generated.h"

UCLASS()
class INTERACTIVEMEDIAART_API ACJS_LobbyPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACJS_LobbyPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* IMC_LobbyPlayer;
	
	/** InputActions */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Move;
	FVector Direction;
	void OnMyActionMove(const FInputActionValue& Value);
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Look;
	void OnMyActionLook(const FInputActionValue& Value);
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Jump;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* IA_ClickBnt;
	void OnMouseClick(const FInputActionInstance& Value);
	void OnMouseClickRelease(const FInputActionInstance& Value);

	/** Widgets */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PopUpUIClass;
	UPROPERTY()
	class UUserWidget* PopUpUI;
	bool bPopUpUIShowing;

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")  // BlueprintImplementableEvent : cpp에서 블루프린트 함수 호출할 때 / BlueprintCallable : 블루프린트에서 cpp 호출할 때
	void AIChatbot(ACJS_AIChatbotBnt* buttonexp);
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void VoiceRecord(ACJS_AIChatbotBnt* buttonexp);
	// 블루프린트에서 호출 가능한 함수 선언
    UFUNCTION(BlueprintCallable, Category = "File Path")
    static FString GetProjectSavedDir();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class UUserWidget> WBP_aimpoint;
	UPROPERTY(EditDefaultsOnly)
	class UAimPoint* AimpointUI;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf <class UUserWidget> WBP_EscapeUI;
	UPROPERTY()
	class UEscapeUI* EscapeUI;

	void RemoveAimPoint();
	void ShowAimPoint();
	void HideAimPoint();

	void ShowMouseCursor();
	//void HideMouseCursor();

	void ShowEscapeUI();
	void HideEscapeUI();
	void OnExitBnt();
	bool bExitBnt2_1;

	
	// 작품 1의 플레이어
	// ----------------------------------------JSG----------------------------------------

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ASG_ArtPlayer> ArtPlayerFactory;
	UPROPERTY(Replicated)
	class ASG_ArtPlayer* ArtPlayer;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SpawnArtPlayer(FTransform TargetTransform);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SpawnArtPlayer(FTransform TargetTransform);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_ReturnToCamera();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_ShowArt1WinUI();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_ShowArt1LoseUI();

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UUserWidget> WBP_Art1Win;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UUserWidget> WBP_Art1Lose;

	UPROPERTY()
	class UUserWidget* Art1WinUI;

	UPROPERTY()
	class UUserWidget* Art1LoseUI;


	// ----------------------------------------JSG----------------------------------------
	
	void ExitArt();

	UFUNCTION()
	void MoveToArtPos(ACJS_MovePosBnt* button);

	// 작품 1의 멀티 플레이
	UFUNCTION(Server, Reliable)
	void ServerRPC_StartInteraction();

	UFUNCTION(Client, Reliable)
	void ClientRPC_MultiplaySetting();

	// 작품 1의 캔슬 Btn
	UPROPERTY(EditDefaultsOnly, Category = "Buttons")
	TSubclassOf<class ACJS_CancelBtn> CancelButtonFactory;
	UPROPERTY()
	class ACJS_CancelBtn* CancelButton;
	void SpawnCancelButton();
	void OnCancelButtonClicked();
	UFUNCTION(Server, Reliable)
	void ServerRPC_CancelInteraction();
	

	// 작품 2의 요소
	UPROPERTY()
	class UCJS_UIManager* UIManager;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> StartPanelFactory;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> EndPanelFactory;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> QuitUIFactory;
	void StartExperience();
	void EndExperience();

	UPROPERTY()
	class APlayerController* pc;
	//virtual void PossessedBy(AController* NewController) override;

	// 작품 2의 Voice Input
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> BP_VoiceMeterClass;
	UPROPERTY()
	class AActor* VoiceMeter1;
	UPROPERTY()
	class AActor* VoiceMeter2;
	void EnableAudioCapture();   // AudioCapture 활성화
	void DisableAudioCapture();  // AudioCapture 비활성화

	

	


};
