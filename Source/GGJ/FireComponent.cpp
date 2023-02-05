// Fill out your copyright notice in the Description page of Project Settings.


#include "FireComponent.h"
#include "Flammable.h"

// Sets default values for this component's properties
UFireComponent::UFireComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UFireComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFireComponent::OnFireComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{  
  
  if (OtherActor->GetClass()->ImplementsInterface(UFlammable::StaticClass()) &&
      ! IFlammable::Execute_IsOnFire(OtherActor))
  {
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Fire Spread !!!"));
    IFlammable::Execute_CatchFire(OtherActor);
  }
  else
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, OtherActor->GetActorLabel());
}

void UFireComponent::DelegateComponentHit(UPrimitiveComponent* componentUsedForOverlap)
{
  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Delegated"));
  componentUsedForOverlap->OnComponentHit.AddDynamic(this, &UFireComponent::OnFireComponentHit);
}
  
