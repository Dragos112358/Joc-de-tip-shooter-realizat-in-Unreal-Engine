// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Smoke_Grenade.generated.h"

UCLASS()
class CPP_SHOOTER2_API ASmoke_Grenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmoke_Grenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY()
	class UParticleSystemComponent* ActiveSmokeEffect;  // <--- Adaugă asta

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* SmokeParticles;
	UFUNCTION()
	void DeactivateAndDestroy2();

	FTimerHandle SmokeTimerHandle;

	void ActivateSmoke();

	float EffectElapsedTime = 0.f;
	float EffectDuration = 10.f;  // total
	float FadeInTime = 3.f;
	float HoldTime = 4.f;
	float FadeOutTime = 3.f;
	void UpdateSmokeEffectScale(float DeltaTime);

};
