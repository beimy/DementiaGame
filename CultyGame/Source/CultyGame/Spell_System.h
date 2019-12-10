// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Spell_System.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CULTYGAME_API USpell_System : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpell_System();

	//Distance that the line trace travels
	float dist = 500.0f;
	
	//Casts the spell that is equiped
	void CastEquipedSpell();
	
	//Gets the equiped spell
	void FindEquipedSpell();

	FVector GetLineTraceStart();

	FVector GetLineTraceEnd();

	float MadnessCost = 50.0f;

	//Ensures player has enough madness to cast the spell
	void MadnessCheck();







protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
