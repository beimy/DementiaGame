// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Engine.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "MeleeAttackSystem.generated.h"

UENUM(BlueprintType)
enum class ELogLevel : uint8
{
	TRACE			UMETA(DisplayName = "Trace"),
	DEBUG			UMETA(DisplayName = "Debug"),
	INFO			UMETA(DisplayName = "Info"),
	WARNING			UMETA(DisplayName = "Warning"),
	ERROR			UMETA(DisplayName = "Error")
};

UENUM(BlueprintType)
enum class ELogOutput : uint8 // Where the log messages will go.
{
	ALL				UMETA(DisplayName = "All levels"),
	OUTPUT_LOG		UMETA(DisplayName = "Output log"),
	SCREEN			UMETA(DisplayName = "Screen")
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	MELEE_SWORD		UMETA(DisplayName = "Melee - Sword")
};



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CULTYGAME_API UMeleeAttackSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMeleeAttackSystem();

protected:
	// Called when the game starts, or when the player is spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AActor* PlayerCharacter;

	ACharacter* PlayerCharacterAnim;

	FString ActorName;

	// How far ahead from the melee weapon can our Line Trace(s) reach.
	float Reach = 30.0f;

	// Used to point to our Input component, set to nullptr in case UInputComponent.h/.cpp loads up before.
	USceneComponent* SceneComponent = nullptr;

	// USkeletalMeshComponent* OwnerMeshComponent = nullptr;

	// Play animation & deal damage 
	void MeleeAttack();

	/// The Punch - Part 1
	// Melee Attack Montage Animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true")) // Allows us to bind our properties to a Blueprint
		UAnimMontage* MeleeSwordAttackMontage;

	/// The Punch - Part 2
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true")) // Allows us to bind our properties to a Blueprint
		UBoxComponent* SwordBaseCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true")) // Allows us to bind our properties to a Blueprint
		UBoxComponent* SwordMidCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true")) // Allows us to bind our properties to a Blueprint
		UBoxComponent* SwordTipCollisionBox;
	void MeleeAttackInput(); // Triggers attack animations based on user input
	/// The Punch - Part 2
	void MeleeAttackStart(); // Initiates player attack
	void MeleeAttackEnd(); // Stops player attack
	/// The Punch - Part 1


	// Triggered when the collision hit event fires between our weapon and enemy entities
	UFUNCTION()
		void MeleeAttackHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Used to point to our Input component, set to nullptr in case UInputComponent.h/.cpp loads up before.
	UInputComponent* InputComponent = nullptr;

	// Set up input actions
	void SetupInputComponent();

private:
	/*
	Log - Prints a message to all the log ouputs with a specific color
	@param LogLevel (@see ELogLevel) affects color of log
	@param FString the message for display
	*/
	void Log(ELogLevel LogLevel, FString Message);
	/*
	Log - Prints a message to all the log ouputs with a specific color
	@param LogLevel (@see ELogLevel) affects color of log
	@param FString the message for display
	@param ELogOutput - All, Output Log or Screen
	*/
	void Log(ELogLevel LogLevel, FString Message, ELogOutput LogOutput);
};
