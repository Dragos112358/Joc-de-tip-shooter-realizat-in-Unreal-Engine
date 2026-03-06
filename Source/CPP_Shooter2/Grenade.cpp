#include "Grenade.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Enemy_Bot.h"
#include "EnemyRangedBot.h"
#include "BossEnemyBot.h"
#include "EngineUtils.h"



AGrenade::AGrenade()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComponent->InitSphereRadius(20.f);
    CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    RootComponent = CollisionComponent;
    CollisionComponent->SetSimulatePhysics(true);

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/bomba.bomba'"));
    if (MeshAsset.Succeeded())
    {
        MeshComponent->SetStaticMesh(MeshAsset.Object);
        MeshComponent->SetWorldScale3D(FVector(0.1f));
    }
    ImpactParticles2 = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), nullptr, TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion")));
    if (ImpactParticles2)
    {
        UE_LOG(LogTemp, Warning, TEXT("Particula a fost încărcată cu succes"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Eroare la încărcarea particulei"));
    }

}

void AGrenade::BeginPlay()
{
    Super::BeginPlay();

    // 🔥 Mărește gravitația de 3x (default este 1.0)
    CollisionComponent->SetEnableGravity(true);
    CollisionComponent->SetMassOverrideInKg(NAME_None, 1.f, true);
    CollisionComponent->SetLinearDamping(0.1f);
    CollisionComponent->SetAngularDamping(0.1f);
    CollisionComponent->SetPhysicsLinearVelocity(GetActorForwardVector() * 2000.f); // viteză inițială
    CollisionComponent->SetPhysicsAngularVelocityInDegrees(FVector(0.f, 0.f, 500.f)); // rotire în aer

    // ⏳ Timer de explozie
    GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AGrenade::Explode, 3.0f, false);
}

void AGrenade::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGrenade::Explode()
{
    // Spawn particula de explozie la locația grenzii
    if (ImpactParticles2)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles2, GetActorLocation(), FRotator::ZeroRotator, FVector(3.0f));
    }

    // 💣 DAMAGE PENTRU BOTS ÎN RAZA DE 500 UNITĂȚI
    float DamageRadius = 500.f;
    float GrenadeDamage = 100.f; // Damage mare pentru grenadă

    // 🤖 Damage pentru Enemy Bots
    for (TActorIterator<AEnemy_Bot> It(GetWorld()); It; ++It)
    {
        AEnemy_Bot* Bot = *It;
        if (!Bot) continue;

        float Distance = FVector::Dist(GetActorLocation(), Bot->GetActorLocation());
        if (Distance <= DamageRadius && Bot->CurrentHealth > 0)
        {
            Bot->TakeDamage(GrenadeDamage);
            UE_LOG(LogTemp, Warning, TEXT("💥 Grenada a aplicat %.1f damage la Enemy Bot (distanța: %.1f)"), GrenadeDamage, Distance);
        }
    }

    // 🎯 Damage pentru Ranged Bots
    for (TActorIterator<AEnemyRangedBot> RangedIt(GetWorld()); RangedIt; ++RangedIt)
    {
        AEnemyRangedBot* RangedBot = *RangedIt;
        if (!RangedBot) continue;

        float Distance = FVector::Dist(GetActorLocation(), RangedBot->GetActorLocation());
        if (Distance <= DamageRadius && RangedBot->CurrentHealth > 0)
        {
            RangedBot->TakeDamage(GrenadeDamage);
            UE_LOG(LogTemp, Warning, TEXT("💥 Grenada a aplicat %.1f damage la Ranged Bot (distanța: %.1f)"), GrenadeDamage, Distance);
        }
    }

    // 👹 Damage pentru Boss
    for (TActorIterator<ABossEnemyBot> BossIt(GetWorld()); BossIt; ++BossIt)
    {
        ABossEnemyBot* Boss = *BossIt;
        if (!Boss) continue;

        float Distance = FVector::Dist(GetActorLocation(), Boss->GetActorLocation());
        if (Distance <= DamageRadius && Boss->CurrentHealth > 0)
        {
            Boss->TakeDamage(GrenadeDamage);
            UE_LOG(LogTemp, Warning, TEXT("💥 Grenada a aplicat %.1f damage la BOSS (distanța: %.1f)"), GrenadeDamage, Distance);
        }
    }

    // 💥 IMPULS FIZIC (raza mai mare pentru efecte vizuale)
    float ExplosionRadius = 1000.f;
    float ExplosionStrength = 1500.f;

    // Detectează toate actorii în raza exploziei
    TArray<FOverlapResult> Overlaps;
    FCollisionShape CollShape = FCollisionShape::MakeSphere(ExplosionRadius);
    bool bHasOverlaps = GetWorld()->OverlapMultiByChannel(
        Overlaps,
        GetActorLocation(),
        FQuat::Identity,
        ECC_PhysicsBody, // canal pentru obiecte cu fizica
        CollShape
    );

    if (bHasOverlaps)
    {
        for (auto& Result : Overlaps)
        {
            UPrimitiveComponent* PrimComp = Result.GetComponent();
            if (PrimComp && PrimComp->IsSimulatingPhysics())
            {
                // Aplică un impuls radial, centrat pe locația grenadei
                PrimComp->AddRadialImpulse(GetActorLocation(), ExplosionRadius, ExplosionStrength, ERadialImpulseFalloff::RIF_Linear, true);
            }
        }
    }

    // 🗑️ Distruge grenada
    Destroy();
}
