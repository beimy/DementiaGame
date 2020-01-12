// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

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
#include "CultyGameCharacter.generated.h"


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


UCLASS(config=Game)
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

public:
	ACultyGameCharacter();

	// Called when the game starts, or when the player is spawned
	virtual void BeginPlay() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	// AttackInput - triggers attack animations based on user input
	void AttackInput();

	// AttackStart - triggered when the player initiates an attack
	void AttackStart();

	// AttackEnd - triggered when the player stops their attack
	void AttackEnd();

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
