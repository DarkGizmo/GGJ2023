#include "TWABlueprintLibrary.h"

#include "TWAController.h"
#include "TWAUtilities.h"
#include "Windable.h"

class ATWAPawn* UTWABlueprintLibrary::GetLocalPawn()
{
	return Utils::GetPawn();
}

ATWAController* UTWABlueprintLibrary::GetLocalController()
{
	return Utils::GetPlayerController();
}

float UTWABlueprintLibrary::GetRemainingBoostPercentage()
{
	if (ATWAController* controller = Utils::GetPlayerController())
	{
		if (controller->BoostCurveMultiplier != nullptr)
		{
			float windBoostElapsedTime = Utils::ElapsedTime(controller->WindBoostStartedTime);
			return (controller->BoostCurveMultiplier->FloatCurve.GetLastKey().Time - windBoostElapsedTime)/controller->BoostCurveMultiplier->FloatCurve.GetLastKey().Time;
		}
	}
	
	return 0.0f;
}

float UTWABlueprintLibrary::GetBoostCooldownPercentage()
{
	if (ATWAController* controller = Utils::GetPlayerController())
	{
		return (controller->BoostRegenDuration - controller->WindBoostBurnedOutDuration)/controller->BoostRegenDuration;
	}

	return 0.0f;
}

void UTWABlueprintLibrary::RegisterWindable(UObject* self)
{
	UWindable::RegisterWindable(self);
}

void UTWABlueprintLibrary::UnregisterWindable(UObject* self)
{
	UWindable::UnregisterWindable(self);
}

void UTWABlueprintLibrary::SimplePhysicsWind(UObject* selfObject, const FVector& windForce, float clampForce)
{
	UWindable::SimplePhysicsWind(selfObject, windForce, clampForce);
}