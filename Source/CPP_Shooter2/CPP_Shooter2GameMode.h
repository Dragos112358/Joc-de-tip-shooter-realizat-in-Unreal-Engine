// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CPP_Shooter2HUD.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Bullet.h"
#include "DragRace.h"
#include "MenuWidget.h"
#include "CPP_Shooter2GameMode.generated.h"


UCLASS(minimalapi)
class ACPP_Shooter2GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACPP_Shooter2GameMode();
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode")
	//TSubclassOf<class AHUD> HUDClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bot")
	TSubclassOf<APawn> BotPawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bot")
	TSubclassOf<APawn> BossEnemyBotClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bot")
	TSubclassOf<APawn> RangedBotPawnClass;

	UPROPERTY(EditAnywhere, Category = "DragRace")
	TSubclassOf<ADragRace> DragRaceClass;

	// funcție pentru spawn bot (optional)
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<ABullet> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(BlueprintReadWrite, Category = "Car Upgrades")
	int32 CurrentEngineLevel = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Car Upgrades")
	int32 CurrentTurboLevel = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Car Upgrades")
	int32 CurrentWeightLevel = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Player Upgrade")
	int32 CurrentHealthLevel = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Player achievements")
	TArray<int32> AchievementStates = TArray<int32>({
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		});

	UPROPERTY(BlueprintReadWrite, Category = "Player speed")
	float TopSpeed = 0.f;


	UFUNCTION(BlueprintCallable, Category = "Player speed")
	float GetTopSpeed() const
	{
		return TopSpeed;
	}

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Player speed")
	void SetTopSpeed(float NewSpeed)
	{
		TopSpeed = NewSpeed;
	}


	UPROPERTY(VisibleAnywhere, Category = "Total Car Distance")
	float CarDistance = 0;

	UFUNCTION(BlueprintCallable, Category = "Total Car Distance")
	float GetCarDistance() const
	{
		return CarDistance;
	}

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Total Car Distance")
	void SetCarDistance(float NewDistance)
	{
		CarDistance = NewDistance;
	}


	// Getter
	UFUNCTION(BlueprintCallable, Category = "Player achievements")
	int32 GetAchievementState(int32 Index) const
	{
		return AchievementStates.IsValidIndex(Index) ? AchievementStates[Index] : -1;
	}

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Player achievements")
	void SetAchievementState(int32 Index, int32 NewValue)
	{
		if (AchievementStates.IsValidIndex(Index))
		{
			AchievementStates[Index] = NewValue;
		}
	}




	UPROPERTY(BlueprintReadWrite, Category = "Player Upgrade")
	TArray<FString> OwnedWeaponNames;

	UFUNCTION(BlueprintCallable, Category = "Player Upgrade")
	TArray<FString> GetOwnedWeaponNames() const
	{
		return OwnedWeaponNames;
	}

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Player Upgrade")
	void SetOwnedWeaponNames(const TArray<FString>& NewOwnedWeaponNames)
	{
		OwnedWeaponNames = NewOwnedWeaponNames;
	}

	
	UPROPERTY(BlueprintReadWrite, Category = "Weapons")
	FString WeaponName1;

	UPROPERTY(BlueprintReadWrite, Category = "Weapons")
	FString WeaponName2;
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	FString GetWeaponName1() const { return WeaponName1; }

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	FString GetWeaponName2() const { return WeaponName2; }

	// ===== Settere =====
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void AddOwnedWeapon(const FString& NewWeapon) { OwnedWeaponNames.Add(NewWeapon); }
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void SetWeaponName1(const FString& NewName) { 
		WeaponName1 = NewName;
		SavePlayerData();
		 
	}
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void SetWeaponName2(const FString& NewName) { 
		WeaponName2 = NewName;
		SavePlayerData(); 
	}

	// Funcții pentru upgrade-uri
	UFUNCTION(BlueprintCallable, Category = "Car Upgrades")
	void UpgradeEngine();

	UFUNCTION(BlueprintCallable, Category = "Car Upgrades")
	void UpgradeTurbo();

	UFUNCTION(BlueprintCallable, Category = "Car Upgrades")
	void UpgradeWeight();


	UFUNCTION(BlueprintCallable, Category = "Player ugrade")
	int32 GetCurrentHealthLevel() const { return CurrentHealthLevel; }

	UFUNCTION(BlueprintCallable, Category = "Car Upgrades")
	int32 GetCurrentEngineLevel() const { return CurrentEngineLevel; }

	UFUNCTION(BlueprintCallable, Category = "Car Upgrades")
	int32 GetCurrentTurboLevel() const { return CurrentTurboLevel; }

	UFUNCTION(BlueprintCallable, Category = "Car Upgrades")
	int32 GetCurrentWeightLevel() const { return CurrentWeightLevel; }

	// Setters pentru upgrade-uri
	UFUNCTION(BlueprintCallable, Category = "Car Upgrades")
	void SetCurrentEngineLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Car Upgrades")
	void SetCurrentTurboLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Car Upgrades")
	void SetCurrentWeightLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Player Upgrades")
	void SetCurrentHealthtLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Save System")
	void ResetPlayerData();

	void SpawnBullet(const FVector& SpawnLocation, const FRotator& SpawnRotation, EBulletType BulletType);
	void SpawnWeapon(const FVector& SpawnLocation, const FRotator& SpawnRotation, EWeaponType WeaponType);
	void SavePlayerData();
	void LoadPlayerData();
	void SpawnRangedBot();
	void SpawnBot();
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MenuWidgetClass;
	UPROPERTY()
	int32 CurrentCoins;

	UPROPERTY()
	TArray<FString> CurrentUnlockedWeapons;
	int32 GetCurrentCoins() const { return CurrentCoins; }
	const TArray<FString>& GetCurrentUnlockedWeapons() const { return CurrentUnlockedWeapons; }
	void SetCurrentCoins(int32 NewCoins) { CurrentCoins = NewCoins; }
	void SetCurrentUnlockedWeapons(const TArray<FString>& NewWeapons) { CurrentUnlockedWeapons = NewWeapons; }
	void SpawnBoss();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	int32 BotsKilledCount = 0;
	int32 MaxBotsToKillBeforeNextSpawn = 4;
	int32 ActiveBotCount = 0;
	int32 MaxActiveBots = 5;

	UPROPERTY(VisibleAnywhere, Category = "Player Grenades")
	int32 Grenades = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Grenades")
	int32 Molotovs = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Grenades")
	int32 SmokeGrenades = 0;

	//partea de grenade
	// ======== Getters ========
	UFUNCTION(BlueprintCallable, Category = "Player Grenades")
	int32 GetGrenades() const { return Grenades; }

	UFUNCTION(BlueprintCallable, Category = "Player Grenades")
	int32 GetMolotovs() const { return Molotovs; }

	UFUNCTION(BlueprintCallable, Category = "Player Grenades")
	int32 GetSmokeGrenades() const { return SmokeGrenades; }

	// ======== Setters ========
	UFUNCTION(BlueprintCallable, Category = "Player Grenades")
	void SetGrenades(int32 NewValue) { Grenades = FMath::Max(0, NewValue); }

	UFUNCTION(BlueprintCallable, Category = "Player Grenades")
	void SetMolotovs(int32 NewValue) { Molotovs = FMath::Max(0, NewValue); }

	UFUNCTION(BlueprintCallable, Category = "Player Grenades")
	void SetSmokeGrenades(int32 NewValue) { SmokeGrenades = FMath::Max(0, NewValue); }

	// ======== Add/Subtract Helpers ========
	UFUNCTION(BlueprintCallable, Category = "Player Grenades")
	void AddGrenades(int32 Amount) { Grenades = FMath::Max(0, Grenades + Amount); }

	UFUNCTION(BlueprintCallable, Category = "Player Grenades")
	void AddMolotovs(int32 Amount) { Molotovs = FMath::Max(0, Molotovs + Amount); }

	UFUNCTION(BlueprintCallable, Category = "Player Grenades")
	void AddSmokeGrenades(int32 Amount) { SmokeGrenades = FMath::Max(0, SmokeGrenades + Amount); }


	UPROPERTY(VisibleAnywhere, Category = "Player Bullets")
	int32 bullet556 = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Bullets")
	int32 bullet762 = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Bullets")
	int32 bullet9 = 0;



	UPROPERTY(VisibleAnywhere, Category = "Total Kills")
	int TotalKills = 0;


	UFUNCTION(BlueprintCallable, Category = "Total Kills")
	int GetTotalKills() const
	{
		return TotalKills;
	}

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Total Kills")
	void SetTotalKills(int NewTotalKills)
	{
		TotalKills = NewTotalKills;
	}

	UPROPERTY(VisibleAnywhere, Category = "Total Games")
	int Games = 0;
	UFUNCTION(BlueprintCallable, Category = "Total Games")
	int32 GetGames() const
	{
		return Games;
	}

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Total Games")
	void SetGames(int32 NewGames)
	{
		Games = NewGames;
	}

	UPROPERTY(VisibleAnywhere, Category = "Kill range")
	float MaxKillDistance = 0.f;

	UFUNCTION(BlueprintCallable, Category = "Kill range")
	float GetMaxKillDistance() const
	{
		return MaxKillDistance;
	}

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Kill range")
	void SetMaxKillDistance(float NewDistance)
	{
		MaxKillDistance = NewDistance;
	}

	UPROPERTY(VisibleAnywhere, Category = "Total Grenades")
	int TotalGrenades = 0;

	UFUNCTION(BlueprintCallable, Category = "Total Grenades")
	int32 GetTotalGrenades() const
	{
		return TotalGrenades;
	}

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Total Grenades")
	void SetTotalGrenades(int32 NewTotalGrenades)
	{
		TotalGrenades = NewTotalGrenades;
	}

	UPROPERTY(VisibleAnywhere, Category = "PlayerDistance")
	float PlayerDistance = 0.f;

	UFUNCTION(BlueprintCallable, Category = "PlayerDistance")
	float GetPlayerDistance() const
	{
		return PlayerDistance;
	}

	// Setter
	UFUNCTION(BlueprintCallable, Category = "PlayerDistance")
	void SetPlayerDistance(float NewPlayerDistance)
	{
		PlayerDistance = NewPlayerDistance;
	}


	UPROPERTY(VisibleAnywhere, Category = "Die")
	int NumberDies = 0;


	UFUNCTION(BlueprintCallable, Category = "Die")
	int GetNumberDies() const
	{
		return NumberDies;
	}

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Die")
	void SetNumberDies(int NewNumberDies)
	{
		NumberDies = NewNumberDies;
	}

	UPROPERTY(VisibleAnywhere, Category = "Boss Kill")
	int BossesKilled = 0;

	// Getter
	UFUNCTION(BlueprintCallable, Category = "Boss Kill")
	int32 GetBossesKilled() const
	{
		return BossesKilled;
	}

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Boss Kill")
	void SetBossesKilled(int32 NewBossesKilled)
	{
		BossesKilled = NewBossesKilled;
	}


	//gettere si settere pentru gloante
	UFUNCTION(BlueprintCallable, Category = "Player Bullets")
	int32 GetBullet556() const { return bullet556; }

	UFUNCTION(BlueprintCallable, Category = "Player Bullets")
	int32 GetBullet762() const { return bullet762; }

	UFUNCTION(BlueprintCallable, Category = "Player Bullets")
	int32 GetBullet9() const { return bullet9; }

	// ========= Setters =========
	UFUNCTION(BlueprintCallable, Category = "Player Bullets")
	void SetBullet556(int32 NewValue) { bullet556 = FMath::Max(0, NewValue); }

	UFUNCTION(BlueprintCallable, Category = "Player Bullets")
	void SetBullet762(int32 NewValue) { bullet762 = FMath::Max(0, NewValue); }

	UFUNCTION(BlueprintCallable, Category = "Player Bullets")
	void SetBullet9(int32 NewValue) { bullet9 = FMath::Max(0, NewValue); }

	// ========= Add / Subtract Helpers =========
	UFUNCTION(BlueprintCallable, Category = "Player Bullets")
	void AddBullet556(int32 Amount) { bullet556 = FMath::Max(0, bullet556 + Amount); }

	UFUNCTION(BlueprintCallable, Category = "Player Bullets")
	void AddBullet762(int32 Amount) { bullet762 = FMath::Max(0, bullet762 + Amount); }

	UFUNCTION(BlueprintCallable, Category = "Player Bullets")
	void AddBullet9(int32 Amount) { bullet9 = FMath::Max(0, bullet9 + Amount); }



private:

public:
	void OnBotKilled();
	void SpawnNextWaveBots();
};



