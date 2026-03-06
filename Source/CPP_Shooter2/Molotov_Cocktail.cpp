#include "Molotov_Cocktail.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Enemy_Bot.h"
#include "EnemyRangedBot.h"
#include "BossEnemyBot.h"
#include "EngineUtils.h" // Pentru TActorIterator


AMolotov_Cocktail::AMolotov_Cocktail(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComponent->InitSphereRadius(15.f);
    CollisionComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetSimulatePhysics(true);

    RootComponent = CollisionComponent;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/StarterContent/source/MOLOTOV_Bottle.MOLOTOV_Bottle"));
    if (MeshAsset.Succeeded())
    {
        MeshComponent->SetStaticMesh(MeshAsset.Object);
        MeshComponent->SetWorldScale3D(FVector(1.f));
    }

    FireEffect = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), nullptr, TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire")));
}




void AMolotov_Cocktail::BeginPlay()
{
    Super::BeginPlay();

    // Aici putem activa notificarea pentru coliziuni după ce fizica este activă
    CollisionComponent->SetNotifyRigidBodyCollision(true);

    CollisionComponent->OnComponentHit.AddDynamic(this, &AMolotov_Cocktail::OnHit);
}

void AMolotov_Cocktail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMolotov_Cocktail::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (FireEffect)
    {
        ActiveFireEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, GetActorLocation(), FRotator::ZeroRotator, FVector(3.f));
    }

    CollisionComponent->SetSimulatePhysics(false);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComponent->SetVisibility(false);

    // 🔥 Începe damage-ul periodic (la fiecare secundă)
    GetWorldTimerManager().SetTimer(
        DamageTimerHandle,
        this,
        &AMolotov_Cocktail::ApplyFireDamage,
        1.0f, // la fiecare secundă
        true  // repeating
    );

    // 🗑️ Timer pentru dezactivare și distrugere după 5 secunde
    GetWorldTimerManager().SetTimer(
        DestroyTimerHandle,
        this,
        &AMolotov_Cocktail::DeactivateAndDestroy,
        FireDuration,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("🔥 Molotov a izbucnit! Damage %.1f/sec pentru %.1f secunde în raza %.1f"),
        FireDamagePerSecond, FireDuration, FireDamageRadius);
}

void AMolotov_Cocktail::ApplyFireDamage()
{
    // 🔥 Aplică damage la toți bot-ii în raza de foc

    // 🤖 Damage pentru Enemy Bots
    for (TActorIterator<AEnemy_Bot> It(GetWorld()); It; ++It)
    {
        AEnemy_Bot* Bot = *It;
        if (!Bot) continue;

        float Distance = FVector::Dist(GetActorLocation(), Bot->GetActorLocation());
        if (Distance <= FireDamageRadius && Bot->CurrentHealth > 0)
        {
            Bot->TakeDamage(FireDamagePerSecond);
            UE_LOG(LogTemp, Warning, TEXT("🔥 Molotov aplică %.1f damage la Enemy Bot (distanța: %.1f)"), FireDamagePerSecond, Distance);
        }
    }

    // 🎯 Damage pentru Ranged Bots
    for (TActorIterator<AEnemyRangedBot> RangedIt(GetWorld()); RangedIt; ++RangedIt)
    {
        AEnemyRangedBot* RangedBot = *RangedIt;
        if (!RangedBot) continue;

        float Distance = FVector::Dist(GetActorLocation(), RangedBot->GetActorLocation());
        if (Distance <= FireDamageRadius && RangedBot->CurrentHealth > 0)
        {
            RangedBot->TakeDamage(FireDamagePerSecond);
            UE_LOG(LogTemp, Warning, TEXT("🔥 Molotov aplică %.1f damage la Ranged Bot (distanța: %.1f)"), FireDamagePerSecond, Distance);
        }
    }

    // 👹 Damage pentru Boss
    for (TActorIterator<ABossEnemyBot> BossIt(GetWorld()); BossIt; ++BossIt)
    {
        ABossEnemyBot* Boss = *BossIt;
        if (!Boss) continue;

        float Distance = FVector::Dist(GetActorLocation(), Boss->GetActorLocation());
        if (Distance <= FireDamageRadius && Boss->CurrentHealth > 0)
        {
            Boss->TakeDamage(FireDamagePerSecond);
            UE_LOG(LogTemp, Warning, TEXT("🔥 Molotov aplică %.1f damage la BOSS (distanța: %.1f)"), FireDamagePerSecond, Distance);
        }
    }
}

void AMolotov_Cocktail::DeactivateAndDestroy()
{
    // 🛑 Oprește timer-ul de damage
    GetWorldTimerManager().ClearTimer(DamageTimerHandle);

    // 🔥 Oprește efectul de foc
    if (ActiveFireEffect)
    {
        ActiveFireEffect->DeactivateSystem();
        ActiveFireEffect->DestroyComponent();
        ActiveFireEffect = nullptr;
    }

    UE_LOG(LogTemp, Warning, TEXT("🔥 Molotov s-a stins și se distruge"));
    Destroy();
}