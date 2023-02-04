#include "TWABlueprintLibrary.h"

#include "Windable.h"

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