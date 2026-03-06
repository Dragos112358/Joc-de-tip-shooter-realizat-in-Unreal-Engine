// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "Projectiles.generated.h"


UCLASS()
class CPP_SHOOTER2_API AProjectiles : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectiles();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    UParticleSystem* DustParticles;
    UPROPERTY(VisibleAnywhere)
    class UProjectileMovementComponent* ProjectileMovement;
    UPROPERTY(VisibleAnywhere)
    class USphereComponent* CollisionComp;

    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* MeshComp;


    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    UParticleSystem* MuzzleEffect;


    UParticleSystemComponent* ActiveDustEffect;
protected:
    UPROPERTY()
    UMaterialInterface* BulletHoleDecalMaterial;

private:

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);
};

