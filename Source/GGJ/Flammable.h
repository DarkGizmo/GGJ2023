// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Flammable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UFlammable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GGJ_API IFlammable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BluePrintCallable, BlueprintNativeEvent)
	void CatchFire();
};
