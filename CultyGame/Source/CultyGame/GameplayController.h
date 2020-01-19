// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayController.generated.h"

/**
 * 
 */
UCLASS()
class CULTYGAME_API AGameplayController : public APlayerController
{
	GENERATED_BODY()

public:

	// The interactable that the player is currently looking at. This will be equal to nullptr if the player is not looking at something that is interactable.
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class AInteractable* CurrentInteractable;
	
};
