// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MeleeAttackSystem.h"
#include "MeleeAttackAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class CULTYGAME_API UMeleeAttackAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	// UMeleeAttackSystem* MeleeAttackSystemComponent = nullptr;
};


/*
Step 1:
Make AnimNotify & AnimNotifyState C++ Files

Step 2:
Ctrl+Left Click 'UAnimNotify' in AnimNotify .h file

Step 3:
Scroll down and find 'virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)', copy.

Step 4:
Paste 'virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)' into AnimNotify .h file, append 'override' at the end of this line. 
i.e. 'virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;'

Step 5:
Define 'virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)' into AnimNotify .cpp file, 
i.e. 'void UMeleeAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) { }'

*/