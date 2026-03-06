// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBotAIController.h"

#include "EnemyBotAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy_Bot.h"
#include "EnemyRangedBot.h"




AEnemyBotAIController::AEnemyBotAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}
void AEnemyBotAIController::BeginPlay()
{
    Super::BeginPlay();

    ControlledPawn = GetPawn();
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    bIsRangedBot = Cast<AEnemyRangedBot>(ControlledPawn) != nullptr;

    if (ControlledPawn)
    {
        ACharacter* ControlledCharacter = Cast<ACharacter>(ControlledPawn);
        if (ControlledCharacter && ControlledCharacter->GetCharacterMovement())
        {
            ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = 100.f; // <-- Setează viteza aici
            UE_LOG(LogTemp, Warning, TEXT("🚶‍♂️ Viteza botului setată la 150"));
        }
    }
}

void AEnemyBotAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!ControlledPawn) ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    if (!PlayerCharacter) PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!PlayerCharacter) return;
    bIsRangedBot = Cast<AEnemyRangedBot>(ControlledPawn) != nullptr;
    TimeSinceLastMove += DeltaSeconds;
    if (TimeSinceLastMove < MoveInterval)
    {
        return; // nu facem nimic încă
    }
    TimeSinceLastMove = 0.f;

    // 👉 Dacă e RANGED BOT, stă la distanță și nu se mișcă
    if (bIsRangedBot)
    {
        float DistanceToPlayer = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());

        AEnemyRangedBot* RangedBot = Cast<AEnemyRangedBot>(ControlledPawn);
        if (!RangedBot) return;

        // Dacă e în range, NU se mișcă
        if (DistanceToPlayer <= RangedBot->AttackRange)
        {
            StopMovement();
            UE_LOG(LogTemp, Warning, TEXT("🔫 Botul ranged este în range - NU se apropie."));
            FVector Direction = (PlayerCharacter->GetActorLocation() - ControlledPawn->GetActorLocation()).GetSafeNormal2D();
            FRotator NewRotation = Direction.Rotation();
            ControlledPawn->SetActorRotation(NewRotation);

            UE_LOG(LogTemp, Warning, TEXT("🔫 Botul ranged este în range - NU se apropie, dar se orientează."));
            return;
        }

        // Dacă e prea departe, se poziționează
        EPathFollowingRequestResult::Type Result = MoveToActor(PlayerCharacter, RangedBot->AttackRange * 0.8f);
        if (Result == EPathFollowingRequestResult::RequestSuccessful)
        {
            UE_LOG(LogTemp, Warning, TEXT("🚶‍♂️ Botul ranged se poziționează la distanță."));
            bIsMoving = true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("❌ Botul ranged nu a putut porni deplasarea."));
        }

        return; // ⛔ evităm execuția de mai jos pentru melee
    }

    // 👉 Dacă e MELEE BOT
    if (HasLineOfSightToPlayer())
    {
        MoveToPlayer(); // folosește comportamentul de delay pentru MELEE
    }
    else
    {
        StopMovement();
        bIsMoving = false;
        bIsWaitingToMove = false;
        GetWorld()->GetTimerManager().ClearTimer(DelayMoveHandle);
        UE_LOG(LogTemp, Warning, TEXT("🚫 Botul melee nu îl vede pe player, stă pe loc."));
    }
}



bool AEnemyBotAIController::HasLineOfSightToPlayer()
{
    if (!ControlledPawn || !PlayerCharacter) return false;

    FVector Start = ControlledPawn->GetActorLocation() + FVector(0.f, 0.f, 50.f); // ridică un pic punctul de start
    FVector End = PlayerCharacter->GetActorLocation() + FVector(0.f, 0.f, 50.f);  // ridică un pic punctul țintă

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(ControlledPawn);  // să nu lovească pe sine însuși

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

    if (bHit)
    {
        // Dacă primul obiect lovit e jucătorul, înseamnă că e vizibil
        return HitResult.GetActor() == PlayerCharacter;
    }

    // Nu a lovit nimic => drum liber către player
    return true;
}


void AEnemyBotAIController::DelayedMoveToPlayer()
{
    bIsWaitingToMove = false;
    bIsMoving = true;

    ACharacter* ControlledCharacter = Cast<ACharacter>(ControlledPawn);
    if (ControlledCharacter && ControlledCharacter->GetCharacterMovement())
    {
        ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = 100.f;
        UE_LOG(LogTemp, Warning, TEXT("🏃‍♂️ Botul a pornit spre player!"));
    }

    EPathFollowingRequestResult::Type Result = MoveToActor(PlayerCharacter, 20.f);
    if (Result == EPathFollowingRequestResult::RequestSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("✅ MoveToActor după delay a început cu succes."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ MoveToActor după delay a eșuat cu codul: %d"), (int)Result);
        bIsMoving = false; // ❗ Dacă nu a mers, resetăm
    }
}



void AEnemyBotAIController::MoveToPlayer()
{
    if (!PlayerCharacter || !ControlledPawn) return;

    // ❌ Nu face nimic dacă deja merge sau dacă e în așteptare
    if (bIsWaitingToMove || bIsMoving) return;

    bIsWaitingToMove = true;

    ACharacter* ControlledCharacter = Cast<ACharacter>(ControlledPawn);
    if (ControlledCharacter && ControlledCharacter->GetCharacterMovement())
    {
        ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = 0.f;
        UE_LOG(LogTemp, Warning, TEXT("⏳ Botul va porni în 2 secunde..."));
    }

    GetWorld()->GetTimerManager().SetTimer(
        DelayMoveHandle,
        this,
        &AEnemyBotAIController::DelayedMoveToPlayer,
        2.f,
        false
    );
}
void AEnemyBotAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    bIsMoving = false;

    UE_LOG(LogTemp, Warning, TEXT("🛑 Botul a terminat deplasarea către player"));
}





