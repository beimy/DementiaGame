// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CultyGameCharacter.h"
#include "Interactable.h" // Inventory
#include "GameplayController.h" // Inventory
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Engine.h"

//////////////////////////////////////////////////////////////////////////
// ACultyGameCharacter

ACultyGameCharacter::ACultyGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.0, 100.f, 0.0);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	
	// Load our animation montage on runtime
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeSwordAttackMontageObject(TEXT("AnimMontage'/Game/Mannequin/Montages/MeleeSwordAttackMontage2.MeleeSwordAttackMontage2'"));
	if (MeleeSwordAttackMontageObject.Succeeded()) // Null check
	{
		MeleeSwordAttackMontage = MeleeSwordAttackMontageObject.Object; // Retrieve the montage from the container object, i.e. get me the object of this thing I just loaded.
		UE_LOG(LogTemp, VeryVerbose, TEXT("Animation Montage has been loaded successfully."));
	}

	SwordBaseCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordBaseCollisionBox"));
	SwordBaseCollisionBox->SetupAttachment(RootComponent);
	SwordBaseCollisionBox->SetCollisionProfileName("NoCollision"); // When collision boxes are initially available to the player character, we don't want anything to start colliding with them. We only want them to be colliding during the actual attack anim.
	SwordBaseCollisionBox->SetNotifyRigidBodyCollision(false); // Hit Generation turned off.
	SwordBaseCollisionBox->SetHiddenInGame(false);

	SwordMidCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordMidCollisionBox"));
	SwordMidCollisionBox->SetupAttachment(RootComponent);
	SwordMidCollisionBox->SetCollisionProfileName("NoCollision"); /// When collision boxes are initially available to the player character, we don't want anything to start colliding with them. We only want them to be colliding during the actual attack anim.
	SwordMidCollisionBox->SetNotifyRigidBodyCollision(false); // Hit Generation turned off.
	SwordMidCollisionBox->SetHiddenInGame(false);

	SwordTipCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordTipCollisionBox"));
	SwordTipCollisionBox->SetupAttachment(RootComponent);
	SwordTipCollisionBox->SetCollisionProfileName("NoCollision"); // When collision boxes are initially available to the player character, we don't want anything to start colliding with them. We only want them to be colliding during the actual attack anim.
	SwordTipCollisionBox->SetNotifyRigidBodyCollision(false); // Hit Generation turned off.
	SwordTipCollisionBox->SetHiddenInGame(false);
}

void ACultyGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	// After all of the work from the constructor is finished (where we setup collision boxes and their attachments)
	// Take the box collisions and snap them onto our player character's weapon sockets
	// Attach collision components to sockets based on transformations definitions
	// Param1: Snap to target location, Param2: Snap to target rotation, Param3, Keep scaling of our object whatever we're attaching to that socket
	// within the world's constraints Param4: False, do nothing with simulated bodies.
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	SwordBaseCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "SwordBase");
	SwordMidCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "SwordMid");
	SwordTipCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "SwordTip");

	// OnComponentHit event provided by ...CollisionBox, pass in myself ('this'), and make sure MeleeAttackHit is triggered on myself ('this') object.
	// SwordBaseCollisionBox->OnComponentHit.AddDynamic(this, &UMeleeAttackSystem::MeleeAttackOnHit);
	// SwordMidCollisionBox->OnComponentHit.AddDynamic(this, &UMeleeAttackSystem::MeleeAttackOnHit);
	// SwordTipCollisionBox->OnComponentHit.AddDynamic(this, &UMeleeAttackSystem::MeleeAttackOnHit);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACultyGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACultyGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACultyGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACultyGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACultyGameCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACultyGameCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACultyGameCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACultyGameCharacter::OnResetVR);

	PlayerInputComponent->BindAction("MeleeAttack", IE_Pressed, this, &ACultyGameCharacter::AttackInput); 
	PlayerInputComponent->BindAction("MeleeAttack", IE_Released, this, &ACultyGameCharacter::AttackEnd);
}

void ACultyGameCharacter::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	CheckForInteractables();

}

void ACultyGameCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACultyGameCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ACultyGameCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ACultyGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACultyGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACultyGameCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACultyGameCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

// Inventory
void ACultyGameCharacter::CheckForInteractables() // Check for interactable times through a ray cast every tick
{

	FHitResult HitResult;

	FVector StartTrace = FollowCamera->GetComponentLocation();
	FVector EndTrace = (FollowCamera->GetForwardVector() * 600) + StartTrace;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Ignore ourselves.

	AGameplayController* Controller = Cast<AGameplayController>(GetController()); // Get our player's controller.

	if (!Controller) { return; }

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, QueryParams) && Controller) // NULL check line trace and player controller
	{
		// Check if the item we hit was an interactable item
		if (AInteractable * Interactable = Cast<AInteractable>(HitResult.GetActor())) // Check if actor hit is an interactable, if so...
		{
			Controller->CurrentInteractable = Interactable; // ...set current interactable to interactable.
			return;
		}
	}

	// If we didn't hit anything, or the actor we hit was not an interactable, set the current interactable to nullptr
	Controller->CurrentInteractable = nullptr;

}

