// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAttackAnimNotify.h"

#include "Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

void UMeleeAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Orange, __FUNCTION__);

	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		// Cast to player character object
		UMeleeAttackSystem* MeleeAttackSystemComponent = Cast<UMeleeAttackSystem>(MeshComp->GetOwner());
		if (MeleeAttackSystemComponent != NULL)
		{
			//MeleeAttackSystemComponent->MeleeAttack();
		}
	}
	return;
}