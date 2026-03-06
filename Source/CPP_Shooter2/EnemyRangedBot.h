// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyRangedBot.generated.h"

UCLASS()
class CPP_SHOOTER2_API AEnemyRangedBot : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyRangedBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool IsPausedAfterStop() const { return bIsPausedAfterStop; }

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	float AnimationTimer = 0.f;
	FVector LastAnimStartLocation;
	FVector LocationBeforeStop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
	float CurrentHealth;

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void Die();
	void DestroyBot();

	UPROPERTY()
	float WalkAnimLength = 0.f;

	UPROPERTY()
	float WalkAnimTimeElapsed = 0.f;

	UPROPERTY()
	FVector StepDirection = FVector(5000.f, 0.f, 0.f); // Poți ajusta direcția și distanța

	FVector LastLocation;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PistolMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead = false;
	UFUNCTION()
	void ShootProjectile();
	float TimeSinceLastShot;
	float FireInterval = 1.5f;  // Intervalul de tragere în secunde
	float AttackRange = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* FireSound;


private:
	UAnimSequence* WalkAnim;
	UAnimSequence* AttackAnim;
	UAnimSequence* DieAnim;
	bool bIsPlayingWalkAnim = false;
	FVector LastStepPosition;
	bool bHasSavedLastStepPosition = false;
	bool bIsPausedAfterStop = false;
	FTimerHandle PauseTimerHandle;

	void ResumeMovement(); // funcție privată care reia mișcarea
};
