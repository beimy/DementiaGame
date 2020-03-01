// Fill out your copyright notice in the Description page of Project Settings.


#include "Light_Mook.h"

// Sets default values
ALight_Mook::ALight_Mook()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALight_Mook::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALight_Mook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALight_Mook::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

