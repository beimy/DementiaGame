// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	DefaultHealth = 100;
	Health = DefaultHealth;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner != nullptr)
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);
	}
	
}


void UHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{	
	if (Damage <= 0)
	{
		// Play Death anim, destroy actor
		return;
	}

	else
	{
		//Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
		Health -= Damage;
		//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, FString::Printf(TEXT("Damage Applied")));
	}
}
