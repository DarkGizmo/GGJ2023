

#include "TWAPawn.h"

#include "TWAController.h"

void ATWAPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (ATWAController* controller = Cast<ATWAController>(GetController()))
	{
		controller->SetupPlayerInputComponent(PlayerInputComponent);
	}
}