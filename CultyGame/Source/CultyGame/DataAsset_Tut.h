// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_Tut.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class CULTYGAME_API UDataAsset_Tut : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<FString> TutStrings; //= {TutText1,TutText2,TutText3,TutText4,TutText5};
		

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString TutText1 = "This is text 1";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString TutText2 = "This is text 2";
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString TutText3 = "This is text 3";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString TutText4 = "This is text 5";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString TutText5 = "This is text 5";
	*/
};
