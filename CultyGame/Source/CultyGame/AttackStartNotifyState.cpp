// Fill out your copyright notice in the Description page of Project Settings.

/// The Punch - Part 2
#include "AttackStartNotifyState.h"

#include "MeleeAttackSystem.h"

#include "Engine.h"

void UAttackStartNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, __FUNCTION__);

	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		UMeleeAttackSystem* MeleeAttackSystemComponent = Cast<UMeleeAttackSystem>(MeshComp->GetOwner());
		if (MeleeAttackSystemComponent != NULL)
		{
			//MeleeAttackSystemComponent->MeleeAttackStart();
		}
	}
}

void UAttackStartNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, __FUNCTION__);

	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		UMeleeAttackSystem* MeleeAttackSystemComponent = Cast<UMeleeAttackSystem>(MeshComp->GetOwner());
		if (MeleeAttackSystemComponent != NULL)
		{
			//MeleeAttackSystemComponent->MeleeAttackEnd();
		}
	}
}
/// The Punch - Part 2
