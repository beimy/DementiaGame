// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/World.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CollisionQueryParams.h"
#include "Materials/Material.h"
#include "MeshDrawShaderBindings.h"
#include "ProceduralMeshComponent.h"
#include "Light_Mook.generated.h"

UCLASS()
class CULTYGAME_API ALight_Mook : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALight_Mook();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Line of Sight", meta = (AllowPrivateAccess = "true"))
		class UProceduralMeshComponent* LOSMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Line of Sight", meta = (AllowPrivateAccess = "true"))
		UMaterial* LOSMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Line of Sight", meta = (AllowPrivateAccess = "true", UIMin = "1.0", UIMax = "360.0"))
		float ArcAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Line of Sight", meta = (AllowPrivateAccess = "true", UIMin = "1.0", UIMax = "5.0"))
		float AngleStep;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Line of Sight", meta = (AllowPrivateAccess = "true", UIMin = "200", UIMax = "1000"))
		float Radius;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Line of Sight", meta = (AllowPrivateAccess = "true"))
		TArray<FVector> LOSVertices;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Line of Sight", meta = (AllowPrivateAccess = "true"))
		TArray<int32> LOSTriangles;

private:

	void InitLOSMesh();

	void TickLOSMesh(float DeltaTime);

	void UpdateLOSMeshData(const TArray<FVector>& Vertices, const TArray<int32>& Triangles);

};
