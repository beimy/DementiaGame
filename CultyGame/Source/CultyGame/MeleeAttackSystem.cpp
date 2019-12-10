// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeAttackSystem.h"
#include "CultyGameCharacter.h"

#define OUT

// Sets default values for this component's properties
UMeleeAttackSystem::UMeleeAttackSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	/// The Punch - Part 1
	// Load our animation montage on runtime
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeSwordAttackMontageObject(TEXT("AnimMontage'/Game/Mannequin/Montages/MeleeSwordAttackMontage.MeleeSwordAttackMontage'"));
	if (MeleeSwordAttackMontageObject.Succeeded()) // I.E. null check
	{
		MeleeSwordAttackMontage = MeleeSwordAttackMontageObject.Object; // Retrieve the montage from the container object, i.e. get me the object of this thing I just loaded.
	}
	/// The Punch - Part 1

	/// The Punch - Part 2
	// No access to 'RootComponent' from 'SetupAttachment()', since RootComponent is found within 'CultyGameCharacter.h'. 
	// We Cast to it, and get it's root component by '->GetRootComponent'
	
	ACultyGameCharacter* CultyGameCharacter = Cast<ACultyGameCharacter>(GetOwner());

	SwordBaseCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordBaseCollisionBox"));
	if (CultyGameCharacter != NULL)
	{
		SwordBaseCollisionBox->SetupAttachment(CultyGameCharacter->GetRootComponent());
		SwordBaseCollisionBox->SetCollisionProfileName("NoCollision"); // When collision boxes are initially available to the player character, we don't want anything to start colliding with them. We only want them to be colliding during the actual attack anim.
		SwordBaseCollisionBox->SetHiddenInGame(false);
	}

	SwordMidCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordMidCollisionBox"));
	if (CultyGameCharacter != NULL)
	{
		SwordMidCollisionBox->SetupAttachment(CultyGameCharacter->GetRootComponent());
		SwordMidCollisionBox->SetCollisionProfileName("NoCollision"); /// When collision boxes are initially available to the player character, we don't want anything to start colliding with them. We only want them to be colliding during the actual attack anim.
		SwordMidCollisionBox->SetHiddenInGame(false);
	}

	SwordTipCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordTipCollisionBox"));
	if (CultyGameCharacter != NULL)
	{
		SwordTipCollisionBox->SetupAttachment(CultyGameCharacter->GetRootComponent());
		SwordTipCollisionBox->SetCollisionProfileName("NoCollision"); // When collision boxes are initially available to the player character, we don't want anything to start colliding with them. We only want them to be colliding during the actual attack anim.
		SwordTipCollisionBox->SetHiddenInGame(false);
	}
	/// The Punch - Part 2
}

void UMeleeAttackSystem::SetupInputComponent()
{
	InputComponent = GetWorld()->GetFirstPlayerController()->GetPawn()->FindComponentByClass<UInputComponent>();

	if (InputComponent != nullptr)
	{
		//InputComponent->BindAction("MeleeAttack", IE_Pressed, this, &UMeleeAttackSystem::MeleeAttack);
		/// The Punch - Part 1
		InputComponent->BindAction("MeleeAttack", IE_Pressed, this, &UMeleeAttackSystem::MeleeAttackInput); /// The Punch - Part 2, changed from MeleeAttackStart to MeleeAttackInput.
		InputComponent->BindAction("MeleeAttack", IE_Released, this, &UMeleeAttackSystem::MeleeAttackEnd);
		/// The Punch - Part 1
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *ActorName);
	}
}

FVector UMeleeAttackSystem::GetLineTraceStart()
{
	SceneComponent = GetWorld()->GetFirstPlayerController()->GetPawn()->FindComponentByClass<USceneComponent>();

	FVector PlayerWeaponLocation = SceneComponent->GetSocketLocation(FName("SwordBase"));
	FRotator PlayerWeaponRotation = SceneComponent->GetSocketRotation(FName("SwordTip"));

	return PlayerWeaponLocation;
}

FVector UMeleeAttackSystem::GetLineTraceEnd()
{
	SceneComponent = GetWorld()->GetFirstPlayerController()->GetPawn()->FindComponentByClass<USceneComponent>();

	FVector PlayerWeaponLocation = SceneComponent->GetSocketLocation(FName("SwordBase"));
	FVector PlayerWeaponRotation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorForwardVector(); 
 // FVector PlayerWeaponRotation = SceneComponent->GetSocketLocation(FName("SwordTip"));

	return PlayerWeaponLocation + PlayerWeaponRotation * Reach;
}

// Line Trace itself
const TArray<FHitResult> UMeleeAttackSystem::GetFirstEnemyHit()
{	
	/// Line Trace (AKA Ray-cast) out to Reach distance
	TArray<FHitResult> HitResults; // Store all actors hit in an array.
	FHitResult HitActor(ForceInit);

	/// Setup query parameters for Line Trace
	FCollisionObjectQueryParams TraceObjectInterest(ECollisionChannel::ECC_GameTraceChannel1); // Collision types we're interested in.
	FCollisionQueryParams TraceParameters = FCollisionQueryParams(FName(TEXT("")), false, GetOwner());

	bool IsHit = GetWorld()->LineTraceMultiByObjectType(
		OUT HitResults,
		GetLineTraceStart(),
		GetLineTraceEnd(),
		TraceObjectInterest,
		TraceParameters
	);

	if (IsHit != false)
	{
		/// Iterate through HitResults, check if we Hit an enemy
		for (auto& Hit : HitResults)
		{
			if (GEngine != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit Result: %s"), *Hit.Actor->GetName()));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("All Hit Information: %s"), *Hit.ToString()));
			}
			HitActor = Hit;
		}

		/*
		UPrimitiveComponent* EnemyToPush = Hit.GetComponent();

		if (HitData.GetActor() != nullptr) // ((HitData.GetActor() != nullptr) && (HitData.GetActor() != GetOwner()) && (EnemyToPush != nullptr))
		{
			const float ForceAmount = 20000.0f;
			EnemyToPush->AddForce(FVector(0.0f, 0.0f, ForceAmount));
		}
		*/
	}
	return HitResults;
}

