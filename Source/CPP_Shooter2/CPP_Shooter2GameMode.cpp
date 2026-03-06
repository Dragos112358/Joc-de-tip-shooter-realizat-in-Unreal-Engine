// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPP_Shooter2GameMode.h"
#include "CPP_Shooter2Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Enemy_Bot.h"
#include "EnemyRangedBot.h"
#include "BossEnemyBot.h"
#include "Bullet.h"
#include "DragRace.h"
#include <Kismet/GameplayStatics.h>
#include "Weapon.h"
#include "LiftActor.h"
#include "Energy.h"
#include "Car.h"
#include "MySaveGame.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.h"


void ACPP_Shooter2GameMode::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay din GameMode apelat!"));
	Super::BeginPlay();

	LoadPlayerData();
	WeaponClass = AWeapon::StaticClass();
	DragRaceClass = ADragRace::StaticClass();
	if (DragRaceClass) // DragRaceClass = TSubclassOf<ADragRace>, setat în editor sau constructor
	{
		FVector SpawnLocation(-30591.85f, -36000.f, -50.f); // Poți modifica poziția
		FRotator SpawnRotation = FRotator::ZeroRotator;

		ADragRace* DragRaceInstance = GetWorld()->SpawnActor<ADragRace>(DragRaceClass, SpawnLocation, SpawnRotation);
		if (DragRaceInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("DragRace spawned successfully!"));
			// aici poți, de exemplu, să îi adaugi mașinile participante
			// DragRaceInstance->RaceCars.Add(PlayerCar);
		}
	}


	SpawnBot();
	SpawnBoss();
	SpawnRangedBot();
	FVector SpawnLocation = FVector(1300, 1100, 125);
	FRotator SpawnRotation = FRotator::ZeroRotator;

	SpawnBullet(SpawnLocation, SpawnRotation, EBulletType::BT_556);
	FVector SpawnLocation2 = FVector(1000, 1100, 125);
	FRotator SpawnRotation2 = FRotator::ZeroRotator;

	SpawnBullet(SpawnLocation2, SpawnRotation2, EBulletType::BT_762);

	FVector SpawnLocation3 = FVector(800, 1100, 125);
	FRotator SpawnRotation3 = FRotator::ZeroRotator;

	SpawnBullet(SpawnLocation3, SpawnRotation3, EBulletType::BT_9MM);

	FVector WeaponLocation(600.f, 1100.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation(-0.f, 0.f, -90.f);

	SpawnWeapon(WeaponLocation, WeaponRotation,EWeaponType::AKM);

	FVector WeaponLocation2(300.f, 1100.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation2(-0.f, 0.f, -90.f);

	SpawnWeapon(WeaponLocation2, WeaponRotation2, EWeaponType::VSS);


	FVector WeaponLocation3(300.f, 900.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation3(-0.f, 0.f, -90.f);

	SpawnWeapon(WeaponLocation3, WeaponRotation3, EWeaponType::Scar);
	FVector WeaponLocation4(500.f, 900.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation4(-0.f, 0.f, -90.f);

	SpawnWeapon(WeaponLocation4, WeaponRotation4, EWeaponType::M416);

	FVector WeaponLocation5(700.f, 900.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation5(-0.f, 0.f, -90.f);

	SpawnWeapon(WeaponLocation5, WeaponRotation5, EWeaponType::M762);


	FVector WeaponLocation6(1100.f, 900.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation6(-0.f, 0.f, -90.f);

	SpawnWeapon(WeaponLocation6, WeaponRotation6, EWeaponType::M249);

	FVector WeaponLocation7(1300.f, 900.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation7(-0.f, 0.f, -90.f);

	SpawnWeapon(WeaponLocation7, WeaponRotation7, EWeaponType::AWM);

	FVector WeaponLocation8(1300.f, 600.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation8(-0.f, 0.f, -90.f);

	SpawnWeapon(WeaponLocation8, WeaponRotation8, EWeaponType::SKS);

	FVector WeaponLocation9(1300.f, 300.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation9(-90.f, -0.f, 0.f);

	SpawnWeapon(WeaponLocation9, WeaponRotation9, EWeaponType::Groza);

	FVector WeaponLocation10(1100.f, 300.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation10(0.f, 0.f, -90.f);

	SpawnWeapon(WeaponLocation10, WeaponRotation10, EWeaponType::KAR98K);

	FVector WeaponLocation11(900.f, 300.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation11(0.f, 0.f, -90.f);

	SpawnWeapon(WeaponLocation11, WeaponRotation11, EWeaponType::S686);
	
	FVector WeaponLocation12(700.f, 300.f, 110.f);

	// Pune Pitch la -90 pentru a culca arma pe podea
	FRotator WeaponRotation12(90.f, -90.f, 0.f);

	SpawnWeapon(WeaponLocation12, WeaponRotation12, EWeaponType::DP28);

	FVector SpawnLocation25 = FVector(0.f, 0.f, 300.f);
	FRotator SpawnRotation25 = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams25;
	SpawnParams25.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ALiftActor* Lift25 = GetWorld()->SpawnActor<ALiftActor>(ALiftActor::StaticClass(), SpawnLocation25, SpawnRotation25, SpawnParams25);

	if (Lift25)
	{
		Lift25->InitLift(3000.f); // setezi noua înălțime aici
	}


	FVector SpawnLocation26 = FVector(-6665, -4680.f, 1250.f);
	FRotator SpawnRotation26 = FRotator(0.f, 180.f, 0.f);

	FActorSpawnParameters SpawnParams26;
	SpawnParams26.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ALiftActor* Lift26 = GetWorld()->SpawnActor<ALiftActor>(ALiftActor::StaticClass(), SpawnLocation26, SpawnRotation26, SpawnParams26);

	if (Lift26)
	{
		Lift26->InitLift(14960.f); // setezi noua înălțime aici
	}

	FVector SpawnLocation100 = FVector(1600.f, 1200.f, 150.f);
	FRotator SpawnRotation100 = FRotator(0.f, 180.f, 0.f);

	FActorSpawnParameters SpawnParams100;
	SpawnParams100.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ACar* SpawnedCar = GetWorld()->SpawnActor<ACar>(ACar::StaticClass(), SpawnLocation100, SpawnRotation100, SpawnParams100);

	if (SpawnedCar)
	{
		// Poți face setări suplimentare dacă vrei
		// De exemplu, să setezi viteza inițială, sau altceva
	}






	FVector SpawnLocation27 = FVector(3225, -620.f, 250.f);
	FRotator SpawnRotation27 = FRotator(0.f, 0.f, 0.f);

	FActorSpawnParameters SpawnParams27;
	SpawnParams27.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ALiftActor* Lift27 = GetWorld()->SpawnActor<ALiftActor>(ALiftActor::StaticClass(), SpawnLocation27, SpawnRotation27, SpawnParams27);

	if (Lift27)
	{
		Lift27->InitLift(30000.f); // setezi noua înălțime aici
	}

	FVector SpawnLocation28 = FVector(3225, 5350, 250.f);
	FRotator SpawnRotation28 = FRotator(0.f, 0.f, 0.f);

	FActorSpawnParameters SpawnParams28;
	SpawnParams28.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ALiftActor* Lift28 = GetWorld()->SpawnActor<ALiftActor>(ALiftActor::StaticClass(), SpawnLocation28, SpawnRotation28, SpawnParams28);

	if (Lift28)
	{
		Lift28->InitLift(30000.f); // setezi noua înălțime aici
	}








	//Spawnare energy drink
	FVector SpawnLocation30 = FVector(1000.f, 1200.f, 125.f);  // Exemplu coordonate
	FRotator SpawnRotation30 = FRotator::ZeroRotator;       // fără rotație

	// Parametrii pentru spawn
	FActorSpawnParameters SpawnParams30;
	SpawnParams30.Owner = this;          // cine este "owner"-ul (opțional)
	SpawnParams30.Instigator = GetInstigator();  // cine l-a creat (opțional)
	SpawnParams30.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn actorul
	AEnergy* SpawnedEnergy = GetWorld()->SpawnActor<AEnergy>(AEnergy::StaticClass(), SpawnLocation30, SpawnRotation30, SpawnParams30);

	if (SpawnedEnergy)
	{
		// Poți să setezi aici variabile dacă ai nevoie, ex:
		SpawnedEnergy->InitEnergy(EEnergyType::E_EnergyDrink);
	}


	FVector SpawnLocation31 = FVector(1200.f, 1200.f, 125.f);  // Exemplu coordonate
	FRotator SpawnRotation31 = FRotator::ZeroRotator;       // fără rotație

	// Parametrii pentru spawn
	FActorSpawnParameters SpawnParams31;
	SpawnParams31.Owner = this;          // cine este "owner"-ul (opțional)
	SpawnParams31.Instigator = GetInstigator();  // cine l-a creat (opțional)
	SpawnParams31.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn actorul
	AEnergy* SpawnedEnergy2 = GetWorld()->SpawnActor<AEnergy>(AEnergy::StaticClass(), SpawnLocation31, SpawnRotation31, SpawnParams31);

	if (SpawnedEnergy2)
	{
		// Poți să setezi aici variabile dacă ai nevoie, ex:
		SpawnedEnergy2->InitEnergy(EEnergyType::E_Painkiller);
	}



	FVector SpawnLocation32 = FVector(1400.f, 1200.f, 125.f);  // Exemplu coordonate
	FRotator SpawnRotation32 = FRotator(0.f, 0.f, 90.f);

	// Parametrii pentru spawn
	FActorSpawnParameters SpawnParams32;
	SpawnParams32.Owner = this;          // cine este "owner"-ul (opțional)
	SpawnParams32.Instigator = GetInstigator();  // cine l-a creat (opțional)
	SpawnParams32.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn actorul
	AEnergy* SpawnedEnergy3 = GetWorld()->SpawnActor<AEnergy>(AEnergy::StaticClass(), SpawnLocation32, SpawnRotation32, SpawnParams32);

	if (SpawnedEnergy3)
	{
		// Poți să setezi aici variabile dacă ai nevoie, ex:
		SpawnedEnergy3->InitEnergy(EEnergyType::E_AdrenalineSyringe);
	}
	//in begin play, pun si meniul:
	APlayerController* PC15 = GetWorld()->GetFirstPlayerController();
	/*if (PC15)
	{
		// Creează widget-ul UMenuWidget folosind clasa C++ direct
		UMenuWidget* Menu = CreateWidget<UMenuWidget>(PC15, UMenuWidget::StaticClass());
		if (Menu)
		{
			Menu->AddToViewport();

			// Setează inputul să fie doar pe UI și arată cursorul mouse-ului
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(Menu->TakeWidget());
			PC15->SetInputMode(InputMode);
			PC15->bShowMouseCursor = true;
		}
	}*/
}

void ACPP_Shooter2GameMode::LoadPlayerData()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
	{
		UMySaveGame* LoadedGame = Cast<UMySaveGame>(
			UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0)
		);
		if (LoadedGame)
		{
			CurrentCoins = LoadedGame->Coins;
			CurrentUnlockedWeapons = LoadedGame->UnlockedWeapons;

			// Încarcă upgrade-urile mașinii
			CurrentEngineLevel = LoadedGame->EngineLevel;
			CurrentTurboLevel = LoadedGame->TurboLevel;
			CurrentWeightLevel = LoadedGame->WeightLevel;
			CurrentHealthLevel = LoadedGame->HealthLevel;
			OwnedWeaponNames = LoadedGame->OwnedWeapon;
			WeaponName1 = LoadedGame->WeaponName1;
			WeaponName2 = LoadedGame->WeaponName2;
			Grenades = LoadedGame->Grenades;
			Molotovs = LoadedGame->Molotovs;
			SmokeGrenades = LoadedGame->SmokeGrenades;
			bullet556 = LoadedGame->bullet556;
			bullet762 = LoadedGame->bullet762;
			bullet9 = LoadedGame->bullet9;
			AchievementStates = LoadedGame->AchievementStates;
			TopSpeed = LoadedGame->TopSpeed;
			CarDistance = LoadedGame->CarDistance;
			TotalKills = LoadedGame->TotalKills;
			Games = LoadedGame->Games;
			MaxKillDistance = LoadedGame->MaxKillDistance;
			TotalGrenades = LoadedGame->TotalGrenades;

			PlayerDistance = LoadedGame->PlayerDistance;
			NumberDies = LoadedGame->NumberDies;

			BossesKilled = LoadedGame->BossesKilled;

			UE_LOG(LogTemp, Log, TEXT("Progres incarcat: %d banuti, %d arme"),
				CurrentCoins, CurrentUnlockedWeapons.Num());
			UE_LOG(LogTemp, Log, TEXT("Upgrade-uri masina: Engine=%d, Turbo=%d, Weight=%d"),
				CurrentEngineLevel, CurrentTurboLevel, CurrentWeightLevel);
			UE_LOG(LogTemp, Log, TEXT("TopSpeed = %f"), TopSpeed);

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Nu exista niciun fisier de salvare!"));
		// Setează valorile default pentru upgrade-uri
		CurrentEngineLevel = 1;
		CurrentTurboLevel = 1;
		CurrentWeightLevel = 1;
		CurrentHealthLevel = 1;
		WeaponName1 = "AKM";
		WeaponName2 = "M762";
		Grenades = 0;
		Molotovs = 0;
		SmokeGrenades = 0;
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (ACPP_Shooter2HUD* HUD = Cast<ACPP_Shooter2HUD>(PC->GetHUD()))
		{
			HUD->UpdateHUD();
		}
	}
}
void ACPP_Shooter2GameMode::SavePlayerData()
{
	// Creează un obiect de tip SaveGame
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(
		UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass())
	);

	// Setează valorile din progres
	SaveGameInstance->Coins = CurrentCoins;
	SaveGameInstance->UnlockedWeapons = CurrentUnlockedWeapons;

	// Salvează upgrade-urile mașinii
	SaveGameInstance->EngineLevel = CurrentEngineLevel;
	SaveGameInstance->TurboLevel = CurrentTurboLevel;
	SaveGameInstance->WeightLevel = CurrentWeightLevel;
	SaveGameInstance->HealthLevel = CurrentHealthLevel;

	SaveGameInstance->OwnedWeapon = OwnedWeaponNames;
	SaveGameInstance->WeaponName1 = WeaponName1;
	SaveGameInstance->WeaponName2 = WeaponName2;

	SaveGameInstance->Grenades = Grenades;
	SaveGameInstance->Molotovs = Molotovs;
	SaveGameInstance->SmokeGrenades = SmokeGrenades;


	SaveGameInstance->bullet556 = bullet556;
	SaveGameInstance->bullet762 = bullet762;
	SaveGameInstance->bullet9 = bullet9;

	//si pentru achievementuri
	SaveGameInstance->AchievementStates = AchievementStates;

	SaveGameInstance->TopSpeed = TopSpeed;

	SaveGameInstance->CarDistance = CarDistance;

	SaveGameInstance->TotalKills = TotalKills;

	SaveGameInstance->Games = Games + 1;

	SaveGameInstance->MaxKillDistance = MaxKillDistance;

	SaveGameInstance->TotalGrenades = TotalGrenades;
	
	SaveGameInstance->PlayerDistance = PlayerDistance;

	SaveGameInstance->NumberDies = NumberDies;

	SaveGameInstance->BossesKilled = BossesKilled;

	for (int32 i = 0; i < SaveGameInstance->AchievementStates.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Achievement %d = %d"), i, SaveGameInstance->AchievementStates[i]);
	}





	// Salvează în slot
	if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerSaveSlot"), 0))
	{
		UE_LOG(LogTemp, Log, TEXT("Progres salvat cu succes!"));
		FString AllWeapons = FString::Join(OwnedWeaponNames, TEXT(", "));
		UE_LOG(LogTemp, Warning, TEXT("Owned weapons: %s"), *AllWeapons);
		UE_LOG(LogTemp, Log, TEXT("Bosses Killed = %d"), BossesKilled);
		UE_LOG(LogTemp, Log, TEXT("NumberDies = %d"), NumberDies);
		UE_LOG(LogTemp, Log, TEXT("PlayerDistance = %f"), PlayerDistance);
		UE_LOG(LogTemp, Log, TEXT("TotalGrenades = %d"), TotalGrenades);
		UE_LOG(LogTemp, Log, TEXT("Kill distance = %f"), MaxKillDistance);
		UE_LOG(LogTemp, Log, TEXT("Games = %d"), Games+1);
		UE_LOG(LogTemp, Log, TEXT("TotalKills = %d"), TotalKills);
		UE_LOG(LogTemp, Log, TEXT("Distance with car = %f"), CarDistance);
		UE_LOG(LogTemp, Log, TEXT("TopSpeed = %f"), TopSpeed);
		UE_LOG(LogTemp, Log, TEXT("Upgrade-uri salvate: Engine=%d, Turbo=%d, Weight=%d"),
			CurrentEngineLevel, CurrentTurboLevel, CurrentWeightLevel);
		UE_LOG(LogTemp, Log, TEXT("WeaponName1: %s"), *WeaponName1);
		UE_LOG(LogTemp, Log, TEXT("WeaponName2: %s"), *WeaponName2);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Eroare la salvarea progresului!"));
	}
}
ACPP_Shooter2GameMode::ACPP_Shooter2GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	HUDClass = ACPP_Shooter2HUD::StaticClass();
	BotPawnClass = AEnemy_Bot::StaticClass();
	RangedBotPawnClass = AEnemyRangedBot::StaticClass();
	BulletClass = ABullet::StaticClass();
	BossEnemyBotClass = ABossEnemyBot::StaticClass();
	UE_LOG(LogTemp, Warning, TEXT("BotPawnClass set to AEnemy_Bot::StaticClass()"));
}
void ACPP_Shooter2GameMode::SpawnBot()
{
	if (BotPawnClass)
	{
		FVector SpawnLocation(1300.f, 1000.f, 200.f);
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		APawn* SpawnedBot = GetWorld()->SpawnActor<APawn>(BotPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (SpawnedBot)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bot spawned!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn bot!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BotPawnClass is nullptr!"));
	
	}
}

void ACPP_Shooter2GameMode::SpawnRangedBot()
{
	if (RangedBotPawnClass)
	{
		FVector SpawnLocation(1000.f, 1000.f, 200.f);
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		APawn* SpawnedBot2 = GetWorld()->SpawnActor<APawn>(RangedBotPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (SpawnedBot2)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ranged Bot spawned!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn ranged bot!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BotPawnClass is nullptr!"));

	}
}
void ACPP_Shooter2GameMode::SpawnBullet(const FVector& SpawnLocation, const FRotator& SpawnRotation, EBulletType BulletType)
{
	if (BulletClass)
	{
		FTransform SpawnTransform(SpawnRotation, SpawnLocation);

		ABullet* SpawnedBullet = GetWorld()->SpawnActorDeferred<ABullet>(BulletClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		if (SpawnedBullet)
		{
			SpawnedBullet->BulletType = BulletType;  // SETEAZĂ TIPUL ÎNAINTE DE BeginPlay

			UGameplayStatics::FinishSpawningActor(SpawnedBullet, SpawnTransform);

			UE_LOG(LogTemp, Warning, TEXT("Bullet spawned! Type: %d"), static_cast<int32>(BulletType));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn bullet!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BulletClass is nullptr!"));
	}
}
void ACPP_Shooter2GameMode::SpawnWeapon(const FVector& SpawnLocation, const FRotator& SpawnRotation, EWeaponType WeaponType)
{
	if (WeaponClass)
	{
		FTransform SpawnTransform(SpawnRotation, SpawnLocation);

		AWeapon* SpawnedWeapon = GetWorld()->SpawnActorDeferred<AWeapon>(
			WeaponClass,
			SpawnTransform,
			this,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		);

		if (SpawnedWeapon)
		{
			// Setezi tipul armei primit ca parametru
			SpawnedWeapon->InitializeWeapon(WeaponType);

			UGameplayStatics::FinishSpawningActor(SpawnedWeapon, SpawnTransform);

			UE_LOG(LogTemp, Warning, TEXT("Weapon spawned! Type: %d"), static_cast<int32>(WeaponType));
		}
	}
}
void ACPP_Shooter2GameMode::SpawnBoss()
{
	if (BossEnemyBotClass)
	{
		FVector SpawnLocation(1500.f, 800.f, 400.f); // Poziție diferită pentru boss
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABossEnemyBot* SpawnedBoss = GetWorld()->SpawnActor<ABossEnemyBot>(BossEnemyBotClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (SpawnedBoss)
		{
			// Poți seta propriețăți specifice boss-ului aici
			// SpawnedBoss->MaxHealth = 300.f; // Dacă vrei boss-ul să aibă mai multă viață

			ActiveBotCount++; // Incrementează contorul de boți activi
			UE_LOG(LogTemp, Warning, TEXT("🔥 Boss Enemy Bot spawned successfully!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Failed to spawn Boss Enemy Bot!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ BossEnemyBotClass is nullptr!"));
	}
}

void ACPP_Shooter2GameMode::OnBotKilled()
{
	BotsKilledCount++;
	UE_LOG(LogTemp, Warning, TEXT("Bot mort! Total morti: %d"), BotsKilledCount);
	if (ActiveBotCount == 0)
	{
		SpawnNextWaveBots();
		BotsKilledCount = 0; // resetează contorul pentru următorul val
	}
}

void ACPP_Shooter2GameMode::SpawnNextWaveBots()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn urmatorii boți!"));

	if (ActiveBotCount >= MaxActiveBots)
	{
		UE_LOG(LogTemp, Warning, TEXT("Nu se mai pot spawna boți, limita de %d a fost atinsă."), MaxActiveBots);
		return;
	}

	// === BOȚI NORMALI ===
	int32 NumNormalBots = 3;
	ActiveBotCount += NumNormalBots;

	for (int i = 0; i < NumNormalBots; ++i)
	{
		float RandX = FMath::RandRange(-1300.f, 1300.f);
		float RandY = FMath::RandRange(-1300.f, 1300.f);
		FVector SpawnLocation(RandX, RandY, 200.f);
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		APawn* SpawnedBot = GetWorld()->SpawnActor<APawn>(BotPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (SpawnedBot)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bot nou spawnat la %s"), *SpawnLocation.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn bot in next wave!"));
		}
	}

	// === BOSS ===
	if (BossEnemyBotClass) // asigură-te că ai setat clasa BossPawnClass în Blueprint/în editor
	{
		float RandX = FMath::RandRange(-1300.f, 1300.f);
		float RandY = FMath::RandRange(-1300.f, 1300.f);
		FVector BossSpawnLocation(RandX, RandY, 400.f);
		FRotator BossSpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		APawn* SpawnedBoss = GetWorld()->SpawnActor<APawn>(BossEnemyBotClass, BossSpawnLocation, BossSpawnRotation, SpawnParams);
		if (SpawnedBoss)
		{
			UE_LOG(LogTemp, Warning, TEXT("BOSS spawnat la %s"), *BossSpawnLocation.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn BOSS in next wave!"));
		}
	}
}


void ACPP_Shooter2GameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Salvează progresul când se închide jocul
	/*CurrentEngineLevel = 1;
	CurrentTurboLevel = 1;
	CurrentHealthLevel = 1;
	CurrentWeightLevel = 1;*/
	/*AchievementStates[0] = 0;
	AchievementStates[1] = 0;
	AchievementStates[2] = 0;
	AchievementStates[3] = 0;
	AchievementStates[4] = 0;
	AchievementStates[5] = 0;
	AchievementStates[6] = 0;
	AchievementStates[7] = 0;
	AchievementStates[8] = 0;
	AchievementStates[9] = 0;
	AchievementStates[10] = 0;
	AchievementStates[11] = 0;
	AchievementStates[12] = 0;
	AchievementStates[13] = 0;
	AchievementStates[14] = 0;
	AchievementStates[15] = 0;*/



	SavePlayerData();

	UE_LOG(LogTemp, Warning, TEXT("Jocul se inchide - progres salvat."));
}
void ACPP_Shooter2GameMode::UpgradeEngine()
{
	if (CurrentEngineLevel < 20)
	{
		CurrentEngineLevel++;
		UE_LOG(LogTemp, Log, TEXT("Engine upgraded to level %d"), CurrentEngineLevel);
		SavePlayerData(); // Salvează automat după upgrade
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Engine is already at maximum level!"));
	}
}

void ACPP_Shooter2GameMode::UpgradeTurbo()
{
	if (CurrentTurboLevel < 20)
	{
		CurrentTurboLevel++;
		UE_LOG(LogTemp, Log, TEXT("Turbo upgraded to level %d"), CurrentTurboLevel);
		SavePlayerData();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Turbo is already at maximum level!"));
	}
}
void ACPP_Shooter2GameMode::UpgradeWeight()
{
	if (CurrentWeightLevel < 20)
	{
		CurrentWeightLevel++;
		UE_LOG(LogTemp, Log, TEXT("Weight upgraded to level %d"), CurrentWeightLevel);
		SavePlayerData();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Weight is already at maximum level!"));
	}
}
void ACPP_Shooter2GameMode::SetCurrentEngineLevel(int32 NewLevel)
{
	if (NewLevel >= 1 && NewLevel <= 20)
	{
		CurrentEngineLevel = NewLevel;
		UE_LOG(LogTemp, Log, TEXT("Engine level set to %d"), CurrentEngineLevel);
		SavePlayerData(); // Salvează automat când se schimbă valoarea
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid engine level: %d. Must be between 1 and 20."), NewLevel);
	}
}

void ACPP_Shooter2GameMode::SetCurrentTurboLevel(int32 NewLevel)
{
	if (NewLevel >= 1 && NewLevel <= 20)
	{
		CurrentTurboLevel = NewLevel;
		UE_LOG(LogTemp, Log, TEXT("Turbo level set to %d"), CurrentTurboLevel);
		SavePlayerData();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid turbo level: %d. Must be between 1 and 20."), NewLevel);
	}
}

void ACPP_Shooter2GameMode::SetCurrentWeightLevel(int32 NewLevel)
{
	if (NewLevel >= 1 && NewLevel <= 20)
	{
		CurrentWeightLevel = NewLevel;
		UE_LOG(LogTemp, Log, TEXT("Weight level set to %d"), CurrentWeightLevel);
		SavePlayerData();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid weight level: %d. Must be between 1 and 20."), NewLevel);
	}
}
void ACPP_Shooter2GameMode::SetCurrentHealthtLevel(int32 NewLevel)
{
	if (NewLevel >= 1 && NewLevel <= 51)
	{
		CurrentHealthLevel = NewLevel;
		UE_LOG(LogTemp, Log, TEXT("Health level set to %d"), CurrentHealthLevel);
		SavePlayerData();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid weight level: %d. Must be between 1 and 20."), NewLevel);
	}
}

void ACPP_Shooter2GameMode::ResetPlayerData()
{
	// Resetează monedele și armele
	CurrentCoins = 1000;
	CurrentUnlockedWeapons.Empty();

	// Resetează upgrade-urile mașinii la nivel 1
	CurrentEngineLevel = 1;
	CurrentTurboLevel = 1;
	CurrentWeightLevel = 1;
	CurrentHealthLevel = 1;

	// Resetează armele la valorile default
	OwnedWeaponNames.Empty();
	WeaponName1 = "Empty";
	WeaponName2 = "Empty";

	// Resetează consumabilele
	Grenades = 0;
	Molotovs = 0;
	SmokeGrenades = 0;

	// Resetează munițiile
	bullet556 = 0;
	bullet762 = 0;
	bullet9 = 0;

	// Resetează toate achievement-urile
	for (int32 i = 0; i < AchievementStates.Num(); i++)
	{
		AchievementStates[i] = 0;
	}

	// Resetează statisticile
	TopSpeed = 0.0f;
	CarDistance = 0.0f;
	TotalKills = 0;
	Games = 0;
	MaxKillDistance = 0.0f;
	TotalGrenades = 0;
	PlayerDistance = 0.0f;
	NumberDies = 0;
	BossesKilled = 0;

	// Salvează noile valori resetate
	SavePlayerData();

	// Update HUD pentru a reflecta schimbările
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (ACPP_Shooter2HUD* HUD = Cast<ACPP_Shooter2HUD>(PC->GetHUD()))
		{
			HUD->UpdateHUD();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("🔄 Toate datele jucătorului au fost resetate la valorile implicite!"));
}



