#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Texture2D.h"
#include "Engine/SceneCapture2D.h"              // Pentru ASceneCapture2D și SceneCaptureComponent2D
#include "Components/SceneCaptureComponent2D.h" // Pentru USceneCaptureComponent2D
#include "Engine/TextureRenderTarget2D.h"       // Pentru UTextureRenderTarget2D
#include "Bullet.h"
#include "Weapon.h"
#include "Projectiles.h"
#include "CPP_Shooter2HUD.generated.h"

USTRUCT()
struct CPP_SHOOTER2_API FInventoryItem
{
	GENERATED_BODY()

	UTexture2D* Icon = nullptr;
	FString Name;
	int32 Quantity = 0;
	bool bIsWeapon = false;
	EBulletType BulletType;
	int32 AmmoInMagazine = 0;
	float FireRate = 0.f;
	float Damage = 0.f;

};


UCLASS()
class CPP_SHOOTER2_API ACPP_Shooter2HUD : public AHUD
{
	GENERATED_BODY()

public:
	ACPP_Shooter2HUD();

	virtual void DrawHUD() override;
	int32 SelectedWeaponSlot = 1;
	void SetWeaponInSlot(int32 SlotIndex, const FString& WeaponName, UTexture2D* WeaponIcon = nullptr);
	FString GetWeaponInSlot(int32 SlotIndex) const;
	void UpdateMenuButtonPositions();
	void CloseMainMenu();
	void OpenMainMenu();