void UMeleeAttackSystem::MeleeAttack()
{
	GetFirstEnemyHit();

	// DrawDebugLine(GetWorld(), GetLineTraceStart(), GetLineTraceEnd(), FColor(255, 0, 0), false, 2.0f, 0.0f, 1.5f);

	/// Get Socket locations
	FVector curBase = SceneComponent->GetSocketLocation(FName("SwordBase"));
	FVector curTip = SceneComponent->GetSocketLocation(FName("SwordTip"));

	/// Store previous Socket locations
	FVector prevBase = curBase;
	FVector prevTip = curTip;

	/// Store current & previous length between Sockets for Line Trace
	float curLength = (curBase - curTip).Size();
	float prevLength = (prevBase - prevTip).Size();

	const int sub = 32;
	for (int i = 1; i < sub; i++)
	{
		FVector tmpBase = FMath::Lerp(curBase, prevBase, i / float(sub));
		FVector tmpTip = FMath::Lerp(curTip, prevTip, i / float(sub));
		FVector tmpOff = (tmpTip - tmpBase);
		tmpOff.Normalize();
		DrawDebugLine(GetWorld(), tmpBase, tmpBase + tmpOff * FMath::Lerp(curLength, prevLength, i / float(sub)), FColor::Red, false, 2.0f, 0.0f, 1.5f);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Test")));
	}
	DrawDebugLine(GetWorld(), curBase, curTip, FColor::Green, false, 2.0f, 0.0f, 1.5f);
	// DrawDebugLine(GetWorld(), curBase, curTip, FColor::Green, false, 1 / 15.0f * 2);

	return;
}

/// The Punch - Part 2
void UMeleeAttackSystem::MeleeAttackInput()
{
	Log(ELogLevel::INFO, __FUNCTION__);

	// generate a random number between 1 and 3
	int MontageSectionIndex = rand() % 3 + 1;

	// FString animation section, start_ is hard coded, and we just pass in the number generated above, thus "start_x", can be either "start_1" or "start_2"
	FString MontageSection = "start_" + FString::FromInt(MontageSectionIndex);

	PlayerCharacterAnim = GetWorld()->GetFirstPlayerController()->GetCharacter();
	PlayerCharacterAnim->PlayAnimMontage(MeleeSwordAttackMontage, 1.f, FName(*MontageSection));
}
/// The Punch - Part 2

/// The Punch - Part 1
void UMeleeAttackSystem::MeleeAttackStart()
{
	/// The Punch - Part 2
	Log(ELogLevel::INFO, __FUNCTION__);

	// Enable colliders when animation starts.
	SwordBaseCollisionBox->SetCollisionProfileName("Weapon");
	SwordMidCollisionBox->SetCollisionProfileName("Weapon");
	SwordTipCollisionBox->SetCollisionProfileName("Weapon");
	/// The Punch - Part 2
}
/// The Punch - Part 1

/// The Punch - Part 1
void UMeleeAttackSystem::MeleeAttackEnd()
{
	Log(ELogLevel::INFO, __FUNCTION__);

	/// The Punch - Part 2
	// Disable colliders when animation ends.
	SwordBaseCollisionBox->SetCollisionProfileName("NoCollision");
	SwordMidCollisionBox->SetCollisionProfileName("NoCollision");
	SwordTipCollisionBox->SetCollisionProfileName("NoCollision");
	/// The Punch - Part 2
}
/// The Punch - Part 1

// Called when the game starts
void UMeleeAttackSystem::BeginPlay()
{
	Super::BeginPlay();

	SetupInputComponent();
	// No access to 'RootComponent' from 'SetupAttachment()', since RootComponent is found within 'CultyGameCharacter.h'. 
	// We Cast to it, and get it's root component by '->GetRootComponent'
	ACultyGameCharacter* CultyGameCharacter = Cast<ACultyGameCharacter>(GetOwner());

	// After all of the work from the constructor is finished (where we setup collision boxes and their attachments)
	// Take the box collisions and snap them onto our player character's weapon sockets
	// Attach collision components to sockets based on transformations definitions
	// Param1: Snap to target location, Param2: Snap to target rotation, Param3, Keep scaling of our object whatever we're attaching to that socket
	// within the world's constraints Param4: False, do nothing with simulated bodies.
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
		
	SwordBaseCollisionBox->AttachToComponent(CultyGameCharacter->GetMesh(), AttachmentRules, "SwordBase");
	SwordMidCollisionBox->AttachToComponent(CultyGameCharacter->GetMesh(), AttachmentRules, "SwordMid");
	SwordTipCollisionBox->AttachToComponent(CultyGameCharacter->GetMesh(), AttachmentRules, "SwordTip");
}

// Called every frame
void UMeleeAttackSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If actor melee's
		// MeleeAttack();
			// LastTimeMeleePressed = GetWorld()->GetTimeSeconds();
	// If (GetWorld()->GetTimeSeconds() - LastTimeMeleePressed >= MeleeDelay)
	
}

void UMeleeAttackSystem::Log(ELogLevel LogLevel, FString Message)
{
	Log(LogLevel, Message, ELogOutput::ALL);
}

void UMeleeAttackSystem::Log(ELogLevel LogLevel, FString Message, ELogOutput LogOutput)
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
