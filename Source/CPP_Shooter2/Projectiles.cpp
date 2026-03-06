#include "Projectiles.h"
#include "Enemy_Bot.h"
#include "EnemyRangedBot.h"
#include "BossEnemyBot.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "CPP_Shooter2HUD.h"

AProjectiles::AProjectiles()
{
    PrimaryActorTick.bCanEverTick = true;

    // Collision component
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(10.f);
    CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    CollisionComp->SetNotifyRigidBodyCollision(true);
    CollisionComp->BodyInstance.bNotifyRigidBodyCollision = true;
    CollisionComp->SetSimulatePhysics(false);
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);

    RootComponent = CollisionComp;

    // Mesh component
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> BulletMesh(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/bullet_shot.bullet_shot'"));
    if (BulletMesh.Succeeded())
    {
        MeshComp->SetStaticMesh(BulletMesh.Object);
        MeshComp->SetRelativeScale3D(FVector(0.075f));
        MeshComp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));  // exemplu, ajustează după cum trebuie

        UE_LOG(LogTemp, Warning, TEXT("Bullet mesh loaded successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load bullet mesh!"));
    }

    // Movement component
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->InitialSpeed = 10000.f;
    ProjectileMovement->MaxSpeed = 10000.f;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.0f;

    // OnHit event binding
    CollisionComp->OnComponentHit.AddDynamic(this, &AProjectiles::OnHit);

    // Load smoke particle system
    DustParticles = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), nullptr, TEXT("/Game/StarterContent/Particles/P_Smoke.P_Smoke")));
    if (DustParticles)
    {
        UE_LOG(LogTemp, Warning, TEXT("Dust particle loaded successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load smoke particle!"));
    }

    ActiveDustEffect = nullptr;

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> DecalMat(TEXT("/Script/Engine.Material'/Game/StarterContent/Textures/Impact_glont_Mat.Impact_glont_Mat'"));
    if (DecalMat.Succeeded())
    {
        BulletHoleDecalMaterial = DecalMat.Object;
        UE_LOG(LogTemp, Warning, TEXT("Bullet hole decal material loaded successfully"));
    }
    else
    {
        BulletHoleDecalMaterial = nullptr;
        UE_LOG(LogTemp, Error, TEXT("Failed to load bullet hole decal material!"));
    }
}

void AProjectiles::BeginPlay()
{
    Super::BeginPlay();

    /*UE_LOG(LogTemp, Warning, TEXT("BeginPlay called on Projectile"));

    if (DustParticles)
    {
        UE_LOG(LogTemp, Warning, TEXT("Spawning smoke emitter attached to projectile"));

        ActiveDustEffect = UGameplayStatics::SpawnEmitterAttached(
            DustParticles,
            RootComponent,
            NAME_None,
            FVector::ZeroVector,
            GetActorRotation(),
            EAttachLocation::KeepRelativeOffset,
            false // manual control over destruction
        );

        if (ActiveDustEffect)
        {
            UE_LOG(LogTemp, Warning, TEXT("Smoke emitter spawned successfully"));

            // Set velocity parameter if your particle system supports it
            FVector Velocity = ProjectileMovement->Velocity * 0.1f;
            ActiveDustEffect->SetVectorParameter(FName("Velocity"), Velocity);

            // Set scale
            ActiveDustEffect->SetWorldScale3D(FVector(1.f));

            // Timer to stop particle system after 1.5 seconds
            FTimerHandle DustTimerHandle;
            GetWorldTimerManager().SetTimer(DustTimerHandle, [this]()
                {
                    if (ActiveDustEffect)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Deactivating smoke emitter"));
                        ActiveDustEffect->DeactivateSystem();
                        ActiveDustEffect->DestroyComponent();
                        ActiveDustEffect = nullptr;
                    }
                }, 1.5f, false);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn smoke emitter!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DustParticles pointer is nullptr at BeginPlay"));
    }*/
}

