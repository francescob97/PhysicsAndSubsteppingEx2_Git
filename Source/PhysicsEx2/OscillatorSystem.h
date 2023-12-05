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

protected:	
	virtual void BeginPlay() override;

	// Initialize the equilibrium position
	void InitEquilibriumPosition();

	// Draw equilibrium debug line
	void DrawEquilibriumPosition() const;

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

	// Current displacement of the spring [cm]. Negative if compression, positive if extension //TODO
	UPROPERTY(VisibleAnywhere, Category="Spring Properties")
	float SpringCurrentDisplacement;

private:
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
};
