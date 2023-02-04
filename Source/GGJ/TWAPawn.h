#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TWAPawn.generated.h"

/**
 * 
 */
UCLASS()
class GGJ_API ATWAPawn : public APawn
{
	GENERATED_BODY()
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

};
