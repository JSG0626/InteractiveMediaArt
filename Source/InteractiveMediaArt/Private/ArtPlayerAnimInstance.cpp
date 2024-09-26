// Fill out your copyright notice in the Description page of Project Settings.


#include "ArtPlayerAnimInstance.h"
#include "SG_ArtPlayer.h"

void UArtPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

}
void UArtPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if ( Me )
	{
		hand_l_position = Me->hand_l_Position;
		hand_r_position = Me->hand_r_Position;
		foot_l_position = Me->foot_l_Position;
		foot_r_position = Me->foot_r_Position;
		head_position = Me->head_Position;
		pelvis_position = Me->pelvis_Position;
		spine5_position = Me->spine5_Position;
		GEngine->AddOnScreenDebugMessage(1, -1, FColor::Green, FString::Printf(TEXT("hand_r: %s hand_l: %s\nfoot_r: %s  foot_l: %s"), *hand_l_position.ToString(),
			*hand_r_position.ToString(), *foot_l_position.ToString(), *foot_r_position.ToString()));
		if ( hand_l_position.Y > 150 )
		{
			UE_LOG(LogTemp, Warning, TEXT("%s %s %s %s"), *hand_l_position.ToString(),
			*hand_r_position.ToString(), *foot_l_position.ToString(), *foot_r_position.ToString());
		}
	}
}

