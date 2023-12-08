// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugHUD.h"
#include "CanvasItem.h"
#include "EngineUtils.h"
#include "Engine/Canvas.h"
#include "PhysicsEx2/OscillatorSystem.h"

#define INVERSE_GRAPH -1.f

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
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	TopLeftCorner = FVector2D(ViewportSize.X - X - GraphSize, Y); // A
	BottomLeftCorner= FVector2D(TopLeftCorner.X, TopLeftCorner.Y + GraphSize); // B	
	
	if(OscillatorSystemActor)
	{		
		if(const AOscillatorSystem* OscillatorSystem = Cast<AOscillatorSystem>(OscillatorSystemActor))
		{
			AddBool(L"Start Simulation: ", OscillatorSystem->bStartSimulation);
			AddFloat(L"Mass [Kg]: ", OscillatorSystem->CubeMass);
			AddFloat(L"Damping coefficient []: ", OscillatorSystem->DampingCoefficient);
			AddFloat(L"Spring Stiffness [N/m]: ", OscillatorSystem->SpringStiffness);
			AddFloat(L"Spring Initial Displacement [m]: ", OscillatorSystem->SpringInitialDisplacement * CM_TO_METER);
			AddFloat(L"Spring Current Displacement [m]: ", OscillatorSystem->SpringCurrentDisplacement * CM_TO_METER);
			AddFloat(L"Velocity [m/s]: ", OscillatorSystem->VelocityZ * CM_TO_METER);
			AddFloat(L"Acceleration [m/s^2]: ", OscillatorSystem->AccelerationZ * CM_TO_METER);

			Y = 20.f;
			MaxTrackedValues = OscillatorSystem->MaxTrackedValues;
			DrawRect(FLinearColor(0, 0, 0, 0.5),  BottomLeftCorner.X, TopLeftCorner.Y, GraphSize, GraphSize);
			DrawGraphData(OscillatorSystem->SpringDisplacementsOverTime, FVector2D(-100.f, 100.f), FColor::Blue);
			DrawAmplitude(OscillatorSystem->Amplitude, OscillatorSystem->LastPeriodIndex, FVector2D(-100.f, 100.f), FColor::Red);
			DrawPeriod(OscillatorSystem->Period, OscillatorSystem->LastPeriodIndex, OscillatorSystem->PeriodIndex, FColor::Green);
		}
	}	
}

void ADebugHUD::DrawGraphData(const TArray<float>& Values, const FVector2D YValuesInterval, const FLinearColor Color)
{	
	for (int32 i = 1; i < Values.Num(); i++)
	{	
		const float PreviousValue = Values[i - 1];
		const float CurrentValue = Values[i];

		const float PreviousMappedY = FMath::GetMappedRangeValueClamped(
			FVector2D(YValuesInterval.X, YValuesInterval.Y),
			FVector2D(BottomLeftCorner.Y, TopLeftCorner.Y),
			PreviousValue * INVERSE_GRAPH
		);

		const float CurrentMappedY = FMath::GetMappedRangeValueClamped(
			FVector2D(YValuesInterval.X, YValuesInterval.Y),
			FVector2D(BottomLeftCorner.Y, TopLeftCorner.Y),
			CurrentValue * INVERSE_GRAPH
		);

		const FVector2D StartPoint(TopLeftCorner.X + (GraphSize/ MaxTrackedValues) * i - 1, PreviousMappedY);
		const FVector2D EndPoint(TopLeftCorner.X  + (GraphSize/ MaxTrackedValues) * i, CurrentMappedY);

		DrawLine(StartPoint.X, StartPoint.Y, EndPoint.X, EndPoint.Y, Color);		
	}

	if(Values.Num() > 0)
	{
		const FString DisplayText = FString::Printf(TEXT("Displacement: %f [cm]"), Values.Last());
		FCanvasTextItem item1(FVector2D(TopLeftCorner.X - HorizontalOffset, Y * 1), FText::FromString(DisplayText), MainFont, Color);
		item1.EnableShadow(FLinearColor(0.f, 0.f, 0.f));
		Canvas->DrawItem(item1);	
	}	
}

void ADebugHUD::DrawAmplitude(const float Amplitude, const int32 AmpIndex, const FVector2D YValuesInterval, const FLinearColor Color)
{
	const float CurrentMappedY = FMath::GetMappedRangeValueClamped(
			FVector2D(YValuesInterval.X, YValuesInterval.Y),
			FVector2D(BottomLeftCorner.Y, TopLeftCorner.Y),
			Amplitude * INVERSE_GRAPH
		);

	const FVector2D StartPoint(TopLeftCorner.X + (GraphSize/ MaxTrackedValues) * AmpIndex, Y + (BottomLeftCorner.Y - TopLeftCorner.Y) *.5f);
	const FVector2D EndPoint(TopLeftCorner.X  + (GraphSize/ MaxTrackedValues) * AmpIndex, CurrentMappedY);
	DrawLine(StartPoint.X, StartPoint.Y, EndPoint.X, EndPoint.Y, Color);

	const FString DisplayText = FString::Printf(TEXT("Amplitude: %f [cm]"), Amplitude);
	FCanvasTextItem item1(FVector2D(TopLeftCorner.X - HorizontalOffset, Y * 2), FText::FromString(DisplayText), MainFont, Color);
	item1.EnableShadow(FLinearColor(0.f, 0.f, 0.f));
	Canvas->DrawItem(item1);	
}

void ADebugHUD::DrawPeriod(const float Period, const int32 StartIndex, const int32 EndIndex, const FLinearColor Color)
{	
	const FVector2D StartPoint(BottomLeftCorner.X + (GraphSize/ MaxTrackedValues) * StartIndex, BottomLeftCorner.Y - Y);
	const FVector2D EndPoint(TopLeftCorner.X  + (GraphSize/ MaxTrackedValues) * EndIndex, BottomLeftCorner.Y - Y);
	DrawLine(StartPoint.X, StartPoint.Y, EndPoint.X, EndPoint.Y, Color);

	const FString DisplayText = FString::Printf(TEXT("Period: %f [s]"), Period);
	FCanvasTextItem item1(FVector2D(TopLeftCorner.X - HorizontalOffset, Y * 3), FText::FromString(DisplayText), MainFont, Color);
	item1.EnableShadow(FLinearColor(0.f, 0.f, 0.f));
	Canvas->DrawItem(item1);
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