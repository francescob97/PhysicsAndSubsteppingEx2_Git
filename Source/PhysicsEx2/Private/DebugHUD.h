// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DebugHUD.generated.h"

#define CM_TO_METER 0.01

/**
 * 
 */
UCLASS()
class ADebugHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> OscillatorSystemActor;

protected:	
	// Construct the debugging HUD
	ADebugHUD();

	virtual void BeginPlay() override;

	// Add a FText to the HUD for rendering
	void AddText(const TCHAR* title, const FText& value);

	// Add a float to the HUD for rendering
	void AddFloat(const TCHAR* title, float value);

	// Add a bool to the HUD for rendering
	void AddBool(const TCHAR* title, bool value);

	//Draw the HUD
	virtual void DrawHUD() override;
	
	//The horizontal offset to render the statistic values at
	float HorizontalOffset = 200.f;

private:
	// Convert a TCHAR pointer to FText
	FText CStringToText(const TCHAR* text);
	
	// Convert a bool to FText
	FText BoolToText(bool value);

	// Add a segment to the Graph HUD
	void DrawGraphData(const TArray<float>& Values, const FVector2D YValuesInterval, const FLinearColor Color);

	// Render a statistic onto the HUD canvas
	void RenderStatistic(const TCHAR* title, const FText& value, const FLinearColor& valueColor = FLinearColor::White);

	// Font used to render the debug information
	UPROPERTY(Transient)
	UFont* MainFont = nullptr;

	// The current X coordinate
	float X = 20.f;

	// The current Y coordinate
	float Y = 20.f;

	// The line height to separate each entry
	float LineHeight = 16.f;

	// Size of the graph
	float GraphSize = 200.f;
	
	// Top left coordinates of the graph
	FVector2D TopLeftCorner;

	// Bottom left coordinates of the graph
	FVector2D BottomLeftCorner;	
	
	// Number of values tracked for the Graph
	int32 MaxTrackedValues;
};
