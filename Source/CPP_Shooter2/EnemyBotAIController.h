// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyBotAIController.generated.h"

/**
 * 
 */
UCLASS()
class CPP_SHOOTER2_API AEnemyBotAIController : public AAIController
{
	GENERATED_BODY()
public:
    //constructor
    AEnemyBotAIController();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    bool HasLineOfSightToPlayer();
    FTimerHandle DelayMoveHandle;
    bool bIsWaitingToMove = false;
    void DelayedMoveToPlayer();

    void MoveToPlayer();
    float TimeSinceLastMove = 0.f;
    float MoveInterval = 1.0f;  // aproximativ 10 frame-uri la 60 FPS
    bool bIsMoving = false;
    // EnemyBotAIController.h
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
    APawn* ControlledPawn;
    ACharacter* PlayerCharacter;
protected:
    UPROPERTY()
    bool bIsRangedBot = false;

	
};
