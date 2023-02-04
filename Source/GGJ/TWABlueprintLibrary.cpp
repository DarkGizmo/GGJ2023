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