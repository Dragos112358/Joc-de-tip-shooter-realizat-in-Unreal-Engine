#include "BossEnemyBot.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnemyBotAIController.h"
#include "CPP_Shooter2HUD.h"
#include "CPP_Shooter2GameMode.h"

// Sets default values
ABossEnemyBot::ABossEnemyBot()
{
	CurrentHealth = MaxHealth;
	PrimaryActorTick.bCanEverTick = true;

	// Încarcă mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BotMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/StarterContent/source/Bot_walking_correct.Bot_walking_correct'"));
	if (BotMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(BotMesh.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f)); // Dublat offsetul pentru Z din cauza scale-ului
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
		GetMesh()->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f)); // Dublează mărimea botului

		UE_LOG(LogTemp, Warning, TEXT("✅ Boss Mesh încărcat cu succes și mărit la scale 2x."));
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
		TEXT("/Script/Engine.AnimSequence'/Game/StarterContent/source/Standing_Melee_Attack_Anim_mixamo_com.Standing_Melee_Attack_Anim_mixamo_com'"));
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

	bIsPlayingWalkAnim = false;
	AnimationTimer = 0.f;
	AIControllerClass = AEnemyBotAIController::StaticClass();  // leagă AIController-ul
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;     // automat să ia controlul botului
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));

	// Încarcă mesh-ul sabiei
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SwordAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/sword.sword'"));
	if (SwordAsset.Succeeded())
	{
		SwordMesh->SetStaticMesh(SwordAsset.Object);
		SwordMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // nu vrei coliziune pe sabie în idle

		// Atașează sabia la socketul "hand_r" din SkeletalMesh-ul botului
		SwordMesh->SetupAttachment(GetMesh(), FName("RightHand"));

		// Ajustări pentru boss mai mare - pozițiile și scale-ul sunt proporționale
		SwordMesh->SetRelativeLocation(FVector(10.f, -40.f, 0.f)); // Păstrează aceleași coordonate relative
		SwordMesh->SetRelativeRotation(FRotator(0.f, 90.f, 90.f));
		SwordMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f)); // Dublează scale-ul sabiei (0.3f * 2 = 0.6f)
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Nu s-a putut încărca mesh-ul sabiei!"));
	}
}

void ABossEnemyBot::BeginPlay()
{
	Super::BeginPlay();
	//GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
	LastLocation = GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("🔄 BeginPlay din BossEnemyBot apelat"));

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


void ABossEnemyBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsDead) return;
	if (!WalkAnim || !GetMesh()) return;
	if (bIsPausedAfterStop) return; // 🔒 dacă e în pauză, nu mai face nimic
	//GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

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
			GetWorld()->GetTimerManager().SetTimer(PauseTimerHandle, this, &ABossEnemyBot::ResumeMovement, 1.5f, false);
		}
		return;
	}
	else
	{
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
		GetMesh()->bPauseAnims = false;
	}

	LastLocation = GetActorLocation();

	// 🔄 Mișcă spre player
}






void ABossEnemyBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABossEnemyBot::TakeDamage(float DamageAmount)
{
	if (CurrentHealth <= 0.f)
	{
		return; // Deja mort
	}

	CurrentHealth -= DamageAmount;

	UE_LOG(LogTemp, Warning, TEXT("💥 Boss Enemy Bot a primit damage: %.2f. HP ramas: %.2f"), DamageAmount, CurrentHealth);

	if (CurrentHealth <= 0.f)
	{
		Die();
	}
}

void ABossEnemyBot::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("☠️ Boss Enemy Bot a murit!"));
	bIsDead = true;
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

	// Calculează distanța până la player când moare
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("📏 Distanta pana la player la moarte: %.2f metri"), DistanceToPlayer / 100.f);

		if (GM)
		{
			float distance2 = GM->GetMaxKillDistance();
			if (distance2 < DistanceToPlayer / 100.f) {
				GM->SetMaxKillDistance(DistanceToPlayer / 100.f);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Nu s-a putut găsi PlayerPawn pentru calcularea distanței!"));
	}

	if (GM)
	{
		int32 NewCoins = GM->GetCurrentCoins() + 75;
		GM->SetCurrentCoins(NewCoins);

		//aici maresc nr de boti omorati
		int32 BossesKilled = GM->GetBossesKilled();
		GM->SetBossesKilled(BossesKilled + 1);
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
	GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ABossEnemyBot::DestroyBot, 2.5f, false);
}

void ABossEnemyBot::ResumeMovement()
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
				GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
				GetMesh()->PlayAnimation(AttackAnim, false); // 🔁 false: o singură dată

				UE_LOG(LogTemp, Warning, TEXT("🗡️ Atac anim pornit (bot sta pe loc)."));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("❌ AttackAnim e nullptr, nu se poate reda."));
			}

			// Setează timer pentru reluare
			GetWorld()->GetTimerManager().SetTimer(PauseTimerHandle, this, &ABossEnemyBot::ResumeMovement, 1.5f, false);
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


void ABossEnemyBot::DestroyBot()
{
	Destroy();
}