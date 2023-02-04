
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

void UWindable::SimplePhysicsWind(UObject* windable, const FVector& windForce, float clampForce)
{
	FVector force = windForce;
	
	if(clampForce > 0.0f)
	{
		force = force.GetClampedToMaxSize(clampForce);
	}

	if (UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(windable))
	{
		if (primComp->IsSimulatingPhysics())
		{
			primComp->AddImpulse(force);
		}
	}

	if(AActor* actor = Cast<AActor>(windable))
	{
		TArray<UPrimitiveComponent*> primitiveComponents;
		actor->GetComponents(primitiveComponents);

		for(UPrimitiveComponent* primComp : primitiveComponents)
		{
			if (primComp != nullptr)
			{
				if (primComp->IsSimulatingPhysics())
				{
					primComp->AddImpulse(force);
				}
			}
		}
	}
}

