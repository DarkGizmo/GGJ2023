

#include "TWAPawn.h"

#include "EngineUtils.h"

#include "TWAUtilities.h"
#include "TWAController.h"
#include "CameraLimitVolume.h"
#include "TWACheckpoint.h"

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

	for (int i = ViewTargets.Num() - 1; i >= 0; --i)
	{
		if (ViewTargets[i] == nullptr || ViewTargets[i]->IsPendingKill())
		{
			ViewTargets.RemoveAtSwap(i);
		}
	}

	if (ViewTargets.IsEmpty())
	{
		if(PlayerDeadTimestamp <= 0.0f)
		{
			OnPlayerDead();
			PlayerDeadTimestamp = Utils::GetGameTime();
		}
		else 
		{
			if (!Utils::IsRecentEvent(PlayerDeadTimestamp, DeadFadeOutDelay) && !bTriggeredFadeOut)
			{
				bTriggeredFadeOut = true;
				OnDeathFadeOut();
			}

			if (!Utils::IsRecentEvent(PlayerDeadTimestamp, RespawnDelay))
			{
				if (LevelCameraLimit != nullptr && LevelCameraLimit->Checkpoint != nullptr)
				{
					LevelCameraLimit->bExited = false;
					LevelCameraLimit->Checkpoint->Respawn();
					PlayerDeadTimestamp = -1.0f;
					bTriggeredFadeOut = false;
					SetActorLocation(GetTargetViewLocation(), false);
					OnPlayerRespawn();
				}
			}
		}
	}

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

	if (CameraLimits.Num() > 0)
	{
		LevelCameraLimit = CameraLimits[CameraLimits.Num() - 1];
	}
}