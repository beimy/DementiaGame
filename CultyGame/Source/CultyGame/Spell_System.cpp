// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell_System.h"

// Sets default values for this component's properties
USpell_System::USpell_System()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USpell_System::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USpell_System::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

