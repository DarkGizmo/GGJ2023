#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Windable.generated.h"

class IWindObjectCPP
{
public:
	
};

UINTERFACE(BlueprintType)
class GGJ_API UWindable : public UInterface
{
	GENERATED_BODY()

public:
	static void RegisterWindable(UObject* windable);
	static void UnregisterWindable(UObject* windable);

	static void SimplePhysicsWind(UObject* windable, const FVector& windForce);
};

class GGJ_API IWindable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (DisplayName = "Wind"))
	void BP_Wind(const FVector& windForce);
	virtual void BP_Wind_Implementation(const FVector& windForce)
	{
		// Maybe implemented
	}
};