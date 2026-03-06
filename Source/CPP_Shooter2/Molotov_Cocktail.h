#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h" // ✅ Lipsă la tine!

#include "Molotov_Cocktail.generated.h"

UCLASS()
class CPP_SHOOTER2_API AMolotov_Cocktail : public AActor
{
	GENERATED_BODY()

public:
	AMolotov_Cocktail(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Molotov")
	UParticleSystem* FireEffect;

	UPROPERTY()
	UParticleSystemComponent* ActiveFireEffect;
	
	UPROPERTY()
	UParticleSystemComponent* ActiveSmokeEffect;

	FTimerHandle SmokeTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Damage")
	float FireDamageRadius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Damage")
	float FireDamagePerSecond = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Damage")
	float FireDuration = 5.f;

	UPROPERTY()
	FTimerHandle DestroyTimerHandle;

	UPROPERTY()
	FTimerHandle DamageTimerHandle;


	UFUNCTION()
	void DeactivateAndDestroy();
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void ApplyFireDamage();

protected:
	virtual void BeginPlay() override;
};