void ACultyGameCharacter::AttackInput()
{
	Log(ELogLevel::INFO, __FUNCTION__);

	// generate a random number between 1 and 3
	int MontageSectionIndex = rand() % 3 + 1;

	// FString animation section, start_ is hard coded, and we just pass in the number generated above, thus "start_x", can be either "start_1" or "start_2"
	FString MontageSection = "start_" + FString::FromInt(MontageSectionIndex);

	PlayAnimMontage(MeleeSwordAttackMontage, 1.f, FName(*MontageSection));
}

void ACultyGameCharacter::AttackStart()
{	
	Log(ELogLevel::INFO, __FUNCTION__);

	// Enable colliders when animation starts.
	SwordBaseCollisionBox->SetCollisionProfileName("Weapon");
	SwordBaseCollisionBox->SetNotifyRigidBodyCollision(true); // Equivocal to Simulation Generates Hit Events boolean found in BPs, Turn on Hit Generation.

	SwordMidCollisionBox->SetCollisionProfileName("Weapon");
	SwordMidCollisionBox->SetNotifyRigidBodyCollision(true); // Equivocal to Simulation Generates Hit Events boolean found in BPs, Turn on Hit Generation.

	SwordTipCollisionBox->SetCollisionProfileName("Weapon");
	SwordTipCollisionBox->SetNotifyRigidBodyCollision(true); // Equivocal to Simulation Generates Hit Events boolean found in BPs, Turn on Hit Generation.
}

void ACultyGameCharacter::AttackEnd()
{
	Log(ELogLevel::INFO, __FUNCTION__);

	// Disable colliders when animation ends.
	SwordBaseCollisionBox->SetCollisionProfileName("NoCollision");
	SwordBaseCollisionBox->SetNotifyRigidBodyCollision(false); // Equivocal to Simulation Generates Hit Events boolean found in BPs, Turn off Hit Generation.

	SwordMidCollisionBox->SetCollisionProfileName("NoCollision");
	SwordMidCollisionBox->SetNotifyRigidBodyCollision(false); // Equivocal to Simulation Generates Hit Events boolean found in BPs, Turn off Hit Generation.

	SwordTipCollisionBox->SetCollisionProfileName("NoCollision");
	SwordTipCollisionBox->SetNotifyRigidBodyCollision(false); // Equivocal to Simulation Generates Hit Events boolean found in BPs, Turn off Hit Generation.
}

/// The Punch - Part 3
/*
void ACultyGameCharacter::AttackOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Log(ELogLevel::INFO, __FUNCTION__);

	//Log(ELogLevel::INFO, Hit.GetActor()->GetName());

	//UE_LOG(LogTemp, VeryVerbose, TEXT("testing hit"));
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, __FUNCTION__);
}
*/
/// The Punch - Part 3

void ACultyGameCharacter::Log(ELogLevel LogLevel, FString Message)
{
	Log(LogLevel, Message, ELogOutput::ALL);
}

void ACultyGameCharacter::Log(ELogLevel LogLevel, FString Message, ELogOutput LogOutput)
{
	// only print when screen is selected and the GEngine object is available
	if ((LogOutput == ELogOutput::ALL || LogOutput == ELogOutput::SCREEN) && GEngine)
	{
		// default color
		FColor LogColor = FColor::Cyan;
		// flip the color based on the type
		switch (LogLevel)
		{
		case ELogLevel::TRACE:
			LogColor = FColor::Green;
			break;
		case ELogLevel::DEBUG:
			LogColor = FColor::Cyan;
			break;
		case ELogLevel::INFO:
			LogColor = FColor::White;
			break;
		case ELogLevel::WARNING:
			LogColor = FColor::Yellow;
			break;
		case ELogLevel::ERROR:
			LogColor = FColor::Red;
			break;
		default:
			break;
		}
		// print the message and leave it on screen ( 4.5f controls the duration )
		GEngine->AddOnScreenDebugMessage(-1, 4.5f, LogColor, Message);
	}

	if (LogOutput == ELogOutput::ALL || LogOutput == ELogOutput::OUTPUT_LOG)
	{
		// flip the message type based on error level
		switch (LogLevel)
		{
		case ELogLevel::TRACE:
			UE_LOG(LogTemp, VeryVerbose, TEXT("%s"), *Message);
			break;
		case ELogLevel::DEBUG:
			UE_LOG(LogTemp, Verbose, TEXT("%s"), *Message);
			break;
		case ELogLevel::INFO:
			UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
			break;
		case ELogLevel::WARNING:
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
			break;
		case ELogLevel::ERROR:
			UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
			break;
		default:
			UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
			break;
		}
	}
}
