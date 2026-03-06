// MySaveGame.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

UCLASS()
class CPP_SHOOTER2_API UMySaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, Category="Player Data")
    int32 Coins;

    UPROPERTY(VisibleAnywhere, Category="Player Data")
    TArray<FString> UnlockedWeapons;

    UPROPERTY(VisibleAnywhere, Category = "Car Upgrades")
    int32 EngineLevel = 1;

    UPROPERTY(VisibleAnywhere, Category = "Car Upgrades")
    int32 TurboLevel = 1;

    UPROPERTY(VisibleAnywhere, Category = "Car Upgrades")
    int32 WeightLevel = 1;

    UPROPERTY(VisibleAnywhere, Category = "Player upgrade")
    int32 HealthLevel = 1;

    UPROPERTY(VisibleAnywhere, Category = "Player Weapons")
    TArray<FString> OwnedWeapon = {};

    UPROPERTY(VisibleAnywhere, Category = "Player Weapons")
    FString WeaponName1;

    UPROPERTY(VisibleAnywhere, Category = "Player Weapons")
    FString WeaponName2;

    UPROPERTY(VisibleAnywhere, Category = "Player Bullets")
    int32 Bullets556 = 0;

    UPROPERTY(VisibleAnywhere, Category = "Player Bullets")
    int32 Bullets762 = 0;

    UPROPERTY(VisibleAnywhere, Category = "Player Bullets")
    int32 Bullets9 = 0;

    UPROPERTY(VisibleAnywhere, Category = "Player Grenades")
    int32 Grenades = 0;

    UPROPERTY(VisibleAnywhere, Category = "Player Grenades")
    int32 Molotovs = 0;

    UPROPERTY(VisibleAnywhere, Category = "Player Grenades")
    int32 SmokeGrenades = 0;
    

    UPROPERTY(VisibleAnywhere, Category = "Player Bullets")
    int32 bullet556 = 0;

    UPROPERTY(VisibleAnywhere, Category = "Player Bullets")
    int32 bullet762 = 0;

    UPROPERTY(VisibleAnywhere, Category = "Player Bullets")
    int32 bullet9 = 0;

    UPROPERTY(VisibleAnywhere, Category = "Player achievements")
    TArray<int32> AchievementStates = TArray<int32>({
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        });

    UPROPERTY(VisibleAnywhere, Category = "Top Speed Car")
    float TopSpeed = 0;

    UPROPERTY(VisibleAnywhere, Category = "Distance Car")
    float CarDistance = 0;

    UPROPERTY(VisibleAnywhere, Category = "Kills")
    int TotalKills = 0;

    UPROPERTY(VisibleAnywhere, Category = "Play time")
    int Games = 0;

    UPROPERTY(VisibleAnywhere, Category = "Kill range")
    float MaxKillDistance = 0.f;

    UPROPERTY(VisibleAnywhere, Category = "Total Grenades")
    int TotalGrenades = 0;

    UPROPERTY(VisibleAnywhere, Category = "Player Distance")
    float PlayerDistance = 0.f;

    UPROPERTY(VisibleAnywhere, Category = "Die")
    int NumberDies = 0;

    UPROPERTY(VisibleAnywhere, Category = "Boss Kill")
    int BossesKilled = 0;







};
