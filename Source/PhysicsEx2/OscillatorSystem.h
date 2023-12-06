#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OscillatorSystem.generated.h"
#define LINE_EQUILIBRIUM_LENGTH 300
#define LINE_EQUILIBRIUM_THICKNESS 3

UCLASS()
class PHYSICSEX2_API AOscillatorSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	AOscillatorSystem();
	virtual void Tick(float DeltaTime) override;

	// Start the simulation
	UFUNCTION(BlueprintCallable)
	void SetStartSimulation(bool bSimulation);

protected:	
	virtual void BeginPlay() override;

	// Initialize the equilibrium position
	void InitEquilibriumPosition();

	// Draw equilibrium debug line
	void DrawEquilibriumPosition() const;

	// Simulate the Simple Harmonic Motion
	void SimulateMotion();

	// Increase initial displacement of the spring
	UFUNCTION(BlueprintCallable)
	void IncreaseCubeDisplacement();

	// Decrease initial displacement of the spring
	UFUNCTION(BlueprintCallable)
	void DecreaseCubeDisplacement();

	// Root component which spring and mass are attached to
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* Root;
	
	// Mesh of the Mass
	UPROPERTY(EditDefaultsOnly, Category="Components")
	UStaticMeshComponent* CubeMesh;

	// Mesh of the spring
	UPROPERTY(EditDefaultsOnly, Category="Components")
	UStaticMeshComponent* SpringMesh;

	// Mass of the Mass [Kg]
	UPROPERTY(EditAnywhere, Category="Cube Properties")
	float CubeMass = 50;

	// Initial displacement of the spring [cm]
	UPROPERTY(EditAnywhere, Category="Spring Properties")
	float SpringInitialDisplacement;

	// Stiffness of the spring [N/cm]
	UPROPERTY(EditAnywhere, Category="Spring Properties")
	float SpringStiffness = 100.f;

	// Current displacement of the spring [cm]. Negative if compression, positive if extension
	UPROPERTY(VisibleAnywhere, Category="Spring Properties")
	float SpringCurrentDisplacement;

	// Number of values tracked for the Graph
	UPROPERTY(EditAnywhere, Category="Simulation Properties")
	int32 MaxTrackedValues = 1500;

private:
	// Velocity on the previous frame. Used to check velocity changes [cm/s]
	float LastVelocity = 0.f;
	
	// Current velocity on the Z axis [cm/s]
	float VelocityZ = 0.f;

	// Current acceleration on the Z axis [cm/s^2]
	float AccelerationZ;

	// Amplitude in the last period [cm]
	float Amplitude;

	// Last period [s]
	float Period;

	// Number of peaks. Used to count the periods
	int32 PeakCounter = -1;

	// Period index to the frames tracked by the simulation. Used to draw period in the Graph
	int32 PeriodIndex = 0;

	// Last period index to the frames tracked by the simulation. Used to draw period in the Graph
	int32 LastPeriodIndex = 0;
	
	// Index of the simulation state
	bool bStartSimulation = false;

	// Minimum initial displacement allowed for the spring
	float MinInitialDisplacement = - 60.f;

	// Maximum initial displacement allowed for the spring
	float MaxInitialDisplacement = 60.f;
	
	// Increase/Decrease step for initial displacement
	float DisplacementIncreaseStep = 5.f;	

	// Equilibrium position of the spring
	FVector EquilibriumPosition;

	// Array of MaxTrackedValues displacements of the spring. Used to draw displacement graph
	TArray<float> SpringDisplacementsOverTime;
	
	// Allow the Debug HUD access to this class
	friend class ADebugHUD;	
};
