#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "TWABlueprintLibrary.generated.h"

//////////////////////////////////////////////////////////////////////////

UCLASS()
class GGJ_API UTWABlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Windable", meta=(DefaultToSelf = "selfObject"))
	static void RegisterWindable(UObject* selfObject);

	UFUNCTION(BlueprintCallable, Category = "Windable", meta = (DefaultToSelf = "selfObject"))
	static void UnregisterWindable(UObject* selfObject);

	UFUNCTION(BlueprintCallable, Category = "Windable", meta = (DefaultToSelf = "selfObject"))
	static void SimplePhysicsWind(UObject* selfObject, const FVector& windForce, float clampForce = -1.0f);
};