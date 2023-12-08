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
	SimulateMotion();	

	const float ScaleZ = FMath::GetMappedRangeValueClamped(FVector2D(MinInitialDisplacement, MaxInitialDisplacement), FVector2D(0.5f, 2.f), SpringCurrentDisplacement);
	SpringMesh->SetWorldScale3D(FVector(1.f, 1.f, ScaleZ));
}

void AOscillatorSystem::SimulateMotion()
{
	if(!bStartSimulation) return;

	// Using Hooke law: F = -K * DeltaZ
	const float DeltaZ = - (CubeMesh->GetComponentLocation().Z - EquilibriumPosition.Z);
	const FVector SpringForce = - SpringStiffness * DeltaZ * FVector::DownVector;
	const FVector DampingForce = -DampingCoefficient * CubeMesh->GetComponentVelocity().Z * FVector::UpVector;
	const FVector TotalForce = SpringForce + DampingForce;

	CubeMesh->AddForce(SpringForce + DampingForce);

	SpringCurrentDisplacement = DeltaZ;
	AccelerationZ = (TotalForce / CubeMass).Z;

	LastVelocity = VelocityZ;
	VelocityZ = CubeMesh->GetComponentVelocity().Z;
	

	const float ZeroTolerance = 0.001f;
	if ((FMath::Abs(VelocityZ) > ZeroTolerance && FMath::Abs(LastVelocity) > ZeroTolerance) && FMath::Sign(LastVelocity) != FMath::Sign(VelocityZ))
	{
		if(PeakCounter % 2 == 0)
		{
			Amplitude = DeltaZ; 
			LastPeriodIndex = PeriodIndex;
			PeriodIndex = SpringDisplacementsOverTime.Num();
			Period = (PeriodIndex - LastPeriodIndex) * GetWorld()->GetDeltaSeconds();	
		}
		++PeakCounter;				
	}
	
	DrawDebugDirectionalArrow(GetWorld(),
		CubeMesh->GetComponentLocation() + LINE_EQUILIBRIUM_LENGTH * FVector::RightVector,
		EquilibriumPosition + LINE_EQUILIBRIUM_LENGTH * FVector::RightVector,
		25.f,
		FColor::Red);

	SpringDisplacementsOverTime.Add(SpringCurrentDisplacement);
	
	while (SpringDisplacementsOverTime.Num() > MaxTrackedValues)
	{
		SpringDisplacementsOverTime.RemoveAt(0);
		LastPeriodIndex = FMath::Clamp(LastPeriodIndex - 1, 0 , MaxTrackedValues);
		PeriodIndex = FMath::Clamp(PeriodIndex - 1, 0 , MaxTrackedValues);
	}	
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

void AOscillatorSystem::SetStartSimulation(bool bSimulation)
{
	bStartSimulation = bSimulation;
}