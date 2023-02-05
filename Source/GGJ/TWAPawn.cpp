

#include "TWAPawn.h"

#include "EngineUtils.h"

#include "TWAUtilities.h"
#include "TWAController.h"
#include "CameraLimitVolume.h"

void ATWAPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATWAPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (ATWAController* controller = Cast<ATWAController>(GetController()))
	{
		controller->SetupPlayerInputComponent(PlayerInputComponent);
	}

	RecomputeViewTargets();

	SetActorRotation(FRotator::ZeroRotator);
}

void ATWAPawn::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	FVector viewLocation = Utils::Approach(GetActorLocation(), GetTargetViewLocation(), CameraTargetLocationApproach, deltaTime);

	SetActorLocation(viewLocation, false);
}

FVector ATWAPawn::GetTargetViewLocation() const
{
	int targetCount = 0;
	FVector targetViewLocation = FVector::Zero();
	for (AActor* viewTarget : ViewTargets)
	{
		if (viewTarget)
		{
			++targetCount;

			FVector offset = FVector::ZeroVector;

			if (UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(viewTarget->GetRootComponent()))
			{
				if (primComp != nullptr && primComp->IsSimulatingPhysics())
				{
					TWEAKABLE float CameraVelocityOffsetTime = 0.65f;
					offset = primComp->GetComponentVelocity() * CameraVelocityOffsetTime;
				}
			}

			targetViewLocation += viewTarget->GetActorLocation() + offset;
		}
	}

	FVector viewLocation = GetActorLocation();

	if (targetCount > 0)
	{
		targetViewLocation /= (float)targetCount;

		for (ACameraLimitVolume* cameraLimit : CameraLimits)
		{
			if (!cameraLimit->IsInsideLimits(targetViewLocation))
			{
				cameraLimit->ClampViewTargetLocation(targetViewLocation);
			}
		}

		viewLocation = targetViewLocation;
	}

	return viewLocation;
}

void ATWAPawn::RecomputeViewTargets()
{
	bool bHadNoTarget = ViewTargets.IsEmpty();
	ViewTargets.Empty();
	CameraLimits.Empty();

	TArray<ACameraLimitVolume*> cameraLimitVolumes;

	for (TActorIterator<ACameraLimitVolume> cameraLimitVolume(GetWorld()); cameraLimitVolume; ++cameraLimitVolume)
	{
		if ((*cameraLimitVolume) != nullptr)
		{
			cameraLimitVolumes.Add(Cast<ACameraLimitVolume>(*cameraLimitVolume));
		}
	}

	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (FActorIterator It(World); It; ++It)
		{
			AActor* actor = *It;
			if (actor->ActorHasTag("CameraViewTarget"))
			{
				ViewTargets.Add(actor);

				for (ACameraLimitVolume* cameraLimit : cameraLimitVolumes)
				{
					if(cameraLimit && !cameraLimit->bExited)
					{
						TArray<AActor*> overlappingActors;
						cameraLimit->GetOverlappingActors(overlappingActors);

						if (overlappingActors.Contains(actor))
						{
							CameraLimits.AddUnique(cameraLimit);
						}
					}
				}
			}
		}
	}

	if (bHadNoTarget && !bEverHadTargets && ViewTargets.Num() > 0)
	{
		// SNap to position
		SetActorLocation(GetTargetViewLocation(), false);

		bEverHadTargets = true;
	}
}