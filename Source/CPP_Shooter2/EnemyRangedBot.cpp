#include "EnemyRangedBot.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnemyBotAIController.h"
#include "Projectiles.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "CPP_Shooter2HUD.h"
#include "CPP_Shooter2GameMode.h"



// Sets default values
AEnemyRangedBot::AEnemyRangedBot()
{
	CurrentHealth = MaxHealth;
	PrimaryActorTick.bCanEverTick = true;
	TimeSinceLastShot = 0.f;
	FireInterval = 1.5f;
	// Încarcă mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BotMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/StarterContent/source/Bot_walking_correct.Bot_walking_correct'"));
	if (BotMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(BotMesh.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));

		UE_LOG(LogTemp, Warning, TEXT("✅ Mesh încărcat cu succes."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Eroare: Nu s-a putut încărca mesh-ul botului!"));
	}

	// Încarcă animația de mers
	static ConstructorHelpers::FObjectFinder<UAnimSequence> WalkAnimAsset(
		TEXT("/Script/Engine.AnimSequence'/Game/StarterContent/source/Bot_walking_correct_Anim.Bot_walking_correct_Anim'"));
	if (WalkAnimAsset.Succeeded())
	{
		WalkAnim = WalkAnimAsset.Object;
		UE_LOG(LogTemp, Warning, TEXT("✅ WalkAnim încărcat."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Nu s-a putut încărca WalkAnim."));
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> AttackAnimAsset(
		TEXT("/Script/Engine.AnimSequence'/Game/StarterContent/source/Shooting_Anim_mixamo_com.Shooting_Anim_mixamo_com'"));
	if (AttackAnimAsset.Succeeded())
	{
		AttackAnim = AttackAnimAsset.Object;
		UE_LOG(LogTemp, Warning, TEXT("✅ Attack anim încărcat."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Nu s-a putut încărca Attack Anim."));
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> DieAnimAsset(
		TEXT("/Script/Engine.AnimSequence'/Game/StarterContent/source/Dying_Anim_mixamo_com.Dying_Anim_mixamo_com'"));
	if (DieAnimAsset.Succeeded())
	{
		DieAnim = DieAnimAsset.Object;
		UE_LOG(LogTemp, Warning, TEXT("✅ Die anim încărcat."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Nu s-a putut încărca Die Anim."));
	}
	//sound

	static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundObj(TEXT("/Game/StarterContent/Audio/Explosion_Cue.Explosion_Cue"));

	if (FireSoundObj.Succeeded())
	{
		FireSound = FireSoundObj.Object;
		UE_LOG(LogTemp, Warning, TEXT("Fire sound loaded successfully!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load fire sound!"));
	}

	bIsPlayingWalkAnim = false;
	AnimationTimer = 0.f;
	AIControllerClass = AEnemyBotAIController::StaticClass();  // leagă AIController-ul
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;     // automat să ia controlul botului
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));

	PistolMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PistolMesh"));

	// Încarcă mesh-ul sabiei
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PistolAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/Pistol.Pistol'"));
	if (PistolAsset.Succeeded())
	{
		PistolMesh->SetStaticMesh(PistolAsset.Object);
		PistolMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // nu vrei coliziune pe sabie în idle

		// Atașează sabia la socketul "hand_r" din SkeletalMesh-ul botului
		PistolMesh->SetupAttachment(GetMesh(), FName("RightHand"));

		// Opțional: ajustări de poziție/rotație relative
		PistolMesh->SetRelativeLocation(FVector(0.f, -20.f, 0.f));
		PistolMesh->SetRelativeRotation(FRotator(0.f, 90.f, 270.f));
		PistolMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f)); // 1.0 / 5.0 = 0.2
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Nu s-a putut încărca mesh-ul sabiei!"));
	}
}

void AEnemyRangedBot::BeginPlay()
{
	Super::BeginPlay();
	//GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
	LastLocation = GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("🔄 BeginPlay din EnemyBot apelat"));

	if (Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller-ul este: %s"), *Controller->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Botul NU are controller la BeginPlay!"));
	}

	if (WalkAnim)
	{
		GetMesh()->PlayAnimation(WalkAnim, true); // ✅ loop=true, NU mai reporni in Tick
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
		UE_LOG(LogTemp, Warning, TEXT("▶️ Animatia de mers a fost pornita o singura data (loop)."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ WalkAnim e nullptr în BeginPlay!"));
	}
}


void AEnemyRangedBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsDead == true) {
		return;
	}
	if (!WalkAnim || !GetMesh()) return;
	//if (bIsPausedAfterStop) return; // 🔒 dacă e în pauză, nu mai face nimic
	//GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;
	FVector BotLocation = GetActorLocation();
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector Direction = (PlayerLocation - BotLocation);
	Direction.Z = 0; // ignoră diferența de înălțime

	// 2. Verifici dacă direcția e validă
	if (!Direction.IsNearlyZero())
	{
		FRotator TargetRotation = Direction.Rotation();

		// 3. Interpolezi între rotația actuală și țintă
		FRotator CurrentRotation = GetActorRotation();
		float RotationSpeed = 2.5f; // ajustează cât de rapid se întoarce
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);

		// 4. Setezi doar yaw-ul
		NewRotation.Pitch = 0.f;
		NewRotation.Roll = 0.f;

		SetActorRotation(NewRotation);
	}
	float MovementDelta = FVector::Dist(GetActorLocation(), LastLocation);

	if (MovementDelta < 1.f)
	{
		//GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		if (!bIsPausedAfterStop)
		{
			bIsPausedAfterStop = true;
			GetMesh()->bPauseAnims = false; // Asigură-te că animațiile NU sunt în pauză

			if (AttackAnim)
			{
				GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
				GetMesh()->PlayAnimation(AttackAnim, false); // 🔁 false: o singură dată
				UE_LOG(LogTemp, Warning, TEXT("🗡️ Atac anim pornit (bot sta pe loc)."));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("❌ AttackAnim e nullptr, nu se poate reda."));
			}

			// Setează timer pentru reluare
			GetWorld()->GetTimerManager().SetTimer(PauseTimerHandle, this, &AEnemyRangedBot::ResumeMovement, 1.5f, false);
		}
	}
	else
	{
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
		GetMesh()->bPauseAnims = false;
	}

	LastLocation = GetActorLocation();
	TimeSinceLastShot += DeltaTime;

	if (TimeSinceLastShot >= FireInterval)
	{
		ShootProjectile();
		TimeSinceLastShot = 0.f;
	}
	// 🔄 Mișcă spre player
}






void AEnemyRangedBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyRangedBot::TakeDamage(float DamageAmount)
{
	if (CurrentHealth <= 0.f)
	{
		return; // Deja mort
	}

	CurrentHealth -= DamageAmount;

	UE_LOG(LogTemp, Warning, TEXT("💥 Enemy Bot a primit damage: %.2f. HP ramas: %.2f"), DamageAmount, CurrentHealth);

	if (CurrentHealth <= 0.f)
	{
		Die();
	}
}

void AEnemyRangedBot::Die()
{
	bIsDead = true;
	UE_LOG(LogTemp, Warning, TEXT("☠️ Enemy Bot a murit!"));

	// Dezactivează coliziunea și mișcarea
	SetActorEnableCollision(false);
	GetMesh()->Stop();

	// Joacă animația de moarte dacă există
	if (DieAnim)
	{
		GetMesh()->PlayAnimation(DieAnim, false); // 🔁 false = doar o dată
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		UE_LOG(LogTemp, Warning, TEXT("🎬 Animatia de moarte a fost redată."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ DieAnim e nullptr, nu se poate reda."));
	}

	// Oprește tick-ul
	SetActorTickEnabled(false);
	ACPP_Shooter2GameMode* GM = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("📏 Distanta pana la player la moarte: %.2f metri"), DistanceToPlayer / 100.f);
		float distance2 = GM->GetMaxKillDistance();
		if (distance2 < DistanceToPlayer/100.f) {
			GM->SetMaxKillDistance(DistanceToPlayer / 100.f);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Nu s-a putut găsi PlayerPawn pentru calcularea distanței!"));
	}
	if (GM)
	{
		int32 NewCoins = GM->GetCurrentCoins() + 15;
		GM->SetCurrentCoins(NewCoins);
		int32 TotalKills = GM->GetTotalKills();
		GM->SetTotalKills(TotalKills + 1);
		// Actualizează HUD-ul să afișeze noile monede
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			ACPP_Shooter2HUD* HUD = Cast<ACPP_Shooter2HUD>(PC->GetHUD());
			if (HUD)
			{
				HUD->UpdateHUD(); // Sau poți face o funcție specifică să actualizeze doar coins
			}
		}
	}
	ACPP_Shooter2GameMode* GM2 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
	if (GM2)
	{
		GM2->OnBotKilled();
		GM2->ActiveBotCount--;
	}
	// Distrugere după un delay (suficient cât să se termine animatia)
	FTimerHandle DeathTimer;
	GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &AEnemyRangedBot::DestroyBot, 2.5f, false);
}
void AEnemyRangedBot::ResumeMovement()
{
	if (bIsDead == true)
	{
		return;
	}
	bIsPausedAfterStop = false;
	float MovementDelta = FVector::Dist(GetActorLocation(), LastLocation);

	if (MovementDelta < 1.f)
	{
		//GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		if (!bIsPausedAfterStop)
		{
			bIsPausedAfterStop = true;
			GetMesh()->bPauseAnims = false; // Asigură-te că animațiile NU sunt în pauză

			if (AttackAnim)
			{
				GetMesh()->PlayAnimation(AttackAnim, false); // 🔁 false: o singură dată
				GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
				UE_LOG(LogTemp, Warning, TEXT("🗡️ Atac anim pornit (bot sta pe loc)."));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("❌ AttackAnim e nullptr, nu se poate reda."));
			}

			// Setează timer pentru reluare
			GetWorld()->GetTimerManager().SetTimer(PauseTimerHandle, this, &AEnemyRangedBot::ResumeMovement, 1.5f, false);
		}

		return;
	}
	else
	{
		GetMesh()->PlayAnimation(WalkAnim, true); // true = loop
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
		GetMesh()->bPauseAnims = false;
	}

	UE_LOG(LogTemp, Warning, TEXT("⏱️ Botul reia urmărirea jucătorului."));
}
void AEnemyRangedBot::ShootProjectile()
{
	if (bIsDead) return;

	APlayerController* PC2 = UGameplayStatics::GetPlayerController(this, 0);
	int32 VolumeLevel = 10;
	if (PC2)
	{
		ACPP_Shooter2HUD* HUD2 = Cast<ACPP_Shooter2HUD>(PC2->GetHUD());
		if (HUD2)
		{
			VolumeLevel = HUD2->VolumeLevel;
		}
	}
	if (FireSound)
	{
		float CurrentVolume = VolumeLevel / 10.0f;
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation(),CurrentVolume);
	}
	UE_LOG(LogTemp, Warning, TEXT("Apeleaza shootprojectile"));
	TSubclassOf<AProjectiles> ProjectileClass = AProjectiles::StaticClass();
	if (!ProjectileClass) return;

	// 📍 Poziția de spawn a glonțului
	FVector MuzzleLocation = PistolMesh->GetComponentLocation() + PistolMesh->GetForwardVector() * 70.f;

	// 🎯 Obține referința la player
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	// 📐 Direcția spre player
	FVector DirectionToPlayer = (PlayerPawn->GetActorLocation() - MuzzleLocation).GetSafeNormal();
	FRotator MuzzleRotation = DirectionToPlayer.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// 🚀 Spawnează proiectilul orientat spre player
	AProjectiles* Projectile = GetWorld()->SpawnActor<AProjectiles>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
	if (Projectile)
	{
		if (Projectile->CollisionComp)
		{
			Projectile->CollisionComp->IgnoreActorWhenMoving(this, true);
		}

		// 🧭 Setează direcția de mișcare a proiectilului spre player
		FVector ShootDirection = DirectionToPlayer;
		Projectile->ProjectileMovement->Velocity = ShootDirection * Projectile->ProjectileMovement->InitialSpeed;

		UE_LOG(LogTemp, Warning, TEXT("🔫 Botul a tras direct spre player!"));
	}
}




void AEnemyRangedBot::DestroyBot()
{
	Destroy();
}
