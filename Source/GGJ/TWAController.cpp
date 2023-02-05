
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
		ATWAGameModeBase* gameMode = Utils::GetGameMode();

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
			OnBoostEnded.Broadcast(windBoostElapsedTime);
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
			if (gameMode)
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

		if (gameMode)
		{
			FVector eyeLocation;
			FRotator eyeRotation;
			Utils::GetPawn()->GetActorEyesViewPoint(eyeLocation, eyeRotation);

			TWEAKABLE float EyeXOffset = 360.0f;
			TWEAKABLE float EyeXLimit = 2500.0f;
			TWEAKABLE float EyeYLimit = 4000.0f;
			eyeLocation += FVector(EyeXOffset, 0.0f, 0.0f);

			for (UObject* object : gameMode->WindClutter)
			{
				if (AActor* actor = Cast<AActor>(object))
				{
					FVector clutterLocation = actor->GetActorLocation();
					FVector toClutter = actor->GetActorLocation() - eyeLocation;
					if (toClutter.X < -EyeXLimit)
					{
						FVector newLocation = eyeLocation + FVector::ForwardVector * EyeXLimit;
						newLocation.Z = clutterLocation.Z;
						actor->SetActorLocation(newLocation);
					}
					else if (toClutter.X > EyeXLimit)
					{
						FVector newLocation = eyeLocation - FVector::ForwardVector * EyeXLimit;
						newLocation.Z = clutterLocation.Z;
						actor->SetActorLocation(newLocation);
					}

					if (toClutter.Y < -EyeYLimit)
					{
						FVector newLocation = eyeLocation + FVector::RightVector * EyeYLimit;
						newLocation.Z = clutterLocation.Z;
						actor->SetActorLocation(newLocation);
					}
					else if (toClutter.Y > EyeYLimit)
					{
						FVector newLocation = eyeLocation - FVector::RightVector * EyeYLimit;
						newLocation.Z = clutterLocation.Z;
						actor->SetActorLocation(newLocation);
					}
				}
			}
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
		WindBoostBurnedOutDuration = FMath::Max(MapClamped(Utils::ElapsedTime(WindBoostStartedTime), 0.0f, BoostCurveMultiplier->FloatCurve.GetLastKey().Time, 0.0f, BoostRegenDuration), MinimumReboostInterval);
		OnBoostEnded.Broadcast(MapClamped(Utils::ElapsedTime(WindBoostStartedTime), 0.0f, BoostCurveMultiplier->FloatCurve.GetLastKey().Time, 0.0f, BoostRegenDuration));
	}
}
