#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Grenade.generated.h"

UCLASS()
class CPP_SHOOTER2_API AGrenade : public AActor
{
	GENERATED_BODY()

public:
	AGrenade();

protected:
	virtual void BeginPlay() override;
	void Explode();

public:
	virtual void Tick(float DeltaTime) override;

	// Component pointers
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;
	FTimerHandle ExplosionTimerHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ImpactParticles2;
};
