// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Engine.h"
#include "Engine/World.h"
#include "Engine/Datatable.h" // Inventory
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/AudioComponent.h"
#include "CultyGameCharacter.generated.h"


/*
	Stores all valid craft combinations, what items can we craft.

*/
USTRUCT(BlueprintType)
struct FCraftingInfo : public FTableRowBase // Inventory
{

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // Item that we need to use to create something.
		FName ComponentID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // Item created from components.
		FName ProductID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // Item used to create something, thus destroy it after creation.
		bool bDestroyComponentA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // Item used to create something, thus destroy it after creation.
		bool bDestroyComponentB;
};


/*
	Items are made of structs
	Inherits from FTableRowBase, because we want to use this in a data table.
	There are 2 types of items:
	1. Pickups: When picked up, it's destroyed, and adds one of these FInventoryItem(s) to inventory.
	2.
*/
USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase // Inventory
{

	GENERATED_BODY()

public:

	FInventoryItem()
	{
		Name = FText::FromString("Item");
		Action = FText::FromString("Use");
		Description = FText::FromString("Please enter a description for this item");
		Value = 10;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class APickup> ItemPickup; // Pickup, if you pick up an item, but then you drop it, we'll need to spawn it back into the world.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // Stores all valid craft combinations for this item.
		TArray<FCraftingInfo> CraftCombinations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanBeUsed;

	/*
		Overloading '==' operator.
		By default we can't compare 2 inventory items, since C++ doesn't know how to compare inventory items.
		e.g.
		if(ItemA == ItemB)
		So we're instructing C++ how to compare inventory items.
		We need this because when we're removing from an inventory, and Unreal requires that we overload that data, otherwise the array cannot remove.
		Whenever we wish to remove an item from the player's inventory we cannot unless we do this.
	*/
	bool operator==(const FInventoryItem& Item) const
	{
		if (ItemID == Item.ItemID)
		{
			return true;
		}

		else
		{
			return false;
		}
	}
};


UENUM(BlueprintType)
enum class ELogLevel : uint8 {
	TRACE			UMETA(DisplayName = "Trace"),
	DEBUG			UMETA(DisplayName = "Debug"),
	INFO			UMETA(DisplayName = "Info"),
	WARNING			UMETA(DisplayName = "Warning"),
	ERROR			UMETA(DisplayName = "Error")
};

UENUM(BlueprintType)
enum class ELogOutput : uint8 {
	ALL				UMETA(DisplayName = "All levels"),
	OUTPUT_LOG		UMETA(DisplayName = "Output log"),
	SCREEN			UMETA(DisplayName = "Screen")
};

UENUM(BlueprintType)
enum class EAttackType : uint8 {
	MELEE_SWORD			UMETA(DisplayName = "Melee - Sword")
};


UCLASS(config = Game)
class ACultyGameCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	// Melee Attack Montage Animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true")) // Allows us to bind our properties to a Blueprint
		UAnimMontage* MeleeSwordAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true")) // Allows us to bind our properties to a Blueprint
		UBoxComponent* SwordBaseCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true")) // Allows us to bind our properties to a Blueprint
		UBoxComponent* SwordMidCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true")) // Allows us to bind our properties to a Blueprint
		UBoxComponent* SwordTipCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		USoundCue* SwordGestureSoundCue;


public:
	ACultyGameCharacter();

	virtual void Tick(float DeltaTime) override;

	// Called when the game starts, or when the player is spawned
	virtual void BeginPlay() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	// AttackInput - triggers attack animations based on user input
	void AttackInput();

	// AttackStart - triggered when the player initiates an attack
	void AttackStart();

	// AttackEnd - triggered when the player stops their attack
	void AttackEnd();

	// bIsSwinging - check if player is currently in an attack animation
	bool bIsSwinging;

	void InflictDamage();

	TArray<AActor*> DamagedActors;
	TArray<AActor*> HitActors;

	// Triggered when the collision hit event fires between our weapon and enemy entities
	//UFUNCTION()
	//void OnAttackHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//float WorldTime;
	//float EndOfAttack;
	//float AttackDelay = 1.0f;

	/*
	// Triggered when the collider overlaps another component
	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Triggered when the collider stops overlapping with another component
	UFUNCTION()
	void OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	*/

protected:
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void CheckForInteractables(); // Inventory

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	UAudioComponent* SwordAudioComponent;

	/**
	* Log - prints a message to all the log outputs with a specific color
	* @param LogLevel {@see ELogLevel} affects color of log
	* @param FString the message for display
	*/
	void Log(ELogLevel LogLevel, FString Message);
	/**
	* Log - prints a message to all the log outputs with a specific color
	* @param LogLevel {@see ELogLevel} affects color of log
	* @param FString the message for display
	* @param ELogOutput - All, Output Log or Screen
	*/
	void Log(ELogLevel LogLevel, FString Message, ELogOutput LogOutput);
};
