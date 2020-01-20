// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CULTYGAME_API AGameplayGameMode : public AGameModeBase
{
	GENERATED_BODY()

/*
*	Set up in GameMode, because later on when the player picks up an item, we need to add it to the player's inventory,
*	and since we're adding it by the ID we actually need the item database. 
*	The item data base is set within 'BP_GameplayGameMode'
*/ 
public:

	class UDataTable* GetItemDatabase() const { return ItemDatabase; }

protected:
	UPROPERTY(EditDefaultsOnly)
	class UDataTable* ItemDatabase;
	
};