void AProjectiles::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AProjectiles::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this)
    {
        UE_LOG(LogTemp, Warning, TEXT("Projectile hit actor: %s"), *OtherActor->GetName());

        // 🎯 Obține damage-ul dinamic din HUD bazat pe arma curentă
        float DynamicDamage = 30.f; // Default fallback
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            ACPP_Shooter2HUD* MyHUD = Cast<ACPP_Shooter2HUD>(PC->GetHUD());
            if (MyHUD)
            {
                DynamicDamage = static_cast<float>(MyHUD->GetCurrentWeaponDamage());
                UE_LOG(LogTemp, Warning, TEXT("🔥 Damage dinamic obținut: %.1f"), DynamicDamage);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("⚠️ Nu am putut accesa HUD-ul, folosesc damage default: %.1f"), DynamicDamage);
            }
        }

        // 🤖 Verifică dacă e Enemy_Bot
        if (AEnemy_Bot* Enemy = Cast<AEnemy_Bot>(OtherActor))
        {
            Enemy->TakeDamage(DynamicDamage);
            UE_LOG(LogTemp, Warning, TEXT("🔫 Damage aplicat Enemy Bot: %.1f"), DynamicDamage);
        }
        else if (AEnemyRangedBot* Enemy2 = Cast<AEnemyRangedBot>(OtherActor))
        {
            Enemy2->TakeDamage(DynamicDamage);
            UE_LOG(LogTemp, Warning, TEXT("🎯 Damage aplicat Ranged Bot: %.1f"), DynamicDamage);
        }
        else if (ABossEnemyBot* Enemy3 = Cast<ABossEnemyBot>(OtherActor))
        {
            Enemy3->TakeDamage(DynamicDamage);
            UE_LOG(LogTemp, Warning, TEXT("💀 Damage aplicat BOSS: %.1f"), DynamicDamage);
        }
        else
        {
            APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
            if (OtherActor == PlayerPawn)
            {
                UE_LOG(LogTemp, Warning, TEXT("🎯 Jucătorul A FOST LOVIT de un glonț!"));
                ACPP_Shooter2HUD* MyHUD = Cast<ACPP_Shooter2HUD>(PC->GetHUD());
                if (MyHUD)
                {
                    // Exemplu: scade viața
                    MyHUD->CurrentHealth -= 10.0f;
                    if (MyHUD->CurrentHealth < 0.f)
                        MyHUD->CurrentHealth = 0.f;
                }
            }
        }

        // 🎨 Spawn bullet hole decal
        if (BulletHoleDecalMaterial)
        {
            FVector DecalSize = FVector(5.f, 5.f, 5.f);
            UGameplayStatics::SpawnDecalAtLocation(
                GetWorld(),
                BulletHoleDecalMaterial,
                DecalSize,
                Hit.ImpactPoint,
                Hit.ImpactNormal.Rotation(),
                10.f // durata decalului (0 = permanent)
            );
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("BulletHoleDecalMaterial este nullptr!"));
        }

        // 💥 Impuls fizic
        float ExplosionRadius = 100.f;
        float ExplosionStrength = 7500.f;
        TArray<FOverlapResult> Overlaps;
        FCollisionShape Sphere = FCollisionShape::MakeSphere(ExplosionRadius);
        bool bHasOverlaps = GetWorld()->OverlapMultiByChannel(
            Overlaps,
            GetActorLocation(),
            FQuat::Identity,
            ECC_PhysicsBody,
            Sphere);

        if (bHasOverlaps)
        {
            for (auto& Result : Overlaps)
            {
                UPrimitiveComponent* PrimComp = Result.GetComponent();
                if (PrimComp && PrimComp->IsSimulatingPhysics())
                {
                    PrimComp->AddRadialImpulse(GetActorLocation(), ExplosionRadius, ExplosionStrength, ERadialImpulseFalloff::RIF_Linear, true);
                }
            }
        }

        // 🗑️ Cleanup projectil
        ProjectileMovement->StopMovementImmediately();
        CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        MeshComp->SetVisibility(false);
        SetLifeSpan(3.f);
    }
}