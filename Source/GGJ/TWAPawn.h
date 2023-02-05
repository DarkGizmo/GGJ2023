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

public:
	UPROPERTY(EditDefaultsOnly)
	float CameraTargetLocationApproach = 0.9f;

	UPROPERTY(Transient)
	TArray<AActor*> ViewTargets;

	UPROPERTY(Transient)
	TArray<class ACameraLimitVolume*> CameraLimits;
	
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float deltaTime) override;

	FVector GetTargetViewLocation() const;


	UFUNCTION(BlueprintCallable)
	void RecomputeViewTargets();

	bool bEverHadTargets = false;

};
