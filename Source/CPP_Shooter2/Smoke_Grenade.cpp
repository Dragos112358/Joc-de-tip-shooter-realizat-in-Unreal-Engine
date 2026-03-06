#include "Smoke_Grenade.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"

ASmoke_Grenade::ASmoke_Grenade()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComponent->InitSphereRadius(20.f);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->BodyInstance.bLockXRotation = true;
	CollisionComponent->BodyInstance.bLockYRotation = true;
	CollisionComponent->BodyInstance.bLockZRotation = true;


	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->BodyInstance.COMNudge = FVector(0.f, 0.f, 0.5f);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/smoke_gernade_05.smoke_gernade_05'"));
	if (MeshAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshAsset.Object);
		MeshComponent->SetWorldScale3D(FVector(0.5f));
		UE_LOG(LogTemp, Warning, TEXT("Smoke incarcat cu succes"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Nu am incarcat smoke"));
	}

	SmokeParticles = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), nullptr, TEXT("/Game/StarterContent/Particles/P_Smoke.P_Smoke")));
	if (SmokeParticles)
	{
		UE_LOG(LogTemp, Warning, TEXT("Particula de fum a fost încărcată cu succes"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Eroare la încărcarea particulei de fum"));
	}

	// Timpuri pentru fade
	FadeInTime = 2.0f;
	HoldTime = 4.0f;
	FadeOutTime = 4.0f;
}

void ASmoke_Grenade::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->SetEnableGravity(true);
	CollisionComponent->SetLinearDamping(0.1f);
	CollisionComponent->SetAngularDamping(0.1f);
	CollisionComponent->SetPhysicsLinearVelocity(GetActorForwardVector() * 2000.f);
	CollisionComponent->SetPhysicsAngularVelocityInDegrees(FVector(0.f, 0.f, 0.f));

	// Timer pentru activarea fumului după 3 secunde
	GetWorldTimerManager().SetTimer(SmokeTimerHandle, this, &ASmoke_Grenade::ActivateSmoke, 3.0f, false);
}

void ASmoke_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActiveSmokeEffect)
	{
		UpdateSmokeEffectScale(DeltaTime);
	}
}

void ASmoke_Grenade::UpdateSmokeEffectScale(float DeltaTime)
{
	EffectElapsedTime += DeltaTime;

	float NewScaleFactor = 1.f;
	const float MinScaleFactor = 0.35f;

	if (EffectElapsedTime <= FadeInTime)
	{
		NewScaleFactor = FMath::Lerp(0.f, 1.f, EffectElapsedTime / FadeInTime);
	}
	else if (EffectElapsedTime <= FadeInTime + HoldTime)
	{
		NewScaleFactor = 1.f;
	}
	else if (EffectElapsedTime <= FadeInTime + HoldTime + FadeOutTime)
	{
		float FadeOutElapsed = EffectElapsedTime - FadeInTime - HoldTime;
		NewScaleFactor = FMath::Lerp(1.f, MinScaleFactor, FadeOutElapsed / FadeOutTime);
	}
	else
	{
		if (ActiveSmokeEffect)
		{
			ActiveSmokeEffect->DeactivateSystem();
			ActiveSmokeEffect->DestroyComponent();
			ActiveSmokeEffect = nullptr;
		}
		Destroy();
		return;
	}

	ActiveSmokeEffect->SetWorldScale3D(FVector(NewScaleFactor * 15.f));
}

void ASmoke_Grenade::ActivateSmoke()
{
	if (SmokeParticles)
	{
		// Atașează efectul de fum la grenade pentru control complet
		ActiveSmokeEffect = UGameplayStatics::SpawnEmitterAttached(
			SmokeParticles,
			RootComponent,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			false // bAutoDestroy = false, control complet manual
		);

		if (ActiveSmokeEffect)
		{
			ActiveSmokeEffect->SetWorldScale3D(FVector(15.0f));
			EffectElapsedTime = 0.f;
			EffectDuration = FadeInTime + HoldTime + FadeOutTime;

			// Timer pentru distrugere completă
			GetWorldTimerManager().SetTimer(SmokeTimerHandle, this, &ASmoke_Grenade::DeactivateAndDestroy2, EffectDuration, false);
		}
		else
		{
			Destroy();
		}
	}
	else
	{
		Destroy();
	}
}

void ASmoke_Grenade::DeactivateAndDestroy2()
{
	if (ActiveSmokeEffect)
	{
		ActiveSmokeEffect->DeactivateSystem();
		ActiveSmokeEffect->DestroyComponent();
		ActiveSmokeEffect = nullptr;
	}

	Destroy();
}
