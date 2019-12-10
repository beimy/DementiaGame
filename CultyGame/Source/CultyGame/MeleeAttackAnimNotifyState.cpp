// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAttackAnimNotifyState.h"

#include "MeleeAttackSystem.h"

#include "Engine.h"
#include "GameFramework/Character.h"

void UMeleeAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) // MeshComp = Actor animation plays from // Animation = The animation that's going to play from Actor
{
	
	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, __FUNCTION__);

	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		// Cast to UAnimNotify from UMeleeAttackComponent and get the owning Actor of that component
		UMeleeAttackSystem* MeleeAttackSystemComponent = Cast<UMeleeAttackSystem>(MeshComp->GetOwner()); 
		if (MeleeAttackSystemComponent != NULL)
		{
			//MeleeAttackSystemComponent->MeleeAttackStart();
		}
	}
	
}

void UMeleeAttackAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	
	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Yellow, __FUNCTION__);

	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		// Cast to UAnimNotify from UMeleeAttackComponent and get the owning Actor of that component
		UMeleeAttackSystem* MeleeAttackSystemComponent = Cast<UMeleeAttackSystem>(MeshComp->GetOwner());
		if (MeleeAttackSystemComponent != NULL)
		{
			//MeleeAttackSystemComponent->MeleeAttackStart();
		}
	}
	
}

void UMeleeAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	
	if (GEngine != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, __FUNCTION__);
	}
	
}