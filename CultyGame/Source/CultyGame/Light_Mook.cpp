// Fill out your copyright notice in the Description page of Project Settings.


#include "Light_Mook.h"

// Sets default values
ALight_Mook::ALight_Mook()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Create the line of sight mesh
	LOSMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("LineOfSightMesh"));
	LOSMesh->bUseAsyncCooking = true;
	LOSMesh->ContainsPhysicsTriMeshData(false);
	LOSMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));

		// Assign some default numeric values to avoid divide-by-zero problem
		if (ArcAngle == 0)
			ArcAngle = 120;

		if (AngleStep == 0)
			AngleStep = 1;

		if (Radius == 0)
			Radius = 500;

		// Activate ticking in order to update the cursor every frame.
		PrimaryActorTick.bCanEverTick = true;
		PrimaryActorTick.bStartWithTickEnabled = true;
	

}

// Called when the game starts or when spawned
void ALight_Mook::BeginPlay()
{
	Super::BeginPlay();

	InitLOSMesh();
	
}

// Called every frame
void ALight_Mook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickLOSMesh(DeltaTime);
}

// Called to bind functionality to input
void ALight_Mook::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ALight_Mook::InitLOSMesh()
{
	int NumVertices = FMath::RoundToInt(ArcAngle / AngleStep) + 2;
	LOSVertices.Init(FVector::ZeroVector, NumVertices);

	int NumTriangles = (ArcAngle == 360) ? ((NumVertices - 1) * 3) : ((NumVertices - 2) * 3);
	LOSTriangles.Init(0, NumTriangles);

	FVector LineStartLocation = GetActorLocation();
	FVector CurrentActorForward = GetActorForwardVector();
	float MinAngle = -ArcAngle / 2;
	float MaxAngle = ArcAngle / 2;
	int VertexIndex = 1;
	for (float CurrentAngle = MinAngle;
		CurrentAngle <= MaxAngle;
		CurrentAngle += AngleStep)
	{
		FVector CurrentAngleDirection = CurrentActorForward.RotateAngleAxis(CurrentAngle, FVector(0, 0, 1));
		FVector LineEndLocation = LineStartLocation + CurrentAngleDirection * Radius;

		FVector HitResultInCharacterLocalSpace = GetActorTransform().InverseTransformPosition(LineEndLocation);
		LOSVertices[VertexIndex] = HitResultInCharacterLocalSpace;
		VertexIndex++;
	}

	VertexIndex = 0;
	for (int Triangle = 0; Triangle < LOSTriangles.Num(); Triangle += 3)
	{
		LOSTriangles[Triangle] = 0;
		LOSTriangles[Triangle + 1] = VertexIndex + 1;

		if (Triangle == (NumVertices - 2) * 3) // the arc has 360 angle, or in other words, it's a circle
		{
			LOSTriangles[Triangle + 2] = 1;
		}
		else
		{
			LOSTriangles[Triangle + 1] = VertexIndex + 2;
		}

		VertexIndex++;
	}

	UpdateLOSMeshData(LOSVertices, LOSTriangles);

	LOSMesh->SetRelativeLocation(FVector(0, 0, -90));
	LOSMesh->SetMaterial(0, LOSMaterial);
}
void ALight_Mook::TickLOSMesh(float DeltaSeconds)
{
	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	const FName TraceTag("LoSTraceTag");
	FCollisionQueryParams TraceParams = FCollisionQueryParams(TraceTag, false, this);

	FVector LineStartLocation = GetActorLocation();
	FVector CurrentActorForward = GetActorForwardVector();

	float MinAngle = -ArcAngle / 2;
	float MaxAngle = ArcAngle / 2;
	int VertexIndex = 1;
	for (float CurrentAngle = MaxAngle;
		CurrentAngle >= MinAngle;
		CurrentAngle -= AngleStep)
	{
		FVector CurrentAngleDirection = CurrentActorForward.RotateAngleAxis(CurrentAngle, FVector(0, 0, 1));
		FVector LineEndLocation = LineStartLocation + CurrentAngleDirection * Radius;

		FHitResult HitResult;
		FVector HitPoint;

		// In DefaultEngine.ini: Channel=ECC_GameTraceChannel1,Name="Obstacle"
		bool bHit = World->LineTraceSingleByChannel(HitResult, LineStartLocation, LineEndLocation, ECollisionChannel::ECC_GameTraceChannel1, TraceParams, FCollisionResponseParams());
		if (bHit)
		{
			HitPoint = HitResult.ImpactPoint;
		}
		else
		{
			HitPoint = LineEndLocation;
		}

		FVector HitResultInCharacterLocalSpace = GetActorTransform().InverseTransformPosition(HitPoint);
		LOSVertices[VertexIndex] = HitResultInCharacterLocalSpace;
		VertexIndex++;
	}

	VertexIndex = 0;
	int NumVertices = LOSVertices.Num();
	for (int Triangle = 0; Triangle < LOSTriangles.Num(); Triangle += 3)
	{
		LOSTriangles[Triangle] = 0;
		LOSTriangles[Triangle + 1] = VertexIndex + 1;

		if (Triangle == ((NumVertices - 2) * 3)) // the arc has 360 angle, or in other words, it's a circle
		{
			LOSTriangles[Triangle + 2] = 1;
		}
		else
		{
			LOSTriangles[Triangle + 2] = VertexIndex + 2;
		}

		VertexIndex++;
	}

	UpdateLOSMeshData(LOSVertices, LOSTriangles);
}
void ALight_Mook::UpdateLOSMeshData(const TArray<FVector>& Vertices, const TArray<int32>& Triangles)
{
	TArray<FVector> TempNormals;
	TArray<FVector2D> TempUV0;
	TArray<FProcMeshTangent> TempTangents;
	TArray<FLinearColor> TempVertexColors;
	LOSMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TempNormals, TempUV0, TempVertexColors, TempTangents, false);
}

