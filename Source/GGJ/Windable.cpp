
#include "Windable.h"

#include "TWAUtilities.h"
#include "TWAGameModeBase.h"

void UWindable::RegisterWindable(UObject* windable)
{
	if (ATWAGameModeBase* gameMode = Utils::GetGameMode())
	{
		gameMode->WindableList.Add(windable);
	}
}

void UWindable::UnregisterWindable(UObject* windable)
{
	if (ATWAGameModeBase* gameMode = Utils::GetGameMode())
	{
		gameMode->WindableList.Remove(windable);
	}
}

void UWindable::SimplePhysicsWind(UObject* windable, const FVector& windForce)
{

}