	//afisare meniuri pe ecran
	bool bShowMainMenu = true;
	bool bShowStore = false;
	bool bShowSettings = false;
	bool bShowAchievements = false;
	TArray<FVector2D> MenuButtonPositions; //pozitiii butoane
	//health
	UPROPERTY(EditAnywhere, Category = "HUD")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "HUD")
	float CurrentHealth = 100.0f;


	int StageTargets7[8] = { 1000, 2000, 5000, 10000, 20000, 50000, 100000, 300000 };
	int StageTargets8[8] = { 1, 10, 50, 200, 750, 1500, 3000, 5000 };
	int StageTargets9[8] = { 120, 1200, 6000, 30000, 70000, 150000, 400000, 1000000 };
	int StageTargets10[8] = { 1, 3, 10, 20, 50, 100, 150, 200 };
	int StageTargets11[8] = { 10, 50, 200, 500, 1000, 2000, 5000, 6900 };
	int StageTargets12[8] = { 1, 10, 20, 35, 50, 75, 125, 150 };
	int StageTargets13[8] = { 1, 10, 25, 50, 100, 150, 250, 400 };
	int StageTargets14[8] = { 500, 1000, 2500, 5000, 10000, 25000, 42000, 100000 };
	int StageTargets15[8] = { 1, 3, 10, 20, 40, 70, 100, 125 };
	int StageTargets16[8] = { 1, 3, 5, 10, 15, 25, 50, 100 };
	int StageTargets17[5] = { 1, 3, 5, 7, 9};

	//preturi
	int StagePrices[4] = { 100, 200, 400, 600 }; //engine, turbo si weight
	int StagePrices5[5] = { 100, 150, 250, 400, 750 }; //health
	int StagePrices6[8] = { 100, 150, 250, 400, 750, 900, 1200, 1500 }; //speed
	int StagePrices7[8] = { 20, 40, 100, 200, 500, 750, 1000, 1250 }; //distance in car
	int StagePrices8[8] = { 20, 40, 100, 200, 350, 500, 750, 1250 }; //kills
	int StagePrices9[8] = { 20, 40, 75, 135, 250, 450, 750, 1250 }; //damage dealt
	int StagePrices10[8] = { 20, 40, 60, 100, 150, 250, 450, 750 }; //open the game
	int StagePrices11[8] = { 20, 40, 100, 200, 275,350, 500, 700 }; //money spent in game
	int StagePrices12[8] = { 5, 50, 75, 120, 200,275, 400, 550 }; //maxkilldistance
	int StagePrices13[8] = { 50, 100, 200, 350, 600, 800, 1000, 1500 }; //grenade
	int StagePrices14[8] = { 10, 50, 100, 200, 350, 500, 650, 1000 }; //walking achievement
	int StagePrices15[8] = { 10, 20, 50, 75, 100, 250, 500, 800 }; //achievement for number dies
	int StagePrices16[8] = { 10, 20, 50, 75, 100, 250, 500, 800 }; //bosses killed
	int StagePrices17[5] = { 200, 300, 500, 750, 1200 }; //weapons owned

	UPROPERTY()
	FVector2D ResetPlayerDataButtonPos;

	UPROPERTY()
	FVector2D ResetPlayerDataButtonSize;




	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//pentru partea de regen cu bauturi energizante
	float CurrentEnergy = 0;
	float MaxEnergy = 100;
	int32 EnergyDrinks = 0;
	int32 Painkillers = 0;
	int32 Adrenaline_Syringe = 0;

	void SelectSlot1();
	void SelectSlot2();
	void SelectSlot5();
	void SelectGrenade();
	void SetGrenade(const FString& Name, UTexture2D* Icon);
	void SelectMolotov_Cocktail();
	void ToggleInventory();
	bool IsPointInRect(const FVector2D& Point, const FVector2D& RectTopLeft, const FVector2D& RectSize);
	void DrawGrenadeImpactPoint();
	bool IsInventoryOpen();
	void DrawCrosshair();
	void AddWeaponToInventory(AWeapon* Weapon);
	void SpawnOrUpdateVisualWeapon(EWeaponType WeaponType, float SwayAmplitude, float SwaySpeed);
	void DrawMiniMap();
	EBulletType Match_Weapon_with_bullets(const FString& WeaponName);
	void InitMinimapCapture();

	EWeaponType CurrentVisualWeaponType = EWeaponType::None; // adaugă un tip None în enum dacă nu există
	EWeaponType GetWeaponTypeFromName(const FString& WeaponName);
	// Pentru slot 1
	AWeapon* SpawnedWeaponSlot1 = nullptr;
	EWeaponType CurrentVisualWeaponTypeSlot1 = EWeaponType::None;

	// Pentru slot 2
	AWeapon* SpawnedWeaponSlot2 = nullptr;
	EWeaponType CurrentVisualWeaponTypeSlot2 = EWeaponType::None;

	//sunete
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* WalkingSound;
	UPROPERTY()
	UAudioComponent* WalkingAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* PickupWeaponSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ScopeSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* FallingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* SwordSound;
	UPROPERTY(BlueprintReadWrite)
	FString BulletNearbyMessage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD Message")
	FString BulletMessage;

	// Funcția care va fi apelată când delegate-ul transmite mesajul
	UFUNCTION()
	void HandleBulletNearbyMessage(FString Message);

	//functie de calculat damage arma
	UFUNCTION(BlueprintCallable, Category = "Weapon System")
	int32 GetCurrentWeaponDamage();
	// Inventory UI
	bool bIsInventoryOpen;
	FVector2D CloseButtonPos;
	FVector2D CloseButtonSize = FVector2D(50.f, 30.f);

	//vad cand se misca personajul meu
	bool bIsMoving2 = false;

	// Inventory logic
	bool bIsInventoryVisible = false;
	bool slot12 = true;
	bool slot345 = false;
	TArray<FInventoryItem> InventoryItems;

	//parte de reloading
	bool bIsReloading = false;
	float ReloadTimer = 0.f;
	float ReloadDuration = 2.f; // secunde
	FString ReloadingWeaponName;
	//partea de vedere a hartii
	bool bSeeMap = false;

	// Pentru zoom și drag
	FVector2D MapOffset = FVector2D::ZeroVector;
	float MapZoom = 1.0f;
	bool bDraggingMap = false;
	FVector2D LastMousePos;

	// Referință la PlayerController
	APlayerController* CachedPC = nullptr;

	//daca ma aflu langa gloante, le dau pickup
	UPROPERTY()
	bool bShowBulletIcon = false;

	//Gloante in zona
	int32 Nearby556Count = 0;
	int32 Nearby762Count = 0;
	int32 Nearby9mmCount = 0;

	//gloante totale in inventar
	int32 gloante_556 = 0;
	int32 gloante_762 = 0;
	int32 gloante_9 = 0;

	//gloantele pentru cele 2 arme
	int32 AmmoInMag1 = 30;    // Gloanțe în încărcător pentru Slot1
	int32 MaxAmmoInMag1 = 30;
	int32 MaxAmmoInMag2 = 30;
	int32 TotalAmmo1 = 90;    // Gloanțe totale pentru Slot1

	int32 AmmoInMag2 = 30;     // Gloanțe în încărcător pentru Slot2
	int32 TotalAmmo2 = 30;    // Gloanțe totale pentru Slot2

	//pentru fall damage
	FVector LastGroundedLocation;
	FVector LastLocation2;
	FVector LastVelocity;
	bool bIsFalling = false;
	//Parte de pick
	bool PickBullet = false;
	bool bWasInAir = false;
	bool bJustLanded = false;


	//Parte de creare arma automata/neautomata
	bool AutomaticWeapon;
	void AddBulletToInventory(EBulletType BulletType, int32 quantity);
	void ReloadWeapon(EBulletType BulletType, int32 WeaponSlot);
	void ViewWithScope();
	void DrawShotgunTarget();
	void UpdateMiniMapCamera();
	void ApplyRecoil();

	void StartAutoFire();
	void StopAutoFire();
	void HandleAutoFire();
	void AnimateEnergyDrink(float SwayAmplitude, float SwaySpeed);

	FTimerHandle FireTimer;

	UPROPERTY()
	ASceneCapture2D* SceneCaptureActor;

	UPROPERTY()
	USceneCaptureComponent2D* MiniMapCapture;

	UPROPERTY()
	UTextureRenderTarget2D* MiniMapRenderTarget;

	//efect de sange
	UPROPERTY()
	UTexture2D* BloodTexture;

	UPROPERTY(VisibleAnywhere, Category = "Player Data")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, Category = "Player Data")
	TArray<FString> UnlockedWeapons;

	// Timpul când s-a luat damage
	float BloodOverlayEndTime;
	void DrawGauge(FVector2D Pos, float Radius, float MaxValue, float Value, FString Label, FColor Color, int NumTicks);
	void UpdateHUD();

	// Funcție publică pentru a afișa overlay-ul
	void ShowBloodOverlay(float Duration = 2.0f);
	FVector2D LastPoint;
	TArray<AProjectiles*> SpawnShotgunProjectiles(
		FVector SpawnOrigin,
		FVector ShootDirection,
		FRotator BaseRotation,
		const FActorSpawnParameters& SpawnParams,
		int32 NumProjectiles,
		float SpreadAngleDegrees);
	TArray<AProjectiles*> AllProjectiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Info")
	UStaticMeshComponent* WeaponMesh2;

	UPROPERTY(VisibleAnywhere, Category = "Player Grenades")
	int32 Grenades = 5;

	UPROPERTY(VisibleAnywhere, Category = "Player Grenades")
	int32 Molotovs = 5;

	UPROPERTY(VisibleAnywhere, Category = "Player Grenades")
	int32 SmokeGrenades = 5;


	UPROPERTY()
	UStaticMeshComponent* ShoulderWeaponMesh;
	FVector LastPlayerLocation;
	float SwayTime = 0.f;

	//am sau nu arma echipata
	bool bhasweapon = false;
	//energia scade cu 1 o data la 3 secunde
	float EnergyDrainTimer = 0.f;
	float EnergyDrainInterval = 3.f;

	//gloante pe armele mele
	int32 maxbulletsSlot1 = 30;
	int32 bulletsSlot1 = 30;
	int32 maxbulletsSlot2 = 30;
	int32 bulletsSlot2 = 30;
	bool bIsDrinking = false;
	void StartDrinking();
	float SwayTime2 = 0;

	//masina
	float CurrentSpeed;   // viteza în km/h
	float CurrentRPM;     // turatie
	bool PlayerInCar = false;
	UPROPERTY()
	UStaticMeshComponent* HandMeshComponent;
	class UStaticMesh* Hand;

	UPROPERTY()
	UStaticMesh* Hands;

	UPROPERTY()
	UStaticMeshComponent* DrinkMeshComponent;
	UPROPERTY()
	USkeletalMesh* HandMeshAsset;

	UPROPERTY()
	class UStaticMesh* Mesh_Drink;

	UPROPERTY()
	class UStaticMesh* Mesh_Painkiller;

	UPROPERTY()
	class UStaticMesh* Mesh_Adrenaline;


	//pentru modificare sunet
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	class USoundMix* MasterSoundMix;

	//aici e partea de setari
	//partea de setari
	// Pentru bara de luminozitate
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings|Brightness")
	int32 BrightnessLevel = 7; // nivel inițial (1–7)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings|Brightness")
	TArray<FVector2D> BrightnessButtonPositions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings|Brightness")
	TArray<FVector2D> BrightnessButtonSizes;

	// Pentru grafica
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings|Graphics")
	int32 CurrentGraphicsQuality = 0; // 0 = Low, 1 = Medium, 2 = High, 3 = Ultra

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings|Graphics")
	TArray<FVector2D> GraphicsButtonPositions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings|Graphics")
	TArray<FVector2D> GraphicsButtonSizes;

	// Pentru back button
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings|BackButton")
	FVector2D BackButtonPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings|BackButton")
	FVector2D BackButtonSize2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 VolumeLevel = 5; // valoare inițială

	// Pozițiile și dimensiunile butoanelor pentru volum
	UPROPERTY(VisibleAnywhere, Category = "Settings")
	TArray<FVector2D> VolumeButtonPositions;

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	TArray<FVector2D> VolumeButtonSizes;

	//void ApplySettings();

	//achievements
	TArray<FVector2D> AchievementButtonPositions;
	TArray<FVector2D> AchievementButtonSizes;

	// Pozițiile și dimensiunile barelor de progres
	TArray<FVector2D> AchievementBarPositions;
	TArray<FVector2D> AchievementBarSizes;


