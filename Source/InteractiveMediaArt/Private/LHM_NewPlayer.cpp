//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "LHM_NewPlayer.h"
//#include "AimPoint.h"
//#include "ButtonExp.h"
//#include "SG_ArtPlayer.h"
//
//ALHM_NewPlayer::ALHM_NewPlayer()
//{
//	
//
//
//
//}
//
//void ALHM_NewPlayer::BeginPlay()
//{
//	Super::BeginPlay();
//
//
//	AimpoiontUI = CreateWidget<UAimPoint>(GetWorld(), WBP_aimpoint);
//	if (AimpoiontUI)
//	{
//		AimpoiontUI->AddToViewport();
//	}
//}
//
//void ALHM_NewPlayer::Tick(float DeltaTime)
//{
//	
//}
//
//void ALHM_NewPlayer::OnMouseClick(const struct FInputActionInstance& Instance)
//{
//	FVector start = FollowCamera->GetComponentLocation();
//	FVector end = start + FollowCamera->GetForwardVector() * 1000.f;
//	FHitResult outhit;
//	if (GetWorld()->LineTraceSingleByChannel(outhit, start, end, ECollisionChannel::ECC_Visibility))
//	{
//		auto* hitActor = outhit.GetActor();
//		AButtonExp* buttonexp = CastChecked<AButtonExp>(hitActor);
//		if (buttonexp != nullptr)
//		{
//			GetWorld()->SpawnActor<ASG_ArtPlayer>(ASG_ArtPlayer::StaticClass(), buttonexp->TargetTransform);
//			//GetWorld()->GetFirstPlayerController()->SetViewTarget(Cast<AActor>(buttonexp->TargetCamera));
//		}
//	}
//}	
//
//void ALHM_NewPlayer::OnMouseRelease(const struct FInputActionInstance& Instance)
//{
//
//}
