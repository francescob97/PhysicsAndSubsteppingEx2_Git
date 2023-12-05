#include "OscillatorSystem.h"

/*
 * Construct the Oscillator System
 **********************************************************/
AOscillatorSystem::AOscillatorSystem()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("Cube");
	CubeMesh->SetupAttachment(Root);
	CubeMesh->SetSimulatePhysics(true);
	CubeMesh->SetEnableGravity(false);
	CubeMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	SpringMesh = CreateDefaultSubobject<UStaticMeshComponent>("Spring");
	SpringMesh->SetupAttachment(Root);
}

void AOscillatorSystem::BeginPlay()
{
	Super::BeginPlay();

	InitEquilibriumPosition();
	DrawEquilibriumPosition();
}

void AOscillatorSystem::InitEquilibriumPosition()
{
	EquilibriumPosition = CubeMesh->GetComponentLocation() + SpringInitialDisplacement * FVector::UpVector;
}

void AOscillatorSystem::DrawEquilibriumPosition() const
{
	const FVector LineStart = EquilibriumPosition - LINE_EQUILIBRIUM_LENGTH * FVector::RightVector;
	const FVector LineEnd = EquilibriumPosition + LINE_EQUILIBRIUM_LENGTH * FVector::RightVector;
	
	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Black, true, -1, 0 , LINE_EQUILIBRIUM_THICKNESS);
}

void AOscillatorSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CubeMesh->SetMassOverrideInKg(NAME_None, CubeMass, true);

	const float ScaleZ = FMath::GetMappedRangeValueClamped(FVector2D(MinInitialDisplacement, MaxInitialDisplacement), FVector2D(0.5f, 2.f), SpringCurrentDisplacement);
	SpringMesh->SetWorldScale3D(FVector(1.f, 1.f, ScaleZ));
}

void AOscillatorSystem::IncreaseCubeDisplacement()
{
	if(SpringInitialDisplacement < MaxInitialDisplacement)
	{
		CubeMesh->AddLocalOffset(FVector(0.f, 0.f, -DisplacementIncreaseStep));
		SpringInitialDisplacement += DisplacementIncreaseStep;
		SpringCurrentDisplacement = SpringInitialDisplacement;
	}	
}

void AOscillatorSystem::DecreaseCubeDisplacement()
{
	if(SpringInitialDisplacement > MinInitialDisplacement)
	{
		CubeMesh->AddLocalOffset(FVector(0.f, 0.f, DisplacementIncreaseStep));
		SpringInitialDisplacement -= DisplacementIncreaseStep;
		SpringCurrentDisplacement = SpringInitialDisplacement;
	}
}