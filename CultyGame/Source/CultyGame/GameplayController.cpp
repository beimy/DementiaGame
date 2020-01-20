// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayController.h"
#include "CultyGame.h"
#include "Interactable.h"
#include "GameplayGameMode.h"

void AGameplayController::SetupInputComponent()
{

	Super::SetupInputComponent();

	InputComponent->BindAction("Use", IE_Pressed, this, &AGameplayController::Interact);

}

void AGameplayController::Interact()
{

	// Check if player is looking at an interactable object.
	if (CurrentInteractable)
	{
		CurrentInteractable->Interact(this);
	}

}

void AGameplayController::AddItemToInventoryByID(FName ID)
{

	AGameplayGameMode* GameMode = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode());
	UDataTable* ItemTable = GameMode->GetItemDatabase(); // Get item database

	FInventoryItem* ItemToAdd = ItemTable->FindRow<FInventoryItem>(ID, ""); // Find an item in the database using it's ID

	if (!ItemToAdd) { return; } // NULL check

	if (ItemToAdd)
	{
		Inventory.Add(*ItemToAdd); // Add to player's inventory
	}
}
