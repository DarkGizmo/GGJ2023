#include "TWABlueprintLibrary.h"

#include "TWAUtilities.h"
#include "Windable.h"

class ATWAPawn* UTWABlueprintLibrary::GetLocalPawn()
{
	return Utils::GetPawn();
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