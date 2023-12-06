// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugHUD.h"
#include "CanvasItem.h"
#include "EngineUtils.h"
#include "Engine/Canvas.h"
#include "PhysicsEx2/OscillatorSystem.h"

/*
 * Construct the debugging HUD
 **********************************************************/
ADebugHUD::ADebugHUD()
{
	static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Engine/engineFonts/Roboto"));

	MainFont = Font.Object;
}

void ADebugHUD::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<AOscillatorSystem> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		OscillatorSystemActor = *ActorItr;  // Store the reference.
		break;  // Assuming there's only one instance in the world.
	}	
}

void ADebugHUD::AddText(const TCHAR* title, const FText& value)
{
	RenderStatistic(title, value);
}

void ADebugHUD::AddFloat(const TCHAR* title, float value)
{
	RenderStatistic(title, FText::AsNumber(value));
}

void ADebugHUD::AddBool(const TCHAR* title, bool value)
{
	RenderStatistic(title, BoolToText(value), (value == false) ? FLinearColor::Red : FLinearColor::Green);
}

void ADebugHUD::DrawHUD()
{
	Super::DrawHUD();
	
	X = Y = 20.f;
	
	if(OscillatorSystemActor)
	{		
		if(const AOscillatorSystem* OscillatorSystem = Cast<AOscillatorSystem>(OscillatorSystemActor))
		{
			AddBool(L"Start Simulation: ", OscillatorSystem->bStartSimulation);
			AddFloat(L"Mass [Kg]: ", OscillatorSystem->CubeMass);
			AddFloat(L"Spring Stiffness [N/m]: ", OscillatorSystem->SpringStiffness);
			AddFloat(L"Spring Initial Displacement [m]: ", OscillatorSystem->SpringInitialDisplacement * CM_TO_METER);
			AddFloat(L"Spring Current Displacement [m]: ", OscillatorSystem->SpringCurrentDisplacement * CM_TO_METER);
			AddFloat(L"Velocity [m/s]: ", OscillatorSystem->VelocityZ * CM_TO_METER);
			AddFloat(L"Acceleration [m/s^2]: ", OscillatorSystem->AccelerationZ * CM_TO_METER);			
		}
	}	
}

FText ADebugHUD::CStringToText(const TCHAR* text)
{
	return FText::FromString(text);
}

FText ADebugHUD::BoolToText(bool value)
{
	return CStringToText(value ? TEXT("true") : TEXT("false"));
}

void ADebugHUD::RenderStatistic(const TCHAR* title, const FText& value, const FLinearColor& valueColor)
{
	FCanvasTextItem item0(FVector2D(X, Y), CStringToText(title), MainFont, FLinearColor::White);
	item0.EnableShadow(FLinearColor(0.f, 0.f, 0.f));
	Canvas->DrawItem(item0);

	FCanvasTextItem item1(FVector2D(X + HorizontalOffset, Y), value, MainFont, valueColor);
	item1.EnableShadow(FLinearColor(0.f, 0.f, 0.f));
	Canvas->DrawItem(item1);
	Y += LineHeight;	
}