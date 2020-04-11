// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackStartNotifyState.h"

#include "CultyGameCharacter.h"

#include "Engine.h"

void UAttackStartNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	// GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, __FUNCTION__);

	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		ACultyGameCharacter* Player = Cast<ACultyGameCharacter>(MeshComp->GetOwner());
		if (Player != NULL)
		{
			Player->AttackStart();
		}
	}
}

void UAttackStartNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{

	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		ACultyGameCharacter* Player = Cast<ACultyGameCharacter>(MeshComp->GetOwner());
		if (Player != NULL)
		{
			Player->InflictDamage();
			//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Orange, __FUNCTION__);
		}
	}
}

void UAttackStartNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, __FUNCTION__);

	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		ACultyGameCharacter* Player = Cast<ACultyGameCharacter>(MeshComp->GetOwner());
		if (Player != NULL)
		{
			Player->AttackEnd();
			//Player->bIsSwinging = false;
		}
	}
}
