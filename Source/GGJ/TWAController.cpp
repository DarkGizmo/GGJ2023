
#include "TWAController.h"

#include "Curves/CurveFloat.h"
#include "Curves/RichCurve.h"
#include "DrawDebugHelpers.h"

#include "TWAUtilities.h"
#include "TWAGameModeBase.h"
#include "Windable.h"
#include "TWAPawn.h"

void ATWAController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("WindForward", this, &ATWAController::WindForward);
	PlayerInputComponent->BindAxis("WindRight", this, &ATWAController::WindRight);

	PlayerInputComponent->BindAction("BoostWind", IE_Pressed, this, &ATWAController::BoostWindPressed);
	PlayerInputComponent->BindAction("BoostWind", IE_Released, this, &ATWAController::BoostWindReleased);
}

void ATWAController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if(GetWorld() && !GetWorld()->IsPaused())
	{
		// Regen boost
		if (WindBoostBurnedOutDuration > 0.0f)
		{
			WindBoostBurnedOutDuration = FMath::Max(WindBoostBurnedOutDuration - deltaTime, 0.0f);
		}

		float windBoostElapsedTime = Utils::ElapsedTime(WindBoostStartedTime);

		if (bWindWantsBoost && BoostCurveMultiplier != nullptr && windBoostElapsedTime > BoostCurveMultiplier->FloatCurve.GetLastKey().Time)
		{
			bWindWantsBoost = false;
			WindBoostBurnedOutDuration = BoostRegenDuration;
		}

		float windBoostRatio = BoostCurveMultiplier && bWindWantsBoost ? BoostCurveMultiplier->GetFloatValue(windBoostElapsedTime) : 1.0f;

		float windInputRatio = bWindWantsBoost ? 1.0f : 0.0f;
		if(!WindInput2D.IsNearlyZero())
		{
			float desiredWindAngle = FMath::RadiansToDegrees(FMath::Atan2(WindInput2D.Y, WindInput2D.X));
			if (FMath::IsNearlyZero(WindSpeed, 0.01f))
			{
				WindAngle = desiredWindAngle;
			}
			else
			{
				WindAngle = Utils::ApproachAngle(WindAngle, desiredWindAngle, WindAngleApproach, deltaTime);
			}

			windInputRatio = WindInput2D.Size();
		}

		float targetWindSpeed = windInputRatio * windBoostRatio;
		WindSpeed = Utils::Approach(WindSpeed, targetWindSpeed, WindSpeedApproach, deltaTime);
		
		float windAngleRad = FMath::DegreesToRadians(WindAngle);
		FVector windForce = FVector(FMath::Cos(windAngleRad), FMath::Sin(windAngleRad), 0.0f) * WindSpeed;

		if(!windForce.IsNearlyZero(0.01f))
		{
			if (ATWAGameModeBase* gameMode = Utils::GetGameMode())
			{
				for (UObject* object : gameMode->WindableList)
				{
					if(object != nullptr)
					{
						IWindable::Execute_Wind(object, windForce);
					}
				}
			}
		}

		if (ATWAPawn* pawn = Cast<ATWAPawn>(GetPawn()))
		{
			TWEAKABLE float WindForceDebugDrawMultiplier = 100.0f;
			DrawDebugDirectionalArrow(GetWorld(), pawn->GetActorLocation() + VecZ(100.0f), pawn->GetActorLocation() + VecZ(100.0f) + windForce * WindForceDebugDrawMultiplier, 25.0f, FColor::Blue);
		}
	}
}

void ATWAController::WindForward(float value)
{
	WindInput2D.X = value;
}

void ATWAController::WindRight(float value)
{
	WindInput2D.Y = value;
}

void ATWAController::BoostWindPressed()
{
	if(!bWindWantsBoost && WindBoostBurnedOutDuration <= 0.0f)
	{
		bWindWantsBoost = true;
		WindBoostStartedTime = Utils::GetGameTime();
		OnBoostStarted.Broadcast();
	}
}

void ATWAController::BoostWindReleased()
{
	if (bWindWantsBoost && BoostCurveMultiplier != nullptr)
	{
		bWindWantsBoost = false;
		float windBoostElapsedTime = FMath::Max(Utils::ElapsedTime(WindBoostStartedTime), MinimumReboostInterval);
		WindBoostBurnedOutDuration = MapClamped(windBoostElapsedTime, 0.0f, BoostCurveMultiplier->FloatCurve.GetLastKey().Time, 0.0f, BoostRegenDuration);
		OnBoostEnded.Broadcast();
	}
	
}