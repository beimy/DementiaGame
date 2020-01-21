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

void AGameplayController::CraftItem(FInventoryItem ItemA, FInventoryItem ItemB, AGameplayController* Controller)
{

	// Check if we've made a valid craft, or if the items were not correct, and nothing was made.
	for (auto Craft : ItemB.CraftCombinations)
	{
		// If the craft's component ID was the one that we used, then we've created an item.
		if (Craft.ComponentID == ItemA.ItemID)
		{

			if (Craft.bDestroyComponentA)
			{
				Inventory.RemoveSingle(ItemA);
			}

			if (Craft.bDestroyComponentB)
			{
				Inventory.RemoveSingle(ItemB);
			}

			AddItemToInventoryByID(Craft.ProductID);

			ReloadInventory();
		}
	}
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
