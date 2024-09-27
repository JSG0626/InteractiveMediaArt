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
		lowerarm_l_position = Me->lowerarm_l_Position;
		lowerarm_r_position = Me->lowerarm_r_Position;
		hand_l_position = Me->hand_l_Position;
		hand_r_position = Me->hand_r_Position;
		foot_l_position = Me->foot_l_Position;
		foot_r_position = Me->foot_r_Position;
		head_position = Me->head_Position;
		pelvis_position = Me->pelvis_Position;
		spine5_position = Me->spine5_Position;
		UE_LOG(LogTemp, Warning, TEXT("head: %s, pelvis: %s, spine5: %s, hand_l: %s, hand_r: %s"),
		*head_position.ToString(), *pelvis_position.ToString(), *spine5_position.ToString(), *hand_l_position.ToString(), *hand_r_position.ToString());
	}
}