private:
	// Weapon slots
	FString WeaponSlot1;
	FString WeaponSlot2;
	UTexture2D* WeaponIcon1 = nullptr;
	UTexture2D* WeaponIcon2 = nullptr;
	FString GrenadeSlot;
	UTexture2D* GrenadeIcon = nullptr;
	UTexture2D* Slot4Icon = nullptr;
	UTexture2D* Slot5Icon = nullptr;
	FString MolotovSlot;
	FString WeaponSlot5;
	UTexture2D* WeaponIcon5 = nullptr;

	// Ammo icons
	UTexture2D* Slot556Icon = nullptr;
	UTexture2D* Slot762Icon = nullptr;
	UTexture2D* Slot9Icon = nullptr;

	//Weapons icons
	UTexture2D* AKM_image = nullptr;
	UTexture2D* Scar_image = nullptr;
	UTexture2D* M416_image = nullptr;
	UTexture2D* VSS_image = nullptr;
	UTexture2D* M762_image = nullptr;
	UTexture2D* M249_image = nullptr;
	UTexture2D* AWM_image = nullptr;
	UTexture2D* SKS_image = nullptr;
	UTexture2D* Groza_image = nullptr;
	UTexture2D* AUG_image = nullptr;
	UTexture2D* KAR98K_image = nullptr;
	UTexture2D* S686_image = nullptr;
	UTexture2D* DP28_image = nullptr;
	TArray<FString> WeaponNames = {
	TEXT("AWM"),
	TEXT("S686"),
	TEXT("M249"),
	TEXT("Scar-l"),
	TEXT("M416"),
	TEXT("DP28"),
	TEXT("M762"),
	TEXT("Kar98K"),
	TEXT("AKM")
	};


	TArray<int32> WeaponPrices = {
			500,    // Pistol
			300,   // Shotgun
			1200,   // Rifle
			900,   // Sniper
			1000,   // Rocket
			950,   // SMG
			800,   // GrenadeLauncher
			250,    // LaserGun
			1000
	};

	//scopes
	UTexture2D* Scopex2_image = nullptr;
	UTexture2D* Scopex3_image = nullptr;
	UTexture2D* Scopex4_image = nullptr;
	UTexture2D* Scopex6_image = nullptr;
	UTexture2D* Scopex8_image = nullptr;

	UTexture2D* Car_image = nullptr;
	TArray<FVector2D> UpgradeButtonPositions;
	TArray<FVector2D> UpgradeButtonSizes;

	//pentru arme
	TArray<FVector2D> WeaponButtonPositions;
	TArray<FVector2D> WeaponButtonSizes;
	FVector2D BackButtonPos;
	FVector2D BackButtonPos2;
	FVector2D BackButtonSize;

	FVector2D BackButtonPos3;
	FVector2D BackButtonSize3;

	//lista de iconuri
	TArray<UTexture2D*> WeaponIcons;

	//pentru grenade
	TArray<FVector2D> ThrowableButtonPositions;
	TArray<FVector2D> ThrowableButtonSizes;
	//pentru gloante
	TArray<FVector2D> AmmoButtonPositions;
	TArray<FVector2D> AmmoButtonSizes;

	UTexture2D* Coin_image = nullptr;


	UPROPERTY()
	AWeapon* SpawnedWeapon = nullptr;

	// Throwables
	void ThrowGrenade();
	void ThrowMolotov();
	void ThrowSmoke();

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<class AGrenade> GrenadeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<class AMolotov_Cocktail> MolotovClass;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<class ASmoke_Grenade> SmokeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<class ABullet> BulletClass ;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Projectiles")
	TSubclassOf<AProjectiles> ProjectilesClass; // Setezi din editor blueprint-ul de glonț

	void SpawnBullet();
	//pentru scope
	bool bIsScoped = false;
	bool IsShotgun = false;
	// Backpack icon
	UTexture2D* BackpackIcon = nullptr;
	FVector2D BackpackPos;
	FVector2D BackpackSize;

};
