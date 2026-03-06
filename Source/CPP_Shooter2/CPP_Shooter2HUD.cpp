#include "CPP_Shooter2HUD.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/HUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Grenade.h"
#include "Smoke_Grenade.h"
#include "Molotov_Cocktail.h"
#include "Bullet.h"
#include "Projectiles.h"
#include "Enemy_Bot.h"
#include "EnemyRangedBot.h"
#include "BossEnemyBot.h"
#include "Engine/SceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "CPP_Shooter2GameMode.h"
#include "Components/AudioComponent.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "Car.h"
#include <Components/LineBatchComponent.h>
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Engine/World.h"
#include "AudioDevice.h"
#include "HAL/IConsoleManager.h"






ACPP_Shooter2HUD::ACPP_Shooter2HUD()
{
    WeaponMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh2"));
    RootComponent = WeaponMesh2;
    WeaponSlot1 = TEXT("Empty");
    WeaponSlot2 = TEXT("Empty");
    GrenadeSlot = TEXT("Grenade");
    WeaponSlot5 = TEXT("Empty");
    WeaponIcon5 = nullptr;
    GrenadeIcon = nullptr;
    Slot4Icon = nullptr;
    bIsInventoryOpen = false;
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bTickEvenWhenPaused = true;
    GrenadeClass = AGrenade::StaticClass();
    MolotovClass = AMolotov_Cocktail::StaticClass();
    SmokeClass = ASmoke_Grenade::StaticClass();
    BulletClass = ABullet::StaticClass();
    WeaponClass = AWeapon::StaticClass();
    ProjectilesClass = AProjectiles::StaticClass();
    static ConstructorHelpers::FObjectFinder<UTexture2D> GrenadeTexObj(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/grenade-no-bg.grenade-no-bg'")   // <- înlocuiește cu calea ta exactă
    );
    if (GrenadeTexObj.Succeeded())
    {
        GrenadeIcon = GrenadeTexObj.Object;
        UE_LOG(LogTemp, Log, TEXT("Grenade icon loaded in HUD CDO."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find grenade icon texture!"));
    }
    static ConstructorHelpers::FObjectFinder<UTexture2D> GrenadeTexObj2(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/source/molotov-cocktail.molotov-cocktail'")   // <- înlocuiește cu calea ta exactă
    );
    if (GrenadeTexObj2.Succeeded())
    {
        Slot4Icon = GrenadeTexObj2.Object;
        UE_LOG(LogTemp, Log, TEXT("Grenade icon2 loaded in HUD CDO."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find grenade2 icon texture!"));
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D> GrenadeTexObj3(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/smoke_grenade_no_bg.smoke_grenade_no_bg'")   // <- înlocuiește cu calea ta exactă
    );
    if (GrenadeTexObj3.Succeeded())
    {
        Slot5Icon = GrenadeTexObj3.Object;
        WeaponIcon5 = Slot5Icon;
        UE_LOG(LogTemp, Log, TEXT("Grenade icon3 loaded in HUD CDO."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find grenade3 icon texture!"));
    }

    BackpackIcon = nullptr; // Sau încarcă iconul dacă ai unul
    static ConstructorHelpers::FObjectFinder<UTexture2D> BagIconObj(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/bagpack.bagpack'") // <- Înlocuiește cu calea ta reală
    );
    if (BagIconObj.Succeeded())
    {
        BackpackIcon = BagIconObj.Object;
        UE_LOG(LogTemp, Log, TEXT("Am gasit icon de backpack"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Nu am gasit icon de backpack"));
    }


    /* Incarcare imagini gloante */
    //5.56
    static ConstructorHelpers::FObjectFinder<UTexture2D> Bullet556(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/5_56mm.5_56mm'")   // <- înlocuiește cu calea ta exactă
    );
    if (Bullet556.Succeeded())
    {
        Slot556Icon = Bullet556.Object;
        UE_LOG(LogTemp, Log, TEXT("Bullet 556 loaded in HUD CDO."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find bullet556 icon texture!"));
    }

    //7.62
    static ConstructorHelpers::FObjectFinder<UTexture2D> Bullet762(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/7_62mm.7_62mm'")   // <- înlocuiește cu calea ta exactă
    );
    if (Bullet762.Succeeded())
    {
        Slot762Icon = Bullet762.Object;
        UE_LOG(LogTemp, Log, TEXT("Bullet 762 loaded in HUD CDO."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find bullet762 icon texture!"));
    }

    //9mm

    static ConstructorHelpers::FObjectFinder<UTexture2D> Bullet9(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/9mm.9mm'")   // <- înlocuiește cu calea ta exactă
    );
    if (Bullet9.Succeeded())
    {
        Slot9Icon = Bullet9.Object;
        UE_LOG(LogTemp, Log, TEXT("Bullet 9mm loaded in HUD CDO."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find bullet9mm icon texture!"));
    }

    //adaug eu un item de mana
    FInventoryItem AmmoItem;
    AmmoItem.Name = TEXT("5.56mm Ammo");
    AmmoItem.Quantity = 120;
    gloante_556 += AmmoItem.Quantity;
    AmmoItem.Icon = Slot556Icon; // asigură-te că ai încărcat iconul undeva
    InventoryItems.Add(AmmoItem);

    //incarcare arme
    static ConstructorHelpers::FObjectFinder<UTexture2D> AKM(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/akm-image.akm-image'")   // <- înlocuiește cu calea ta exactă
    );
    if (AKM.Succeeded())
    {
        AKM_image = AKM.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon AKM loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find AKM icon texture!"));
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D> VSS(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/vss-icon.vss-icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (VSS.Succeeded())
    {
        VSS_image = VSS.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon VSS loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find VSS icon texture!"));
    }


    static ConstructorHelpers::FObjectFinder<UTexture2D> SCAR(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/scar_icon.scar_icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (SCAR.Succeeded())
    {
        Scar_image = SCAR.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon SCAR loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find SCAR icon texture!"));
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D> M416(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/M416-icon.M416-icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (M416.Succeeded())
    {
        M416_image = M416.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon M416 loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find M416 icon texture!"));
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D> M762(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/M762-icon.M762-icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (M762.Succeeded())
    {
        M762_image = M762.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon M762 loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find M762 icon texture!"));
    }


    static ConstructorHelpers::FObjectFinder<UTexture2D> M249(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/m249-icon.m249-icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (M249.Succeeded())
    {
        M249_image = M249.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon M249 loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find M249 icon texture!"));
    }


    static ConstructorHelpers::FObjectFinder<UTexture2D> AWM(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/awm-icon.awm-icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (AWM.Succeeded())
    {
        AWM_image = AWM.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon AWM loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find AWM icon texture!"));
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D> SKS(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/SKS-icon.SKS-icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (SKS.Succeeded())
    {
        SKS_image = SKS.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon SKS loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find SKS icon texture!"));
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D> Groza(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/groza-icon.groza-icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (Groza.Succeeded())
    {
        Groza_image = Groza.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon Groza loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find Groza icon texture!"));
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D> KAR98K(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/KAR98K-icon.KAR98K-icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (KAR98K.Succeeded())
    {
        KAR98K_image = KAR98K.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon KAR98K loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find KAR98K icon texture!"));
    }


    static ConstructorHelpers::FObjectFinder<UTexture2D> S686(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/S686-icon.S686-icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (S686.Succeeded())
    {
        S686_image = S686.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon S686 loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find S686 icon texture!"));
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D> DP28(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/dp28-icon.dp28-icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (DP28.Succeeded())
    {
        DP28_image = DP28.Object;
        UE_LOG(LogTemp, Log, TEXT("Weapon DP28 loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find DP28 icon texture!"));
    }
    //incarcare sunet arma
    static ConstructorHelpers::FObjectFinder<USoundBase> SwordObj(TEXT("/Script/Engine.SoundWave'/Game/StarterContent/Audio/sword-hit.sword-hit'"));
    if (SwordObj.Succeeded())
    {
        SwordSound = SwordObj.Object;
        UE_LOG(LogTemp, Warning, TEXT("Sword sound loaded successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load sword sound!"));
    }


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
    static ConstructorHelpers::FObjectFinder<USoundBase> WalkingSoundObj(TEXT("/Script/Engine.SoundWave'/Game/StarterContent/Audio/running.running'"));

    if (WalkingSoundObj.Succeeded())
    {
        WalkingSound = WalkingSoundObj.Object;
        UE_LOG(LogTemp, Warning, TEXT("Walking sound loaded successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load walking sound!"));
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> ScopeObj(TEXT("/Script/Engine.SoundWave'/Game/StarterContent/Audio/scope_loud.scope_loud'"));

    if (ScopeObj.Succeeded())
    {
        ScopeSound = ScopeObj.Object;
        UE_LOG(LogTemp, Warning, TEXT("Scope sound loaded successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load scope sound!"));
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> ReloadObj(TEXT("/Script/Engine.SoundWave'/Game/StarterContent/Audio/output_reload.output_reload'"));

    if (ReloadObj.Succeeded())
    {
        ReloadSound = ReloadObj.Object;
        UE_LOG(LogTemp, Warning, TEXT("Reload sound loaded successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load reload sound!"));
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> FallingObj(TEXT("/Script/Engine.SoundWave'/Game/StarterContent/Audio/thump-and-grunt-85606.thump-and-grunt-85606'"));

    if (FallingObj.Succeeded())
    {
        FallingSound = FallingObj.Object;
        UE_LOG(LogTemp, Warning, TEXT("Falling sound loaded successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load falling sound!"));
    }

   /* static ConstructorHelpers::FObjectFinder<USoundBase> PickupObj(TEXT("/Script/Engine.SoundWave'/Game/StarterContent/Audio/put_item-83043.put_item-83043'"));

    if (PickupObj.Succeeded())
    {
        PickupWeaponSound = PickupObj.Object;
        UE_LOG(LogTemp, Warning, TEXT("Pickup sound loaded successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load pickup sound!"));
    }*/
    //scopes
    static ConstructorHelpers::FObjectFinder<UTexture2D> BloodTexObj(TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/blood_nobg.blood_nobg'"));
    if (BloodTexObj.Succeeded())
    {
        BloodTexture = BloodTexObj.Object;
    }
    BloodOverlayEndTime = 0.0f;

    static ConstructorHelpers::FObjectFinder<UTexture2D> Scopex2(TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/scopex2_bun.scopex2_bun'"));
    if (Scopex2.Succeeded())
    {
        Scopex2_image = Scopex2.Object;
    }
    static ConstructorHelpers::FObjectFinder<UTexture2D> Scopex3(TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/scopex3-bun4.scopex3-bun4'"));
    if (Scopex3.Succeeded())
    {
        Scopex3_image = Scopex3.Object;
    }



    //incarcare mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> AKM_Mesh2(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/AKM_47_Skechtfab.AKM_47_Skechtfab'"));

    if (AKM_Mesh2.Succeeded())
    {
        WeaponMesh2->SetStaticMesh(AKM_Mesh2.Object);
        UE_LOG(LogTemp, Log, TEXT("Mesh for AKM loaded"));
    }

    //incarcare mesh-uri consumabile
    static ConstructorHelpers::FObjectFinder<UStaticMesh> DrinkAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/Energy_drink.Energy_drink'"));
    if (DrinkAsset.Succeeded())
    {
        Mesh_Drink = DrinkAsset.Object;
        UE_LOG(LogTemp, Log, TEXT("Energy Drink incarcat cu succes"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Energy Drink mesh!"));
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> PainkillerAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/Painkiller.Painkiller'"));
    if (PainkillerAsset.Succeeded())
    {
        Mesh_Painkiller = PainkillerAsset.Object;
        UE_LOG(LogTemp, Log, TEXT("Painkiller mesh loaded successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Painkiller mesh!"));
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> AdrenalineAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/Adrenaline.Adrenaline'"));
    if (AdrenalineAsset.Succeeded())
    {
        Mesh_Adrenaline = AdrenalineAsset.Object;
        UE_LOG(LogTemp, Log, TEXT("Adrenaline Syringe mesh loaded successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Adrenaline Syringe mesh!"));
    }
    //incarcare icon masina
    
    static ConstructorHelpers::FObjectFinder<UTexture2D> Car(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/car_icon.car_icon'")   // <- înlocuiește cu calea ta exactă
    );
    if (Car.Succeeded())
    {
        Car_image = Car.Object;
        UE_LOG(LogTemp, Log, TEXT("Car image loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find AKM icon texture!"));
    }

    //incarcare textura moneda
    static ConstructorHelpers::FObjectFinder<UTexture2D> Coin(
        TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/coin2.coin2'")   // <- înlocuiește cu calea ta exactă
    );
    if (Coin.Succeeded())
    {
        Coin_image = Coin.Object;
        UE_LOG(LogTemp, Log, TEXT("Coin image loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find Coin texture"));
    }


    //incarcare brate
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> HandMeshFinder(TEXT("/Script/Engine.SkeletalMesh'/Game/FirstPersonArms/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'"));
    if (HandMeshFinder.Succeeded())
    {
        HandMeshAsset = HandMeshFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Hand mesh loaded successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load hand mesh!"));
    }


    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/hands_fps.hands_fps'"));

    if (MeshFinder.Succeeded())
    {
        Hands = MeshFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Hands loaded successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load hands!"));
    }

}

void ACPP_Shooter2HUD::ShowBloodOverlay(float Duration)
{
    BloodOverlayEndTime = GetWorld()->GetTimeSeconds() + Duration;
}
EWeaponType ACPP_Shooter2HUD::GetWeaponTypeFromName(const FString& WeaponName)
{
    if (WeaponName.Contains(TEXT("AKM")))
    {
        return EWeaponType::AKM;
    }
    else if (WeaponName.Contains(TEXT("VSS")))
    {
        return EWeaponType::VSS;
    }
    else if (WeaponName.Contains(TEXT("Scar")))
    {
        return EWeaponType::Scar;
    }
    else if (WeaponName.Contains(TEXT("M416")))
    {
        return EWeaponType::M416;
    }
    else if (WeaponName.Contains(TEXT("M762")))
    {
        return EWeaponType::M762;
    }
    else if (WeaponName.Contains(TEXT("M249")))
    {
        return EWeaponType::M249;
    }
    else if (WeaponName.Contains(TEXT("AWM")))
    {
        return EWeaponType::AWM;
    }
    else if (WeaponName.Contains(TEXT("SKS")))
    {
        return EWeaponType::SKS;
    }
    else if (WeaponName.Contains(TEXT("Groza")))
    {
        return EWeaponType::Groza;
    }
    else if (WeaponName.Contains(TEXT("KAR98K")))
    {
        return EWeaponType::KAR98K;
    }
    else if (WeaponName.Contains(TEXT("S686")))
    {
        return EWeaponType::S686;
    }
    else if (WeaponName.Contains(TEXT("DP28")))
    {
        return EWeaponType::DP28;
    }


    return EWeaponType::None; // Sau ce ai tu ca default
}
void ACPP_Shooter2HUD::ReloadWeapon(EBulletType BulletType, int32 Slot)
{

    UE_LOG(LogTemp, Log, TEXT("Doresc sa dau reload la arma"));
    if (Slot == 1)
    {
        int32 AmmoNeeded = MaxAmmoInMag1 - AmmoInMag1;

        if (AmmoNeeded <= 0)
        {
            UE_LOG(LogTemp, Log, TEXT("Magazia e deja plină"));
            return;
        }
        int32 gloante_arma1 = 0;
        if(BulletType == EBulletType::BT_556)
        {
            gloante_arma1 = gloante_556;
        }
        if (BulletType == EBulletType::BT_762)
        {
            gloante_arma1 = gloante_762;
        }
        if (BulletType == EBulletType::BT_9MM)
        {
            gloante_arma1 = gloante_9;
        }
        if (gloante_arma1 <= 0)
        {
            return;
        }
        int32 AmmoToReload = FMath::Min(AmmoNeeded, gloante_arma1);
        AmmoInMag1 += AmmoToReload;
        gloante_arma1 -= AmmoToReload;
    }
    if (Slot == 2)
    {
        int32 AmmoNeeded2 = MaxAmmoInMag2 - AmmoInMag2;

        if (AmmoNeeded2 <= 0)
        {
            UE_LOG(LogTemp, Log, TEXT("Magazia e deja plină"));
            return;
        }
        int32 gloante_arma2 = 0;
        if (BulletType == EBulletType::BT_556)
        {
            gloante_arma2 = gloante_556;
        }
        if (BulletType == EBulletType::BT_762)
        {
            gloante_arma2 = gloante_762;
        }
        if (BulletType == EBulletType::BT_9MM)
        {
            gloante_arma2 = gloante_9;
        }
        if (gloante_arma2 <= 0)
        {
            return;
        }
        int32 AmmoToReload2 = FMath::Min(AmmoNeeded2, gloante_arma2);
        AmmoInMag2 += AmmoToReload2;
        gloante_arma2 -= AmmoToReload2;
    }
}
void ACPP_Shooter2HUD::ViewWithScope()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    APawn* Pawn = PC->GetPawn();
    if (!Pawn) return;

    APlayerCameraManager* CameraManager = PC->PlayerCameraManager;

    if (!bIsScoped)
    {
        bIsScoped = true;

        if (CameraManager)
        {
            CameraManager->SetFOV(20.f); // Zoom in
        }
    }
    else
    {
        bIsScoped = false;

        if (CameraManager)
        {
            CameraManager->SetFOV(90.f); // Zoom out (sau DefaultFOV dacă vrei să fie flexibil)
        }
    }
}


void ACPP_Shooter2HUD::AddWeaponToInventory(AWeapon* Weapon)
{
    if (!Weapon) return;

    UE_LOG(LogTemp, Log, TEXT("Gaseste arma"));
    FInventoryItem WeaponItem;
    WeaponItem.Name = Weapon->WeaponName;
    WeaponItem.bIsWeapon = true;
    WeaponItem.Quantity = 1;

    if (WeaponItem.Name.Contains(TEXT("AKM")))
    {
        WeaponItem.Icon = AKM_image;
        gloante_762 += 30;
    }
    else if (WeaponItem.Name.Contains(TEXT("VSS")))
    {
        WeaponItem.Icon = VSS_image;
        gloante_9 += 20;
    }
    else if (WeaponItem.Name.Contains(TEXT("Scar")))
    {
        WeaponItem.Icon = Scar_image;
        gloante_556 += 30;
    }
    else if (WeaponItem.Name.Contains(TEXT("M416")))
    {
        WeaponItem.Icon = M416_image;
        gloante_556 += 30;
    }
    else if (WeaponItem.Name.Contains(TEXT("M762")))
    {
        WeaponItem.Icon = M762_image;
        gloante_762 += 30;
    }
    else if (WeaponItem.Name.Contains(TEXT("M249")))
    {
        WeaponItem.Icon = M249_image;
        gloante_556 += 100;
    }
    else if (WeaponItem.Name.Contains(TEXT("AWM")))
    {
        WeaponItem.Icon = AWM_image;
        gloante_9 += 30;
    }
    else if (WeaponItem.Name.Contains(TEXT("SKS")))
    {
        WeaponItem.Icon = SKS_image;
        gloante_762 += 10;
    }
    else if (WeaponItem.Name.Contains(TEXT("Groza")))
    {
        WeaponItem.Icon = Groza_image;
        gloante_762 += 30;
    }
    else if (WeaponItem.Name.Contains(TEXT("KAR98K")))
    {
        WeaponItem.Icon = KAR98K_image;
        gloante_762 += 5;
    }
    else if (WeaponItem.Name.Contains(TEXT("S686")))
    {
        WeaponItem.Icon = S686_image;
        gloante_762 += 30;
    }
    else if (WeaponItem.Name.Contains(TEXT("DP28")))
    {
        WeaponItem.Icon = DP28_image;
        gloante_762 += 47;
    }


    WeaponItem.BulletType = Weapon->BulletType;
    WeaponItem.AmmoInMagazine = Weapon->AmmoInMagazine;
    WeaponItem.FireRate = Weapon->FireRate;
    WeaponItem.Damage = Weapon->Damage;

    // Pozitia pentru drop
    FVector PlayerLocation = GetOwningPlayerController()->GetPawn()->GetActorLocation();
    FVector DropOffset(150.f, 0.f, -50.0f);
    FVector DropLocation = PlayerLocation + DropOffset;
    FRotator DropRotation = FRotator(-0.f, 0.f, -90.f);

    ACPP_Shooter2GameMode* GameMode = Cast<ACPP_Shooter2GameMode>(GetWorld()->GetAuthGameMode());

    if (SelectedWeaponSlot == 1)
    {
        if (WeaponItem.Name.Contains("DP28"))
        {
            AmmoInMag1 = 47;
            MaxAmmoInMag1 = 47;
        }
        if (WeaponItem.Name.Contains("VSS"))
        {
            AmmoInMag1 = 20;
            MaxAmmoInMag1 = 20;
        }
        if (WeaponItem.Name.Contains("KAR98K"))
        {
            AmmoInMag1 = 5;
            MaxAmmoInMag1 = 5;
        }
        if (WeaponItem.Name.Contains("M249"))
        {
            AmmoInMag1 = 100;
            MaxAmmoInMag1 = 100;
        }
        if (WeaponItem.Name.Contains("SKS"))
        {
            AmmoInMag1 = 10;
            MaxAmmoInMag1 = 10;
        }
        if (WeaponItem.Name.Contains("AKM") || WeaponItem.Name.Contains("Scar") || WeaponItem.Name.Contains("M416")
            || WeaponItem.Name.Contains("M762") || WeaponItem.Name.Contains("Groza") || WeaponItem.Name.Contains("S686"))
        {
            AmmoInMag1 = 30;
            MaxAmmoInMag1 = 30;
        }


        if (WeaponSlot1 != TEXT("Empty"))
        {
            UE_LOG(LogTemp, Warning, TEXT("Slotul 1 este deja ocupat de: %s. Drop arma pe hartă."), *WeaponSlot1);

            if (GameMode)
            {
                EWeaponType WeaponTypeToDrop = GetWeaponTypeFromName(WeaponSlot1);
                if (WeaponTypeToDrop != EWeaponType::None)
                {
                    GameMode->SpawnWeapon(DropLocation, DropRotation, WeaponTypeToDrop);
                    UE_LOG(LogTemp, Log, TEXT("Spawned weapon of type %d at location %s"), (int32)WeaponTypeToDrop, *DropLocation.ToString());
                }
            }

            // Șterge arma veche din inventar
            for (int32 i = 0; i < InventoryItems.Num(); i++)
            {
                if (InventoryItems[i].Name == WeaponSlot1)
                {
                    UE_LOG(LogTemp, Log, TEXT("Removed old weapon %s from InventoryItems"), *WeaponSlot1);
                    InventoryItems.RemoveAt(i);
                    break;
                }
            }
        }

        // Seteaza noua arma in slot 1
        WeaponSlot1 = Weapon->WeaponName;
        WeaponIcon1 = WeaponItem.Icon;
        UE_LOG(LogTemp, Log, TEXT("WeaponSlot1 set to %s"), *WeaponSlot1);
    }
    else if (SelectedWeaponSlot == 2)
    {
        if (WeaponSlot2 != TEXT("Empty"))
        {
            if (WeaponItem.Name.Contains("DP28"))
            {
                AmmoInMag2 = 47;
                MaxAmmoInMag2 = 47;
            }
            if (WeaponItem.Name.Contains("VSS"))
            {
                AmmoInMag2 = 20;
                MaxAmmoInMag2 = 20;
            }
            if (WeaponItem.Name.Contains("KAR98K"))
            {
                AmmoInMag2 = 5;
                MaxAmmoInMag2 = 5;
            }
            if (WeaponItem.Name.Contains("M249"))
            {
                AmmoInMag2 = 100;
                MaxAmmoInMag2 = 100;
            }
            if (WeaponItem.Name.Contains("SKS"))
            {
                AmmoInMag2 = 10;
                MaxAmmoInMag2 = 10;
            }
            if (WeaponItem.Name.Contains("AKM") || WeaponItem.Name.Contains("Scar") || WeaponItem.Name.Contains("M416")
                || WeaponItem.Name.Contains("M762") || WeaponItem.Name.Contains("Groza") || WeaponItem.Name.Contains("S686"))
            {
                AmmoInMag2 = 30;
                MaxAmmoInMag2 = 30;
            }

            UE_LOG(LogTemp, Warning, TEXT("Slotul 2 este deja ocupat de: %s. Drop arma pe hartă."), *WeaponSlot2);

            if (GameMode)
            {
                EWeaponType WeaponTypeToDrop = GetWeaponTypeFromName(WeaponSlot2);
                if (WeaponTypeToDrop != EWeaponType::None)
                {
                    GameMode->SpawnWeapon(DropLocation, DropRotation, WeaponTypeToDrop);
                }
            }

            // Șterge arma veche din inventar
            for (int32 i = 0; i < InventoryItems.Num(); i++)
            {
                if (InventoryItems[i].Name == WeaponSlot2)
                {
                    InventoryItems.RemoveAt(i);
                    break;
                }
            }
        }

        // Seteaza noua arma in slot 2
        WeaponSlot2 = Weapon->WeaponName;
        WeaponIcon2 = WeaponItem.Icon;
    }

    InventoryItems.Add(WeaponItem);

    UE_LOG(LogTemp, Warning, TEXT("Arma %s a fost adăugată în inventar!"), *WeaponItem.Name);
}

TArray<AProjectiles*> ACPP_Shooter2HUD::SpawnShotgunProjectiles(
    FVector SpawnOrigin,
    FVector ShootDirection,
    FRotator BaseRotation,
    const FActorSpawnParameters& SpawnParams,
    int32 NumProjectiles,
    float SpreadAngleDegrees)
{
    TArray<AProjectiles*> SpawnedProjectiles;

    for (int32 i = 0; i < NumProjectiles; ++i)
    {
        FVector RandomDir = FMath::VRandCone(ShootDirection, FMath::DegreesToRadians(SpreadAngleDegrees));
        FVector SpawnLocation = SpawnOrigin + RandomDir * 100.f;
        FRotator SpawnRotation = RandomDir.Rotation();

        AProjectiles* NewProj = GetWorld()->SpawnActor<AProjectiles>(ProjectilesClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (NewProj)
        {
            SpawnedProjectiles.Add(NewProj);
        }
    }

    return SpawnedProjectiles;
}


void ACPP_Shooter2HUD::SpawnBullet()
{
    // Verificări mai robuste pentru ProjectilesClass
    if (!ProjectilesClass || !IsValid(ProjectilesClass))
    {
        UE_LOG(LogTemp, Warning, TEXT("ProjectileClass not set or invalid!"));
        return;
    }

    // Verificare ammo
    if ((SelectedWeaponSlot == 1 && AmmoInMag1 <= 0) || (SelectedWeaponSlot == 2 && AmmoInMag2 <= 0))
    {
        return;
    }

    UWorld* World = GetWorld();
    APlayerController* PC = GetOwningPlayerController();
    if (!World || !IsValid(World) || !PC || !IsValid(PC) || !PC->PlayerCameraManager || !IsValid(PC->PlayerCameraManager))
    {
        UE_LOG(LogTemp, Warning, TEXT("World, PlayerController or CameraManager invalid!"));
        return;
    }

    APawn* Pawn = PC->GetPawn();
    if (!Pawn || !IsValid(Pawn))
    {
        UE_LOG(LogTemp, Warning, TEXT("Pawn invalid!"));
        return;
    }

    FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();

    int32 ScreenSizeX, ScreenSizeY;
    PC->GetViewportSize(ScreenSizeX, ScreenSizeY);

    float ScreenX = ScreenSizeX / 2.f;
    float ScreenY = ScreenSizeY / 2.f;

    FVector WorldLocation, WorldDirection;
    bool bSuccess = PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLocation, WorldDirection);
    if (!bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to deproject screen position to world!"));
        return;
    }

    FVector ShootDirection = WorldDirection.GetSafeNormal();
    FVector SpawnLocation = CameraLocation + ShootDirection * 100.f;
    FRotator SpawnRotation = ShootDirection.Rotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Pawn;

    if (IsShotgun)
    {
        AProjectiles* MainProjectile = World->SpawnActor<AProjectiles>(ProjectilesClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (MainProjectile && IsValid(MainProjectile))
        {
            AllProjectiles.Add(MainProjectile);

            // Verificare sigură pentru CollisionComp
            if (MainProjectile->CollisionComp && IsValid(MainProjectile->CollisionComp))
            {
                MainProjectile->CollisionComp->IgnoreActorWhenMoving(Pawn, true);
            }
        }

        TArray<AProjectiles*> ExtraProjectiles = SpawnShotgunProjectiles(CameraLocation, ShootDirection, SpawnRotation, SpawnParams, 4, 20.f);
        for (AProjectiles* Projectile : ExtraProjectiles)
        {
            if (Projectile && IsValid(Projectile))
            {
                // Verificare sigură pentru CollisionComp
                if (Projectile->CollisionComp && IsValid(Projectile->CollisionComp))
                {
                    Projectile->CollisionComp->IgnoreActorWhenMoving(Pawn, true);
                }
                AllProjectiles.Add(Projectile);
            }
        }
    }
    else
    {
        AProjectiles* SingleProjectile = World->SpawnActor<AProjectiles>(ProjectilesClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (SingleProjectile && IsValid(SingleProjectile))
        {
            // Verificare sigură pentru CollisionComp
            if (SingleProjectile->CollisionComp && IsValid(SingleProjectile->CollisionComp))
            {
                SingleProjectile->CollisionComp->IgnoreActorWhenMoving(Pawn, true);
            }
            AllProjectiles.Add(SingleProjectile);
        }
    }

    // Curățare pointeri invalizi în listă
    for (int32 i = AllProjectiles.Num() - 1; i >= 0; i--)
    {
        if (!IsValid(AllProjectiles[i]))
        {
            AllProjectiles.RemoveAt(i);
        }
    }

    if (AllProjectiles.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid projectiles spawned!"));
        return;
    }

    // Logica pentru ammo
    FString Nume1Arma = GetWeaponInSlot(1);
    FString Nume2Arma = GetWeaponInSlot(2);
    ACPP_Shooter2GameMode* GM15 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));

    // Verificare GameMode
    if (!GM15 || !IsValid(GM15))
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode invalid!"));
        return;
    }

    if (SelectedWeaponSlot == 1 && AmmoInMag1 > 0)
    {
        AmmoInMag1--;
        EBulletType tip1 = Match_Weapon_with_bullets(Nume1Arma);
        if (tip1 == EBulletType::BT_556 && gloante_556 > 0)
        {
            GM15->SetBullet556(gloante_556 - 1);
            gloante_556--;
        }
        else if (tip1 == EBulletType::BT_762 && gloante_762 > 0)
        {
            GM15->SetBullet762(gloante_762 - 1);
            gloante_762--;
        }
        else if (tip1 == EBulletType::BT_9MM && gloante_9 > 0)
        {
            GM15->SetBullet9(gloante_9 - 1);
            gloante_9--;
        }
    }
    else if (SelectedWeaponSlot == 2 && AmmoInMag2 > 0)
    {
        AmmoInMag2--;
        EBulletType tip2 = Match_Weapon_with_bullets(Nume2Arma);
        if (tip2 == EBulletType::BT_556 && gloante_556 > 0)
        {
            GM15->SetBullet556(gloante_556 - 1);
            gloante_556--;
        }
        else if (tip2 == EBulletType::BT_762 && gloante_762 > 0)
        {
            GM15->SetBullet762(gloante_762 - 1);
            gloante_762--;
        }
        else if (tip2 == EBulletType::BT_9MM && gloante_9 > 0)
        {
            GM15->SetBullet9(gloante_9 - 1);
            gloante_9--;
        }
    }

    // Sunet cu verificare
    if (FireSound && IsValid(FireSound))
    {
        float VolumeMultiplier = VolumeLevel / 10.0f;
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, Pawn->GetActorLocation(), VolumeMultiplier);
    }

    // Particule de fum - cu verificări îmbunătățite
    for (AProjectiles* Projectile : AllProjectiles)
    {
        if (!IsValid(Projectile)) continue;

        // Verificare mai sigură pentru DustParticles
        if (!Projectile->DustParticles || !IsValid(Projectile->DustParticles)) continue;

        FVector SmokeLocation = Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 150.f;
        FRotator SmokeRotation = Pawn->GetActorRotation();

        UParticleSystemComponent* SmokePSC = UGameplayStatics::SpawnEmitterAtLocation(World, Projectile->DustParticles, SmokeLocation, SmokeRotation);
        if (SmokePSC && IsValid(SmokePSC))
        {
            FVector BaseDirection = ShootDirection;
            FVector RandomSpread = FVector(
                FMath::FRandRange(-0.5f, 0.5f),
                FMath::FRandRange(-0.5f, 0.5f),
                FMath::FRandRange(-0.2f, 0.5f)
            );
            FVector ExplosionDirection = (BaseDirection + RandomSpread).GetSafeNormal();
            FVector Velocity = ExplosionDirection * 4000.f;
            SmokePSC->SetVectorParameter(FName("Velocity"), Velocity);

            // Folosește TWeakObjectPtr pentru siguranță
            TWeakObjectPtr<UParticleSystemComponent> WeakSmokePSC = SmokePSC;
            FTimerHandle TimerHandle;
            World->GetTimerManager().SetTimer(TimerHandle, [WeakSmokePSC]()
                {
                    if (WeakSmokePSC.IsValid())
                    {
                        WeakSmokePSC->DeactivateSystem();
                        WeakSmokePSC->DestroyComponent();
                    }
                }, 1.f, false);
        }
    }

    ApplyRecoil();
}









bool ACPP_Shooter2HUD::IsInventoryOpen()
{
    return bIsInventoryVisible;
}

void ACPP_Shooter2HUD::ThrowSmoke()
{
    if (!MolotovClass) {
        UE_LOG(LogTemp, Warning, TEXT("Nu a gasit clasa Smoke"));
        return;
    }

    if (SmokeGrenades > 0)
    {
        ACPP_Shooter2GameMode* GM12 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
        if (GM12)
        {
            GM12->SetSmokeGrenades(SmokeGrenades - 1);
            int NrGrenade = GM12->GetTotalGrenades();
            GM12->SetTotalGrenades(NrGrenade + 1);
        }
        SmokeGrenades--;
    }
    else {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("A gasit clasa Smoke"));
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;

    APawn* Pawn = PC->GetPawn();
    if (!Pawn) return;
    UE_LOG(LogTemp, Warning, TEXT("A gasit Pawn"));

    FVector SpawnLocation = Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 100.f + FVector(0, 0, 50.f);
    FRotator SpawnRotation = PC->GetControlRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Pawn;
    SpawnParams.Instigator = Pawn;

    ASmoke_Grenade* SpawnedGrenade = World->SpawnActor<ASmoke_Grenade>(SmokeClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (SpawnedGrenade)
    {
        FVector ThrowDirection = SpawnRotation.Vector();
        float ThrowStrength = 1500.f;

        UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(SpawnedGrenade->GetRootComponent());
        if (RootComp && RootComp->IsSimulatingPhysics())
        {
            RootComp->AddImpulse(ThrowDirection * ThrowStrength, NAME_None, true);
        }
    }
}



void ACPP_Shooter2HUD::AddBulletToInventory(EBulletType BulletType, int32 QuantityToAdd)
{
    FString BulletName;
    UTexture2D* BulletIcon = nullptr;

    switch (BulletType)
    {
    case EBulletType::BT_556:
        BulletName = TEXT("5.56mm Ammo");
        BulletIcon = Slot556Icon;
        gloante_556 = gloante_556 + QuantityToAdd;
        break;
    case EBulletType::BT_762:
        BulletName = TEXT("7.62mm Ammo");
        BulletIcon = Slot762Icon;
        gloante_762 = gloante_762 + QuantityToAdd;
        break;
    case EBulletType::BT_9MM:
        BulletName = TEXT("9mm Ammo");
        BulletIcon = Slot9Icon;
        gloante_9 = gloante_9 + QuantityToAdd;
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Tip de glont necunoscut."));
        return; // Iesim daca nu recunoastem glontul
    }

    // Verifici dacă ai deja acest tip de glonț în inventar
    bool bFound = false;
    for (FInventoryItem& Item : InventoryItems)
    {
        if (Item.Name == BulletName)
        {
            Item.Quantity += QuantityToAdd;
            bFound = true;
            break;
        }
    }

    // Dacă nu există, îl adaugi nou
    if (!bFound)
    {
        FInventoryItem NewItem;
        NewItem.Name = BulletName;
        NewItem.Quantity = QuantityToAdd;
        NewItem.Icon = BulletIcon;

        InventoryItems.Add(NewItem);
    }

    UE_LOG(LogTemp, Warning, TEXT("Ai adaugat %d %s in inventar."), QuantityToAdd, *BulletName);
}


void ACPP_Shooter2HUD::ThrowMolotov()
{
    if (!MolotovClass) {
        UE_LOG(LogTemp, Warning, TEXT("Nu a gasit clasa Molotov"));
        return;
    }

    if (Molotovs > 0)
    {
        ACPP_Shooter2GameMode* GM11 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
        if (GM11)
        {
            GM11->SetMolotovs(Molotovs - 1);
            int NrGrenade = GM11->GetTotalGrenades();
            GM11->SetTotalGrenades(NrGrenade + 1);
        }
        Molotovs--;
    }
    else {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("A gasit clasa Molotov"));
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;

    APawn* Pawn = PC->GetPawn();
    if (!Pawn) return;
    UE_LOG(LogTemp, Warning, TEXT("A gasit Pawn"));

    FVector SpawnLocation = Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 100.f + FVector(0, 0, 50.f);
    FRotator SpawnRotation = PC->GetControlRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Pawn;
    SpawnParams.Instigator = Pawn;

    AMolotov_Cocktail* SpawnedMolotov = World->SpawnActor<AMolotov_Cocktail>(MolotovClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (SpawnedMolotov)
    {
        FVector ThrowDirection = SpawnRotation.Vector();
        float ThrowStrength = 1500.f;

        UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(SpawnedMolotov->GetRootComponent());
        if (RootComp && RootComp->IsSimulatingPhysics())
        {
            RootComp->AddImpulse(ThrowDirection * ThrowStrength, NAME_None, true);
        }
    }
}

void ACPP_Shooter2HUD::ThrowGrenade()
{
    if (!GrenadeClass) {
        UE_LOG(LogTemp, Warning, TEXT("Nu a gasit clasa Grenade"));
        return;
    }

    if (Grenades > 0)
    {
        ACPP_Shooter2GameMode* GM10 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
        if (GM10)
        {
            GM10->SetGrenades(Grenades - 1);
            int NrGrenade = GM10->GetTotalGrenades();
            GM10->SetTotalGrenades(NrGrenade + 1);
        }
        Grenades--;
    }
    else {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("A gasit clasa Grenade"));
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;

    APawn* Pawn = PC->GetPawn();
    if (!Pawn) return;
    UE_LOG(LogTemp, Warning, TEXT("A gasit Pawn"));


    FVector SpawnLocation = Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 100.f + FVector(0, 0, 50.f);
    FRotator SpawnRotation = PC->GetControlRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Pawn;
    SpawnParams.Instigator = Pawn;

    AGrenade* SpawnedGrenade = World->SpawnActor<AGrenade>(GrenadeClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (SpawnedGrenade)
    {
        FVector ThrowDirection = SpawnRotation.Vector();
        float ThrowStrength = 1500.f;

        UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(SpawnedGrenade->GetRootComponent());
        if (RootComp && RootComp->IsSimulatingPhysics())
        {
            RootComp->AddImpulse(ThrowDirection * ThrowStrength, NAME_None, true);
        }
    }
}

void ACPP_Shooter2HUD::SelectGrenade()
{
    SelectedWeaponSlot = 3;
    ThrowGrenade();
    slot12 = false;
    slot345 = true;
}

void ACPP_Shooter2HUD::SelectMolotov_Cocktail()
{
    SelectedWeaponSlot = 4;
    UE_LOG(LogTemp, Log, TEXT("Gadget selected."));
    ThrowMolotov();
    slot12 = false;
    slot345 = true;
    // Poți adăuga aici logica pentru activarea gadgetului, dacă e cazul
}
void ACPP_Shooter2HUD::SelectSlot1()
{
    SelectedWeaponSlot = 1;
    slot12 = true;
    slot345 = false;
}

void ACPP_Shooter2HUD::SelectSlot2()
{
    SelectedWeaponSlot = 2;
    slot12 = true;
    slot345 = false;
}
void ACPP_Shooter2HUD::SelectSlot5()
{
    SelectedWeaponSlot = 5;
    ThrowSmoke();
    slot12 = false;
    slot345 = true;
}

void ACPP_Shooter2HUD::SetGrenade(const FString& Name, UTexture2D* Icon)
{
    GrenadeSlot = Name;
    GrenadeIcon = Icon;
}


void ACPP_Shooter2HUD::InitMinimapCapture()
{
    if (!GetWorld()) return;

    // Creează render target-ul
    MiniMapRenderTarget = NewObject<UTextureRenderTarget2D>(this);
    MiniMapRenderTarget->InitAutoFormat(1024, 1024);
    MiniMapRenderTarget->ClearColor = FLinearColor::Black;
    MiniMapRenderTarget->UpdateResourceImmediate(true);

    // Creează actorul SceneCapture2D
    SceneCaptureActor = GetWorld()->SpawnActor<ASceneCapture2D>(ASceneCapture2D::StaticClass());
    if (!SceneCaptureActor) return;

    // Setări captură
    MiniMapCapture = SceneCaptureActor->GetCaptureComponent2D();
    if (!MiniMapCapture) return;

    MiniMapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
    MiniMapCapture->OrthoWidth = 2000.f; // Acoperă zona dorită (ex: 20m)
    MiniMapCapture->TextureTarget = MiniMapRenderTarget;
    MiniMapCapture->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDRNoAlpha;
    MiniMapCapture->bCaptureEveryFrame = true;
    MiniMapCapture->bCaptureOnMovement = false;

    // Activăm doar ce ne trebuie
    FEngineShowFlags& ShowFlags = MiniMapCapture->ShowFlags;

    ShowFlags.SetWidgetComponents(false);     // elimină UI 3D (UWidgetComponent)
    ShowFlags.SetParticles(false);            // elimină particle systems
    ShowFlags.SetDecals(false);               // elimină decals
    ShowFlags.SetTranslucency(false);         // elimină efecte transparente (ex: săgeți)
    ShowFlags.SetNiagara(false);              // elimină efecte Niagara
    ShowFlags.SetMotionBlur(false);           // elimină motion blur

    ShowFlags.SetStaticMeshes(true);
    ShowFlags.SetSkeletalMeshes(true);
    ShowFlags.SetLandscape(true);
    ShowFlags.SetLighting(true);
    ShowFlags.SetPostProcessing(true);
    ShowFlags.SetFog(true);
    ShowFlags.SetDirectionalLights(true);
    ShowFlags.SetPointLights(true);
    ShowFlags.SetVolumetricFog(true);
    ShowFlags.SetDeferredLighting(true);
    ShowFlags.SetBloom(true);
    ShowFlags.SetNaniteMeshes(true);
}
void ACPP_Shooter2HUD::UpdateMiniMapCamera()
{
    if (!SceneCaptureActor || !MiniMapCapture || !GetOwningPawn()) return;

    FVector PlayerLocation = GetOwningPawn()->GetActorLocation();
    FVector CameraLocation = PlayerLocation + FVector(0.f, 0.f, 50000.f); // Sus deasupra playerului
    FRotator CameraRotation = FRotator(-90.f, 0.f, 0.f); // Privește în jos (axa Z)

    SceneCaptureActor->SetActorLocation(CameraLocation);
    SceneCaptureActor->SetActorRotation(CameraRotation);
}
void ACPP_Shooter2HUD::DrawMiniMap()
{
    if (!Canvas || !MiniMapRenderTarget) return;

    const float MapSize = 300.f;
    const float Padding = 20.f;

    // Poziționare: colț dreapta sus
    FVector2D MapPos(Canvas->ClipX - MapSize - Padding, Padding);

    // Fundal negru (pentru caz de fallback)
    FLinearColor BackgroundColor = FLinearColor::Black;
    FCanvasTileItem Background(MapPos, FVector2D(MapSize, MapSize), BackgroundColor);
    Background.BlendMode = SE_BLEND_Opaque;
    Canvas->DrawItem(Background);

    // Desenăm minimap-ul
    FCanvasTileItem MinimapTile(MapPos, MiniMapRenderTarget->Resource, FVector2D(MapSize, MapSize), FLinearColor::White);
    MinimapTile.BlendMode = SE_BLEND_Opaque;
    Canvas->DrawItem(MinimapTile);
}






void ACPP_Shooter2HUD::BeginPlay()
{
    Super::BeginPlay();
    WeaponIcons.Empty();           // golim array-ul dacă e nevoie
    WeaponIcons.Add(AWM_image);
    WeaponIcons.Add(S686_image);
    WeaponIcons.Add(M249_image);
    WeaponIcons.Add(Scar_image);
    WeaponIcons.Add(M416_image);
    WeaponIcons.Add(DP28_image);
    WeaponIcons.Add(M762_image);
    WeaponIcons.Add(KAR98K_image);
    WeaponIcons.Add(AKM_image);    // sau Rocket_image, după cum ai
    ACharacter* PlayerCharacter2 = Cast<ACharacter>(GetOwningPlayerController()->GetPawn());
    bShowMainMenu = true;
    //aici fac partea in care refac healthul in functie de level
    ACPP_Shooter2GameMode* GM5 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
    if (GM5)
    {
        MaxHealth = MaxHealth + GM5->CurrentHealthLevel - 1;
        CurrentHealth = CurrentHealth + GM5->CurrentHealthLevel - 1;
        WeaponSlot1 = GM5->GetWeaponName1();
        WeaponSlot2 = GM5->GetWeaponName2();
        Grenades = GM5->GetGrenades();
        Molotovs = GM5->GetMolotovs();
        SmokeGrenades = GM5->GetSmokeGrenades();

        gloante_556 = GM5->GetBullet556();
        gloante_762 = GM5->GetBullet762();
        gloante_9 = GM5->GetBullet9();

        UE_LOG(LogTemp, Log, TEXT("WeaponSlot1: %s"), *WeaponSlot1);
        UE_LOG(LogTemp, Log, TEXT("WeaponSlot2: %s"), *WeaponSlot2);

            GM5->SetWeaponName1(WeaponSlot1);

            if (WeaponSlot1.Contains(TEXT("AKM"))) {
                WeaponIcon1 = AKM_image;
            }
            if (WeaponSlot1.Contains(TEXT("VSS"))) {
                WeaponIcon1 = VSS_image;
            }
            if (WeaponSlot1.Contains(TEXT("Scar"))) {
                WeaponIcon1 = Scar_image;
            }
            if (WeaponSlot1.Contains(TEXT("M416"))) {
                WeaponIcon1 = M416_image;
            }
            if (WeaponSlot1.Contains(TEXT("M762"))) {
                WeaponIcon1 = M762_image;
            }
            if (WeaponSlot1.Contains(TEXT("M249"))) {
                WeaponIcon1 = M249_image;
            }
            if (WeaponSlot1.Contains(TEXT("AWM"))) {
                WeaponIcon1 = AWM_image;
            }
            if (WeaponSlot1.Contains(TEXT("SKS"))) {
                WeaponIcon1 = SKS_image;
            }
            if (WeaponSlot1.Contains(TEXT("Groza"))) {
                WeaponIcon1 = Groza_image;
            }
            if (WeaponSlot1.Contains(TEXT("KAR98K"))) {
                WeaponIcon1 = KAR98K_image;
            }
            if (WeaponSlot1.Contains(TEXT("S686"))) {
                WeaponIcon1 = S686_image;
            }
            if (WeaponSlot1.Contains(TEXT("DP28"))) {
                WeaponIcon1 = DP28_image;
            }
            if (WeaponSlot2.Contains(TEXT("AKM"))) {
                WeaponIcon2 = AKM_image;
            }
            if (WeaponSlot2.Contains(TEXT("VSS"))) {
                WeaponIcon2 = VSS_image;
            }
            if (WeaponSlot2.Contains(TEXT("Scar"))) {
                WeaponIcon2 = Scar_image;
            }
            if (WeaponSlot2.Contains(TEXT("M416"))) {
                WeaponIcon2 = M416_image;
            }
            if (WeaponSlot2.Contains(TEXT("M762"))) {
                WeaponIcon2 = M762_image;
            }
            if (WeaponSlot2.Contains(TEXT("M249"))) {
                WeaponIcon2 = M249_image;
            }
            if (WeaponSlot2.Contains(TEXT("AWM"))) {
                WeaponIcon2 = AWM_image;
            }
            if (WeaponSlot2.Contains(TEXT("SKS"))) {
                WeaponIcon2 = SKS_image;
            }
            if (WeaponSlot2.Contains(TEXT("Groza"))) {
                WeaponIcon2 = Groza_image;
            }
            if (WeaponSlot2.Contains(TEXT("KAR98K"))) {
                WeaponIcon2 = KAR98K_image;
            }
            if (WeaponSlot2.Contains(TEXT("S686"))) {
                WeaponIcon2 = S686_image;
            }
            if (WeaponSlot2.Contains(TEXT("DP28"))) {
                WeaponIcon2 = DP28_image;
            }
    }
    //OpenMainMenu();
    /*APlayerController* PC152 = GetOwningPlayerController();
    if (PC152)
    {
        PC152->bShowMouseCursor = true;
    }
    if (PC152)
    {
        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC152->SetInputMode(InputMode);
    }*/
    if (PlayerCharacter2)
    {
        LastPlayerLocation = PlayerCharacter2->GetActorLocation();
        LastVelocity = FVector::ZeroVector;
    }
    InitMinimapCapture();
    LastLocation2 = GetActorLocation();
    /*if (APlayerController* PC = GetOwningPlayerController())
    {
        PC->bShowMouseCursor = false;
    }*/
    //sunet
    if (WalkingSound)
    {
        // Creează componenta manual fără să pornească sunetul
        WalkingAudioComponent = NewObject<UAudioComponent>(this);
        if (WalkingAudioComponent)
        {
            WalkingAudioComponent->SetSound(WalkingSound);
            WalkingAudioComponent->bAutoActivate = false;
            WalkingAudioComponent->bIsUISound = false;

            // Setează volumul inițial
            if (VolumeLevel > 0)
            {
                float CurrentVolume = VolumeLevel / 10.0f;
                WalkingAudioComponent->SetVolumeMultiplier(CurrentVolume);
            }
            else
            {
                // La volum 0, oprește sunetul complet
                WalkingAudioComponent->Stop();
            }

            // Înregistrează componenta în world (important!)
            WalkingAudioComponent->RegisterComponent();
        }
    }
    //test
    if (HandMeshAsset)
    {
        USkeletalMeshComponent* TempHands = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("TempHands"));
        TempHands->RegisterComponent();
        TempHands->SetSkeletalMesh(HandMeshAsset);
        TempHands->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        TempHands->SetCastShadow(true);
        TempHands->SetVisibility(true);
        TempHands->SetHiddenInGame(false);

        // Atașează la root component
        TempHands->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

        // Poziționează în fața camerei
        TempHands->SetRelativeLocation(FVector(1600.f, 1200.f, 150.f));
        TempHands->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

        AddInstanceComponent(TempHands);

        UE_LOG(LogTemp, Warning, TEXT("Hand mesh spawned and visible."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Hand mesh was null at BeginPlay."));
    }
}

void ACPP_Shooter2HUD::SpawnOrUpdateVisualWeapon(EWeaponType WeaponType, float SwayAmplitude, float SwaySpeed)
{
    FString SelectedWeaponName;

    if (SelectedWeaponSlot == 1)
    {
        SelectedWeaponName = WeaponSlot1;
    }
    else if (SelectedWeaponSlot == 2)
    {
        SelectedWeaponName = WeaponSlot2;
    }

    ACharacter* PlayerCharacter = Cast<ACharacter>(GetOwningPlayerController()->GetPawn());
    if (!PlayerCharacter) return;

    FVector CurrentLocation = PlayerCharacter->GetActorLocation();
    bool bIsMoving = !CurrentLocation.Equals(LastPlayerLocation, 0.01f);

    float SwingOffset = 0.f;
    if (bIsMoving)
    {
        SwayTime += GetWorld()->GetDeltaSeconds();
        SwingOffset = FMath::Sin(SwayTime * SwaySpeed) * SwayAmplitude;
    }
    else
    {
        SwayTime = 0.f;
    }

    FVector Forward = PlayerCharacter->GetActorForwardVector();
    FVector Right = PlayerCharacter->GetActorRightVector();
    FVector Up = PlayerCharacter->GetActorUpVector();

    FString WeaponNameString = StaticEnum<EWeaponType>()->GetNameStringByValue((int64)WeaponType);
    //UE_LOG(LogTemp, Warning, TEXT("SelectedWeaponName: %s, WeaponNameString: %s"), *SelectedWeaponName, *WeaponNameString);

    if (SelectedWeaponName.Contains(WeaponNameString))
    {
        if (SpawnedWeapon)
        {
            // Dacă bisDrinking este activ, arma devine invizibilă
            if (bIsDrinking)
            {
                SpawnedWeapon->SetActorHiddenInGame(true);
                SpawnedWeapon->SetActorEnableCollision(false);
                SpawnedWeapon->SetActorTickEnabled(false);
                return; // nu mai actualizăm poziția cât timp e invizibilă
            }
            else
            {
                SpawnedWeapon->SetActorHiddenInGame(false);
                SpawnedWeapon->SetActorEnableCollision(false); // de obicei armele vizuale nu trebuie să aibă coliziune
                SpawnedWeapon->SetActorTickEnabled(false);
            }
        }
        // Dacă arma afișată acum nu e arma pe care vrem să o afișăm acum, distrugem arma curentă
        if (CurrentVisualWeaponType != WeaponType)
        {
            if (SpawnedWeapon)
            {
                SpawnedWeapon->Destroy();
                SpawnedWeapon = nullptr;
            }
            CurrentVisualWeaponType = WeaponType;
        }


        if (!SpawnedWeapon || !SpawnedWeapon->IsValidLowLevel() || SpawnedWeapon->IsPendingKill())
        {
            ACPP_Shooter2GameMode* GameMode = Cast<ACPP_Shooter2GameMode>(GetWorld()->GetAuthGameMode());
            if (GameMode)
            {
                FVector PreviewLocation = CurrentLocation + Forward * 30.f + Right * 30.f + Up * (30.f + SwingOffset);
                FRotator PreviewRotation = PlayerCharacter->GetControlRotation();
                if (WeaponType == EWeaponType::Scar)
                {
                    PreviewRotation.Yaw += 180.f;
                }
                if (WeaponType == EWeaponType::DP28)
                {
                    //UE_LOG(LogTemp, Warning, TEXT("DP 28 SPAWNAT SI ROTIT"));
                    PreviewRotation.Roll += 90.f;
                }
                GameMode->SpawnWeapon(PreviewLocation, PreviewRotation, WeaponType);

                TArray<AActor*> FoundWeapons;
                UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeapon::StaticClass(), FoundWeapons);

                for (AActor* Actor : FoundWeapons)
                {
                    AWeapon* Weapon = Cast<AWeapon>(Actor);
                    if (Weapon && Weapon->WeaponName.Contains(WeaponNameString) && !Weapon->Tags.Contains("VisualOnly"))
                    {
                        SpawnedWeapon = Weapon;

                        Weapon->Tags.Add(FName("VisualOnly"));
                        //UE_LOG(LogTemp, Log, TEXT("%s spawned and captured in HUD."), *WeaponNameString);

                        Weapon->SetActorEnableCollision(false);
                        Weapon->SetActorTickEnabled(false);
                        Weapon->bIsPickupEnabled = false;
                        Weapon->PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                        break;
                    }
                }
            }
        }
        else
        {

           // UE_LOG(LogTemp, Warning, TEXT("Updating location of spawned weapon"));
            FVector PreviewLocation = CurrentLocation + Forward * 30.f + Right * 30.f + Up * (30.f + SwingOffset);
            FRotator PreviewRotation = PlayerCharacter->GetControlRotation();
            if (WeaponType == EWeaponType::DP28)
            {
                //UE_LOG(LogTemp, Warning, TEXT("DP 28 SPAWNAT SI ROTIT"));
                PreviewRotation.Yaw -= 90.f;
            }
            SpawnedWeapon->SetActorLocationAndRotation(PreviewLocation, PreviewRotation);
        }
    }
    else
    {
        if (SpawnedWeapon)
        {
            SpawnedWeapon->Destroy();
            SpawnedWeapon = nullptr;
            CurrentVisualWeaponType = EWeaponType::None;
        }
    }

    LastPlayerLocation = CurrentLocation;
}
void ACPP_Shooter2HUD::AnimateEnergyDrink(float SwayAmplitude, float SwaySpeed)
{
    if (!bIsDrinking)
        return;

    const float AnimDuration = 2.0f;
    SwayTime2 += GetWorld()->GetDeltaSeconds();

    // === Termină animația ===
    if (SwayTime2 >= AnimDuration)
    {
        CurrentEnergy += 25;
        if (CurrentEnergy > 100)
            CurrentEnergy = 100;
        bIsDrinking = false;
        SwayTime2 = 0.f;

        if (HandMeshComponent)
            HandMeshComponent->DestroyComponent();
        if (DrinkMeshComponent)
            DrinkMeshComponent->DestroyComponent();

        HandMeshComponent = nullptr;
        DrinkMeshComponent = nullptr;

        return;
    }

    // === Referințe jucător și cameră ===
    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;

    APawn* Pawn = PC->GetPawn();
    if (!Pawn) return;

    UCameraComponent* Camera = Pawn->FindComponentByClass<UCameraComponent>();
    if (!Camera) return;

    // === Inițializare Hand Mesh ===
    if (!HandMeshComponent && Hands)
    {
        HandMeshComponent = NewObject<UStaticMeshComponent>(Pawn, TEXT("HandMeshComponent"));
        HandMeshComponent->RegisterComponentWithWorld(GetWorld());
        HandMeshComponent->SetStaticMesh(Hands);
        Pawn->AddInstanceComponent(HandMeshComponent);
        HandMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        HandMeshComponent->SetCastShadow(false);
        HandMeshComponent->AttachToComponent(Camera, FAttachmentTransformRules::KeepRelativeTransform);
        HandMeshComponent->SetRelativeLocation(FVector(30.f, 10.f, -10.f));
        HandMeshComponent->SetRelativeRotation(FRotator::ZeroRotator);
        HandMeshComponent->SetOnlyOwnerSee(true);
        HandMeshComponent->SetOwnerNoSee(false);
        HandMeshComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
    }

    // === Inițializare Drink Mesh ===
    if (!DrinkMeshComponent && Mesh_Drink)
    {
        UE_LOG(LogTemp, Warning, TEXT("Creez drinkMeshComponent"));
        DrinkMeshComponent = NewObject<UStaticMeshComponent>(Pawn, TEXT("DrinkMeshComponent"));
        DrinkMeshComponent->RegisterComponentWithWorld(GetWorld());
        DrinkMeshComponent->SetStaticMesh(Mesh_Drink);
        DrinkMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        DrinkMeshComponent->AttachToComponent(HandMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);
        DrinkMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -10.f));
        DrinkMeshComponent->SetRelativeRotation(FRotator::ZeroRotator);
    }

    // === Animație sway și „băut” ===
    float Alpha = SwayTime2 / AnimDuration;
    Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

    float TiltInTime = 0.4f;
    float HoldTime = 1.0f;
    float TiltOutTime = 0.6f;

    float TotalTime = TiltInTime + HoldTime + TiltOutTime;
    float LocalTime = Alpha * TotalTime;

    float TiltAlpha = 0.f;

    if (LocalTime < TiltInTime)
    {
        TiltAlpha = LocalTime / TiltInTime;
    }
    else if (LocalTime < TiltInTime + HoldTime)
    {
        TiltAlpha = 1.f;
    }
    else
    {
        TiltAlpha = 1.f - ((LocalTime - TiltInTime - HoldTime) / TiltOutTime);
    }

    TiltAlpha = FMath::Clamp(TiltAlpha, 0.f, 1.f);

    // === Poziționare și rotație ===
    float SwingOffset = FMath::Sin(SwayTime2 * SwaySpeed) * SwayAmplitude;

    // 👉 Mâna se mișcă împreună cu băutura
    FVector HandOffset(30.f, 10.f, -60.f + SwingOffset + FMath::Lerp(0.f, 15.f, TiltAlpha));
    FRotator HandRotation(FMath::Lerp(0.f, 0.f, TiltAlpha), 90.f, 0.f); // Înclină spre gură

    if (HandMeshComponent)
    {
        HandMeshComponent->SetRelativeLocation(HandOffset);
        HandMeshComponent->SetRelativeRotation(HandRotation);
    }

    // Băutura stă relativ la mână
    if (DrinkMeshComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Am activat bautura"));

        // Poziționare față de mână
        DrinkMeshComponent->SetRelativeLocation(FVector(-60.f, 7.5f, 70.f));

        // Înclinare spre gură în funcție de animație
        float DrinkTiltPitch = FMath::Lerp(0.f, 75.f, TiltAlpha);
        DrinkMeshComponent->SetRelativeRotation(FRotator(0.f, 0.f, DrinkTiltPitch));

        // Debug opțional
        //DrawDebugSphere(GetWorld(), DrinkMeshComponent->GetComponentLocation(), 5.f, 12, FColor::Red, false, 0.1f);
        //DrawDebugCoordinateSystem(GetWorld(), DrinkMeshComponent->GetComponentLocation(), DrinkMeshComponent->GetComponentRotation(), 20.f, false, 0.1f, 0, 1.f);
    }
}






void ACPP_Shooter2HUD::StartAutoFire()
{
    // Trage primul glonț imediat
    HandleAutoFire();

    // Apoi pornește timerul cu delay de 0.1s
    GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &ACPP_Shooter2HUD::HandleAutoFire, 0.1f, true);
}


void ACPP_Shooter2HUD::StopAutoFire()
{
    GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

void ACPP_Shooter2HUD::HandleAutoFire()
{
    if (bhasweapon && (SelectedWeaponSlot == 1 || SelectedWeaponSlot == 2))
    {
        SpawnBullet();
    }
}

//adaugare recul arma
void ACPP_Shooter2HUD::ApplyRecoil()
{
    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;
    //cazul auto
    float RecoilPitch = FMath::FRandRange(-0.08f, -0.025f);   // sus
    float RecoilYaw = FMath::FRandRange(-0.01f, 0.01f);    // stânga-dreapta

    PC->AddPitchInput(RecoilPitch);   // echivalent cu mouse-ul tras ușor în sus
    PC->AddYawInput(RecoilYaw);
}

void ACPP_Shooter2HUD::UpdateMenuButtonPositions()
{
    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;

    int32 ViewportX = 0;
    int32 ViewportY = 0;
    PC->GetViewportSize(ViewportX, ViewportY);

    FVector2D ButtonSize(200.f, 50.f);
    float ButtonSpacing = 20.f;

    FVector2D ScreenCenter(ViewportX * 0.5f, ViewportY * 0.5f);
    FVector2D MenuStartPos(ScreenCenter.X - ButtonSize.X * 0.5f, ScreenCenter.Y - (ButtonSize.Y * 2 + ButtonSpacing * 1.5f));

    MenuButtonPositions.Empty();
    for (int32 i = 0; i < 5; ++i)
    {
        FVector2D ButtonPos = MenuStartPos + FVector2D(0.f, i * (ButtonSize.Y + ButtonSpacing));
        MenuButtonPositions.Add(ButtonPos);
    }
}
int32 ACPP_Shooter2HUD::GetCurrentWeaponDamage()
{
    FString CurrentWeapon = GetWeaponInSlot(SelectedWeaponSlot);

    // 🔫 ASSAULT RIFLES
    if (CurrentWeapon.Contains("AKM"))
    {
        return 49;
    }
    else if (CurrentWeapon.Contains("M416"))
    {
        return 43;
    }
    else if (CurrentWeapon.Contains("Scar"))
    {
        return 43;
    }
    else if (CurrentWeapon.Contains("M762"))
    {
        return 47;
    }
    else if (CurrentWeapon.Contains("Groza"))
    {
        return 48;
    }
    else if (CurrentWeapon.Contains("M249"))
    {
        return 45;
    }
    else if (CurrentWeapon.Contains("DP28"))
    {
        return 51;
    }

    // 🔫 SNIPER RIFLES
    else if (CurrentWeapon.Contains("AWM"))
    {
        return 120;
    }
    else if (CurrentWeapon.Contains("Kar98k"))
    {
        return 90;
    }
    else if (CurrentWeapon.Contains("SKS"))
    {
        return 53;
    }

    // 🔫 SHOTGUNS
    else if (CurrentWeapon.Contains("S686"))
    {
        return 80; // Per pellet
    }
    // 🔫 DEFAULT
    else
    {
        return 30; // Damage default
    }
}


void ACPP_Shooter2HUD::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    APawn* PlayerPawn = GetOwningPawn();
    if (!PlayerPawn) return;

    APawn* ControlledPawn = GetOwningPawn();
    if (!ControlledPawn) return;
    ACar* ControlledCar = Cast<ACar>(ControlledPawn);
    if (PlayerInCar)
    {
        CurrentSpeed = ControlledCar->viteza * 3.6f; // m/s -> km/h
        CurrentRPM = ControlledCar->turatie;
        ACPP_Shooter2GameMode* GM22 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));

        if (CurrentSpeed > GM22->GetTopSpeed())
        {
            GM22->SetTopSpeed(CurrentSpeed);
        }

        float CarDistance = GM22->GetCarDistance();
        GM22->SetCarDistance(CarDistance + CurrentSpeed * DeltaSeconds / 3.6f); // km/h -> m/s * time
    }


    ACPP_Shooter2GameMode* GM21 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
    int32 EngineLevel = GM21->GetCurrentEngineLevel();
    int32 TurboLevel = GM21->GetCurrentTurboLevel();
    int32 WeightLevel = GM21->GetCurrentWeightLevel();
    int32 HealthLevel = GM21->GetCurrentHealthLevel();
    if (CurrentHealth <= 0 && !bShowMainMenu && !bShowSettings && !bShowStore && !bShowAchievements)
    {
        bShowMainMenu = true;
        int NumberDies = GM21->GetNumberDies();
        GM21->SetNumberDies(NumberDies+1);
    }
    int Money = (EngineLevel + 1) * EngineLevel * 10 / 2 + TurboLevel * (TurboLevel + 1) * 10 / 2 +
        WeightLevel * (WeightLevel + 1) * 7 / 2 + HealthLevel * (HealthLevel + 1) / 2;
    float MaxKillDistance = GM21->GetMaxKillDistance();
    int TotalGrenades = GM21->GetTotalGrenades();
    FVector Velocity = PlayerPawn->GetVelocity();
    float Speed = Velocity.Size();
    float Distance152 = GM21->GetPlayerDistance();
    Distance152 = Distance152 + Speed * DeltaSeconds/100.f;
    GM21->SetPlayerDistance(Distance152);
    int NumberDies = GM21->GetNumberDies();
    int BossesKilled = GM21->GetBossesKilled();
    int WeaponCount = GM21->GetOwnedWeaponNames().Num();
    //WeaponName1
    WeaponSlot1 = GM21->GetWeaponName1();
    WeaponSlot2 = GM21->GetWeaponName2();
    /*if (GM8)
    {
        WeaponSlot1 = GM8->GetWeaponName1();
        WeaponSlot2 = GM8->GetWeaponName1();
        if (WeaponSlot1.Contains(TEXT("AKM"))) {
            WeaponIcon1 = AKM_image;
        }
        if (WeaponSlot1.Contains(TEXT("VSS"))) {
            WeaponIcon1 = VSS_image;
        }
        if (WeaponSlot1.Contains(TEXT("Scar"))) {
            WeaponIcon1 = Scar_image;
        }
        if (WeaponSlot1.Contains(TEXT("M416"))) {
            WeaponIcon1 = M416_image;
        }
        if (WeaponSlot1.Contains(TEXT("M762"))) {
            WeaponIcon1 = M762_image;
        }
        if (WeaponSlot1.Contains(TEXT("M249"))) {
            WeaponIcon1 = M249_image;
        }
        if (WeaponSlot1.Contains(TEXT("AWM"))) {
            WeaponIcon1 = AWM_image;
        }
        if (WeaponSlot1.Contains(TEXT("SKS"))) {
            WeaponIcon1 = SKS_image;
        }
        if (WeaponSlot1.Contains(TEXT("Groza"))) {
            WeaponIcon1 = Groza_image;
        }
        if (WeaponSlot1.Contains(TEXT("KAR98K"))) {
            WeaponIcon1 = KAR98K_image;
        }
        if (WeaponSlot1.Contains(TEXT("S686"))) {
            WeaponIcon1 = S686_image;
        }
        if (WeaponSlot1.Contains(TEXT("DP28"))) {
            WeaponIcon1 = DP28_image;
        }

        //si 2
        if (WeaponSlot2.Contains(TEXT("AKM"))) {
            WeaponIcon2 = AKM_image;
        }
        if (WeaponSlot2.Contains(TEXT("VSS"))) {
            WeaponIcon2 = VSS_image;
        }
        if (WeaponSlot2.Contains(TEXT("Scar"))) {
            WeaponIcon2 = Scar_image;
        }
        if (WeaponSlot2.Contains(TEXT("M416"))) {
            WeaponIcon2 = M416_image;
        }
        if (WeaponSlot2.Contains(TEXT("M762"))) {
            WeaponIcon2 = M762_image;
        }
        if (WeaponSlot2.Contains(TEXT("M249"))) {
            WeaponIcon2 = M249_image;
        }
        if (WeaponSlot2.Contains(TEXT("AWM"))) {
            WeaponIcon2 = AWM_image;
        }
        if (WeaponSlot2.Contains(TEXT("SKS"))) {
            WeaponIcon2 = SKS_image;
        }
        if (WeaponSlot2.Contains(TEXT("Groza"))) {
            WeaponIcon2 = Groza_image;
        }
        if (WeaponSlot2.Contains(TEXT("KAR98K"))) {
            WeaponIcon2 = KAR98K_image;
        }
        if (WeaponSlot2.Contains(TEXT("S686"))) {
            WeaponIcon2 = S686_image;
        }
        if (WeaponSlot2.Contains(TEXT("DP28"))) {
            WeaponIcon2 = DP28_image;
        }
    }*/
    if (!bShowMainMenu && bShowAchievements)
    {
        APlayerController* PC27 = GetOwningPlayerController();
        if (!PC27) return;

        if (PC27->WasInputKeyJustPressed(EKeys::LeftMouseButton))
        {
            float MouseX27, MouseY27;
            if (PC27->GetMousePosition(MouseX27, MouseY27))
            {
                FVector2D MousePos2(MouseX27, MouseY27);

                // ==== BACK BUTTON ====
                if (MousePos2.X >= BackButtonPos3.X && MousePos2.X <= BackButtonPos3.X + BackButtonSize3.X &&
                    MousePos2.Y >= BackButtonPos3.Y && MousePos2.Y <= BackButtonPos3.Y + BackButtonSize3.Y)
                {
                    bShowAchievements = false;
                    bShowMainMenu = true;
                    return; // ieșim aici, nu mai verificăm restul
                }

                // ==== ACHIEVEMENT BUTTONS ====
                for (int i = 0; i < AchievementButtonPositions.Num(); i++)
                {
                    FVector2D BtnPos = AchievementButtonPositions[i];
                    FVector2D BtnSize = AchievementButtonSizes[i];

                    if (MousePos2.X >= BtnPos.X && MousePos2.X <= BtnPos.X + BtnSize.X &&
                        MousePos2.Y >= BtnPos.Y && MousePos2.Y <= BtnPos.Y + BtnSize.Y)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Ai dat click pe Achievement %d"), i);
                        if (i == 0 && GM21->GetAchievementState(0) * 5 + 5 <= EngineLevel)
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Enginelevel2 = GM21->GetAchievementState(0);
                            GM21->SetAchievementState(0, Enginelevel2 + 1);
                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices[GM21->GetAchievementState(0)-1]);

                        }
                        if (i == 1 && GM21->GetAchievementState(1) * 5 + 5 <= TurboLevel)
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Turbolevel2 = GM21->GetAchievementState(1);
                            GM21->SetAchievementState(1, Turbolevel2 + 1);
                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices[GM21->GetAchievementState(1) - 1]);
                        }
                        UE_LOG(LogTemp, Warning, TEXT("i=%d, AchievementState[2]=%d, WeightLevel=%d, Calcul=%d"),
                            i,
                            GM21->GetAchievementState(2),
                            WeightLevel,
                            GM21->GetAchievementState(2) * 5 + 5);
                        if (i == 2 && GM21->GetAchievementState(2) * 5 + 5 <= WeightLevel)
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Weightlevel2 = GM21->GetAchievementState(2);
                            GM21->SetAchievementState(2, Weightlevel2 + 1);
                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices[GM21->GetAchievementState(2) - 1]);
                        }
                        if (i == 3 && GM21->GetAchievementState(3) * 10 + 10 <= HealthLevel)
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(3);
                            GM21->SetAchievementState(3, Healthlevel2 + 1);
                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices5[GM21->GetAchievementState(3) - 1]);
                        }
                        if (i == 4 && GM21->GetAchievementState(4)*35.f+180.f <= GM21->GetTopSpeed())
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(4);
                            GM21->SetAchievementState(4, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices6[GM21->GetAchievementState(i) - 1]);
                        }
                        if (i == 5 && GM21->GetCarDistance() >= StageTargets7[GM21->GetAchievementState(5)])
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(5);
                            GM21->SetAchievementState(5, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices7[GM21->GetAchievementState(i) - 1]);
                        }
                        if (i == 6 && GM21->GetTotalKills() >= StageTargets8[GM21->GetAchievementState(6)])
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(6);
                            GM21->SetAchievementState(6, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices8[GM21->GetAchievementState(i) - 1]);
                        }
                        if (i == 7 && GM21->GetTotalKills()*120 >= StageTargets9[GM21->GetAchievementState(7)])
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(7);
                            GM21->SetAchievementState(7, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices9[GM21->GetAchievementState(i) - 1]);
                        }
                        if (i == 8 && GM21->GetGames() >= StageTargets10[GM21->GetAchievementState(8)])
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(8);
                            GM21->SetAchievementState(8, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices10[GM21->GetAchievementState(i) - 1]);
                        }

                        if (i == 9 && Money >= StageTargets11[GM21->GetAchievementState(9)])
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(9);
                            GM21->SetAchievementState(9, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices11[GM21->GetAchievementState(i) - 1]);
                        }

                        if (i == 10 && MaxKillDistance >= StageTargets12[GM21->GetAchievementState(10)])
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(10);
                            GM21->SetAchievementState(10, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices12[GM21->GetAchievementState(i) - 1]);
                        }
                        if (i == 11 && TotalGrenades >= StageTargets13[GM21->GetAchievementState(11)])
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(11);
                            GM21->SetAchievementState(11, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices13[GM21->GetAchievementState(i) - 1]);
                        }

                        if (i == 12 && Distance152 >= StageTargets14[GM21->GetAchievementState(12)])
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(12);
                            GM21->SetAchievementState(12, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices14[GM21->GetAchievementState(i) - 1]);
                        }
                        if (i == 13 && NumberDies >= StageTargets15[GM21->GetAchievementState(13)])
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(13);
                            GM21->SetAchievementState(13, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices15[GM21->GetAchievementState(i) - 1]);
                        }
                        if (i == 14 && BossesKilled >= StageTargets16[GM21->GetAchievementState(14)])
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(14);
                            GM21->SetAchievementState(14, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices16[GM21->GetAchievementState(i) - 1]);
                        }
                        if (i == 15 &&  WeaponCount >= StageTargets17[GM21->GetAchievementState(15)])
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Face upgrade"), i);
                            int32 Healthlevel2 = GM21->GetAchievementState(15);
                            GM21->SetAchievementState(15, Healthlevel2 + 1);

                            int  MyCoins2 = GM21->GetCurrentCoins();
                            GM21->SetCurrentCoins(MyCoins2 + StagePrices17[GM21->GetAchievementState(i) - 1]);
                        }


                        

                        // TODO: aici faci logica reală:
                        // - dacă e Muscle Car => avansezi stagiul
                        // - dacă e alt achievement => marchezi bClaimed = true

                        // exemplu simplu:
                        // Achievements[i].bClaimed = true;

                        break; // am găsit butonul, putem ieși
                    }
                }
            }
        }
    }
    if (!bShowMainMenu && bShowSettings)
    {
        APlayerController* PC26 = GetOwningPlayerController();
        if (!PC26) return;

        if (PC26->WasInputKeyJustPressed(EKeys::LeftMouseButton))
        {
            float MouseX26, MouseY26;
            if (PC26->GetMousePosition(MouseX26, MouseY26))
            {
                FVector2D MousePos(MouseX26, MouseY26);

                // ==== LUMINOZITATE ====
                if (MousePos.X >= BrightnessButtonPositions[0].X && MousePos.X <= BrightnessButtonPositions[0].X + BrightnessButtonSizes[0].X &&
                    MousePos.Y >= BrightnessButtonPositions[0].Y && MousePos.Y <= BrightnessButtonPositions[0].Y + BrightnessButtonSizes[0].Y)
                {
                    BrightnessLevel = FMath::Clamp(BrightnessLevel - 1, 0, 7);
                    GEngine->DisplayGamma = 0.5f + (BrightnessLevel * 0.25f); // aplică gamma
                }

                if (MousePos.X >= BrightnessButtonPositions[1].X && MousePos.X <= BrightnessButtonPositions[1].X + BrightnessButtonSizes[1].X &&
                    MousePos.Y >= BrightnessButtonPositions[1].Y && MousePos.Y <= BrightnessButtonPositions[1].Y + BrightnessButtonSizes[1].Y)
                {
                    BrightnessLevel = FMath::Clamp(BrightnessLevel + 1, 0, 7);
                    GEngine->DisplayGamma = 0.5f + (BrightnessLevel * 0.25f);
                }
                
                //Volum
                if (MousePos.X >= VolumeButtonPositions[0].X && MousePos.X <= VolumeButtonPositions[0].X + VolumeButtonSizes[0].X &&
                    MousePos.Y >= VolumeButtonPositions[0].Y && MousePos.Y <= VolumeButtonPositions[0].Y + VolumeButtonSizes[0].Y)
                {
                    UE_LOG(LogTemp, Log, TEXT("Dau volumul mai incet"));
                    VolumeLevel = FMath::Clamp(VolumeLevel - 1, 0, 10);
                    float VolumeMultiplier = VolumeLevel / 10.0f;

                    IConsoleVariable* MasterVolumeCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("au.MasterVolume"));
                    if (MasterVolumeCVar)
                    {
                        MasterVolumeCVar->Set(VolumeMultiplier);
                    }
                }
                if (MousePos.X >= VolumeButtonPositions[1].X && MousePos.X <= VolumeButtonPositions[1].X + VolumeButtonSizes[1].X &&
                    MousePos.Y >= VolumeButtonPositions[1].Y && MousePos.Y <= VolumeButtonPositions[1].Y + VolumeButtonSizes[1].Y)
                {
                    UE_LOG(LogTemp, Log, TEXT("Dau volumul mai tare"));
                    VolumeLevel = FMath::Clamp(VolumeLevel + 1, 0, 10);
                    float VolumeMultiplier = VolumeLevel / 10.0f;

                    IConsoleVariable* MasterVolumeCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("au.MasterVolume"));
                    if (MasterVolumeCVar)
                    {
                        MasterVolumeCVar->Set(VolumeMultiplier);
                    }
                }


                // ==== GRAPHICS ====
                for (int32 i = 0; i < GraphicsButtonPositions.Num(); i++)
                {
                    if (MousePos.X >= GraphicsButtonPositions[i].X && MousePos.X <= GraphicsButtonPositions[i].X + GraphicsButtonSizes[i].X &&
                        MousePos.Y >= GraphicsButtonPositions[i].Y && MousePos.Y <= GraphicsButtonPositions[i].Y + GraphicsButtonSizes[i].Y)
                    {
                        CurrentGraphicsQuality = i;

                        UGameUserSettings* Settings = GEngine->GetGameUserSettings();
                        if (Settings)
                        {
                            Settings->SetOverallScalabilityLevel(CurrentGraphicsQuality);
                            Settings->ApplySettings(false);
                        }
                    }
                }
                if (MousePos.X >= ResetPlayerDataButtonPos.X && MousePos.X <= ResetPlayerDataButtonPos.X + ResetPlayerDataButtonSize.X &&
                    MousePos.Y >= ResetPlayerDataButtonPos.Y && MousePos.Y <= ResetPlayerDataButtonPos.Y + ResetPlayerDataButtonSize.Y)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Am apasat butonul de resetplayerdata"));
                    GM21->ResetPlayerData();
                }
                // ==== BACK BUTTON ====
                if (MousePos.X >= BackButtonPos2.X && MousePos.X <= BackButtonPos2.X + BackButtonSize2.X &&
                    MousePos.Y >= BackButtonPos2.Y && MousePos.Y <= BackButtonPos2.Y + BackButtonSize2.Y)
                {
                    bShowSettings = false;
                    bShowMainMenu = true;
                }
            }
        }
    }

    if (!bShowMainMenu && bShowStore)
    {
        APlayerController* PC25 = GetOwningPlayerController();
        if (!PC25) return;
        // Verifică dacă s-a apăsat click stânga
        if (PC25->WasInputKeyJustPressed(EKeys::LeftMouseButton))
        {
            float MouseX25, MouseY25;
            if (PC25->GetMousePosition(MouseX25, MouseY25))
            {
                FVector2D MousePos25(MouseX25, MouseY25);
                ACPP_Shooter2GameMode* GM3 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
                int32 Engine = 1;
                int32 Turbo = 1;
                int32 Weight = 1;
                int32 Health = 1;
                int32 MyCoins = 0;
                if (GM3)
                {
                    Engine = GM3->GetCurrentEngineLevel();
                    Turbo = GM3->GetCurrentTurboLevel();
                    Weight = GM3->GetCurrentWeightLevel();
                    Health = GM3->GetCurrentHealthLevel();
                    MyCoins = GM3->GetCurrentCoins();
                    UE_LOG(LogTemp, Log, TEXT("Current upgrades: Engine=%d, Turbo=%d, Weight=%d"),
                        Engine, Turbo, Weight);
                }
                //GM3->SetCurrentEngineLevel(Engine + 1);
                // ==== Verific butoane Upgrade ====
                for (int i = 0; i < UpgradeButtonPositions.Num(); i++)
                {
                    FVector2D BtnPos = UpgradeButtonPositions[i];
                    FVector2D BtnSize = UpgradeButtonSizes[i];

                    if (MousePos25.X >= BtnPos.X && MousePos25.X <= BtnPos.X + BtnSize.X &&
                        MousePos25.Y >= BtnPos.Y && MousePos25.Y <= BtnPos.Y + BtnSize.Y)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Am apasat Upgrade la %d"), i);

                        if (i == 0 && Engine >=1 && Engine<=20)
                        {
                            //EngineLevel++;
                            int32 price = Engine * 10;
                            if (MyCoins >= price)
                            {
                                GM3->SetCurrentEngineLevel(Engine + 1);
                                MyCoins = MyCoins - price;
                            }
                            UE_LOG(LogTemp, Warning, TEXT("Engine upgrade -> Level"));
                        }
                        if (i == 1 && Turbo >= 1 && Turbo <= 20)
                        {
                            //TurboLevel++;
                            int32 price = Turbo * 10;
                            if (MyCoins >= price)
                            {
                                GM3->SetCurrentTurboLevel(Turbo + 1);
                                MyCoins = MyCoins - price;
                            }
                            UE_LOG(LogTemp, Warning, TEXT("Turbo upgrade -> Level:"));
                        }
                        if (i == 2 && Weight >=1 && Weight <= 20)
                        {
                            int32 price = Weight * 7;
                            //WeightLevel++;
                            if (MyCoins >= price) {
                                GM3->SetCurrentWeightLevel(Weight + 1);
                                MyCoins = MyCoins - price;
                            }
                            UE_LOG(LogTemp, Warning, TEXT("Weight upgrade -> Level:"));
                        }
                        if (i == 3 && Health >= 1 && Health <= 51)
                        {
                            int32 price = Health;
                            if (MyCoins >= price)
                            {
                                GM3->SetCurrentHealthtLevel(Health + 1);
                                MyCoins = MyCoins - price;
                            }
                           
                            if (Health < 51) {
                                CurrentHealth++;
                                MaxHealth++;
                            }
                            UE_LOG(LogTemp, Warning, TEXT("Health upgrade -> Level:"));

                        }

                    }
                }
                ACPP_Shooter2GameMode* GM7 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
                int Banuti = GM7->GetCurrentCoins();
               // ACPP_Shooter2GameMode* GM100 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
                for (int i = 0; i < WeaponButtonPositions.Num(); i++)
                {
                    FVector2D BtnPos = WeaponButtonPositions[i];
                    FVector2D BtnSize = WeaponButtonSizes[i];
                    if (MousePos25.X >= BtnPos.X && MousePos25.X <= BtnPos.X + BtnSize.X &&
                        MousePos25.Y >= BtnPos.Y && MousePos25.Y <= BtnPos.Y + BtnSize.Y)
                    {
                       // bool flag = false;
                        UE_LOG(LogTemp, Warning, TEXT("Ai apasat sa iei arma: %s cu pretul %d"), *WeaponNames[i], WeaponPrices[i]);
                        TArray<FString> Nume_arme = GM7->GetOwnedWeaponNames();
                        if (!Nume_arme.Contains(WeaponNames[i])) {
                            //daca cumpar pe bune arma si nu o detin
                            if (Banuti >= WeaponPrices[i])
                            {
                                //flag = true
                                Nume_arme.Add(WeaponNames[i]);
                                GM7->SetOwnedWeaponNames(Nume_arme);
                                MyCoins = MyCoins - WeaponPrices[i];
                                GM7->SetCurrentCoins(MyCoins);
                                UE_LOG(LogTemp, Warning, TEXT("Banuti dupa cumparare: %d"), MyCoins);
                                //UpdateHUD();
                            }
                        }
                        FString SelectedWeaponName;
                        if (SelectedWeaponSlot == 1 && Nume_arme.Contains(WeaponNames[i]))
                        {
                            WeaponSlot1 = WeaponNames[i];
                            SelectedWeaponName = WeaponSlot1;
                            if (WeaponSlot1.Contains("Empty"))
                            {
                                WeaponSlot1 = GM7->GetWeaponName1();
                            }
                            else
                            {
                                GM7->SetWeaponName1(WeaponSlot1);
                            }

                            if (WeaponSlot1.Contains(TEXT("AKM"))) {
                                WeaponIcon1 = AKM_image;
                            }
                            if (WeaponSlot1.Contains(TEXT("VSS"))) {
                                WeaponIcon1 = VSS_image;
                            }
                            if (WeaponSlot1.Contains(TEXT("Scar"))) {
                                WeaponIcon1 = Scar_image;
                            }
                            if (WeaponSlot1.Contains(TEXT("M416"))) {
                                WeaponIcon1 = M416_image;
                            }
                            if (WeaponSlot1.Contains(TEXT("M762"))) {
                                WeaponIcon1 = M762_image;
                            }
                            if (WeaponSlot1.Contains(TEXT("M249"))) {
                                WeaponIcon1 = M249_image;
                            }
                            if (WeaponSlot1.Contains(TEXT("AWM"))) {
                                WeaponIcon1 = AWM_image;
                            }
                            if (WeaponSlot1.Contains(TEXT("SKS"))) {
                                WeaponIcon1 = SKS_image;
                            }
                            if (WeaponSlot1.Contains(TEXT("Groza"))) {
                                WeaponIcon1 = Groza_image;
                            }
                            if (WeaponSlot1.Contains(TEXT("KAR98K"))) {
                                WeaponIcon1 = KAR98K_image;
                            }
                            if (WeaponSlot1.Contains(TEXT("S686"))) {
                                WeaponIcon1 = S686_image;
                            }
                            if (WeaponSlot1.Contains(TEXT("DP28"))) {
                                WeaponIcon1 = DP28_image;
                            }
                        }
                        else if (SelectedWeaponSlot == 2 && Nume_arme.Contains(WeaponNames[i]))
                        {
                            WeaponSlot2 = WeaponNames[i];
                            SelectedWeaponName = WeaponSlot2;
                            if (WeaponSlot2.Contains("Empty"))
                            {
                                WeaponSlot2 = GM7->GetWeaponName2();
                            }
                            else {
                                GM7->SetWeaponName2(WeaponSlot2);
                            }
                            if (WeaponSlot2.Contains(TEXT("AKM"))) {
                                WeaponIcon2 = AKM_image;
                            }
                            if (WeaponSlot2.Contains(TEXT("VSS"))) {
                                WeaponIcon2 = VSS_image;
                            }
                            if (WeaponSlot2.Contains(TEXT("Scar"))) {
                                WeaponIcon2 = Scar_image;
                            }
                            if (WeaponSlot2.Contains(TEXT("M416"))) {
                                WeaponIcon2 = M416_image;
                            }
                            if (WeaponSlot2.Contains(TEXT("M762"))) {
                                WeaponIcon2 = M762_image;
                            }
                            if (WeaponSlot2.Contains(TEXT("M249"))) {
                                WeaponIcon2 = M249_image;
                            }
                            if (WeaponSlot2.Contains(TEXT("AWM"))) {
                                WeaponIcon2 = AWM_image;
                            }
                            if (WeaponSlot2.Contains(TEXT("SKS"))) {
                                WeaponIcon2 = SKS_image;
                            }
                            if (WeaponSlot2.Contains(TEXT("Groza"))) {
                                WeaponIcon2 = Groza_image;
                            }
                            if (WeaponSlot2.Contains(TEXT("KAR98K"))) {
                                WeaponIcon2 = KAR98K_image;
                            }
                            if (WeaponSlot2.Contains(TEXT("S686"))) {
                                WeaponIcon2 = S686_image;
                            }
                            if (WeaponSlot2.Contains(TEXT("DP28"))) {
                                WeaponIcon2 = DP28_image;
                            }
                        }
                    }
                }

                //aici este partea de grenade
                for (int32 i = 0; i < ThrowableButtonPositions.Num(); i++)
                {
                    FVector2D BtnPos = ThrowableButtonPositions[i];
                    FVector2D BtnSize = ThrowableButtonSizes[i];

                    bool bInX = MouseX25 >= BtnPos.X && MouseX25 <= BtnPos.X + BtnSize.X;
                    bool bInY = MouseY25 >= BtnPos.Y && MouseY25 <= BtnPos.Y + BtnSize.Y;

                    if (bInX && bInY)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Am dat click pe butonul %d (Grenade/Molotov/Smoke)"), i);

                        // Aici faci logica de cumparare
                        if (i == 0) { 
                            if (MyCoins >= 40) {
                                MyCoins = MyCoins - 40; //consum 40 de gold pe o grenada
                                GM3->SetGrenades(Grenades + 1);
                                Grenades++;

                                GM3->SetCurrentCoins(MyCoins);
                            }
                        }
                        else if (i == 1) {
                            if (MyCoins >= 35) {
                                GM3->SetMolotovs(Molotovs + 1);
                                Molotovs++;
                                MyCoins = MyCoins - 35; //consum 35 de gold pe un molotov
                                GM3->SetCurrentCoins(MyCoins);
                            }
                        }
                        else if (i == 2) {
                            if (MyCoins >= 15) {
                                GM3->SetSmokeGrenades(SmokeGrenades + 1);
                                SmokeGrenades++;
                                MyCoins = MyCoins - 15;
                                GM3->SetCurrentCoins(MyCoins);
                            }
                        }
                    }
                }
                
                //aici fac partea de cumparat gloante
                for (int32 i = 0; i < AmmoButtonPositions.Num(); i++)
                {
                    FVector2D BtnPos = AmmoButtonPositions[i];
                    FVector2D BtnSize = AmmoButtonSizes[i];

                    bool bInX = MouseX25 >= BtnPos.X && MouseX25 <= BtnPos.X + BtnSize.X;
                    bool bInY = MouseY25 >= BtnPos.Y && MouseY25 <= BtnPos.Y + BtnSize.Y;

                    if (bInX && bInY)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Ai dat click pe butonul de gloanțe %d"), i);

                        // Aici faci logica de cumpărare:
                        // i == 0 -> 5.56
                        // i == 1 -> 7.62
                        // i == 2 -> 9mm
                        if (i == 0) {
                            if (MyCoins >= 100) {
                                MyCoins = MyCoins - 100; //consum 40 de gold pe o grenada
                                GM3->AddBullet556(100);
                                gloante_556 = gloante_556 + 100;
                                GM3->SetCurrentCoins(MyCoins);
                            }
                        }  // exemplu +30
                        else if (i == 1) {
                            if (MyCoins >= 100) {
                                MyCoins = MyCoins - 100; //consum 40 de gold pe o grenada
                                GM3->AddBullet762(100);
                                gloante_762 = gloante_762 + 100;
                                GM3->SetCurrentCoins(MyCoins);
                            }
                        }
                        else if (i == 2) {
                            if (MyCoins >= 100) {
                                MyCoins = MyCoins - 100; //consum 40 de gold pe o grenada
                                GM3->AddBullet9(100);
                                gloante_9 = gloante_9 + 100;
                                GM3->SetCurrentCoins(MyCoins);
                            }
                        }
                    }
                }


                if (GM3)
                {
                    GM3->SetCurrentCoins(MyCoins);
                }

                // ==== Verific Back ====
                if (MousePos25.X >= BackButtonPos.X && MousePos25.X <= BackButtonPos.X + BackButtonSize.X &&
                    MousePos25.Y >= BackButtonPos.Y && MousePos25.Y <= BackButtonPos.Y + BackButtonSize.Y)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Back pressed"));
                    bShowStore = false;
                    bShowMainMenu = true;
                }
            }
        }
    }

    else if (bShowMainMenu)
    {
        UpdateMenuButtonPositions();
        OpenMainMenu();
        APlayerController* PC = GetOwningPlayerController();
        if (!PC) return;

        // Verifică dacă mouse-ul a făcut click stânga
        if (PC->WasInputKeyJustPressed(EKeys::LeftMouseButton))
        {
            float MouseX, MouseY;
            PC->GetMousePosition(MouseX, MouseY);
            FVector2D MousePos(MouseX, MouseY);
            UE_LOG(LogTemp, Warning, TEXT("Mouse Click at: %f, %f"), MouseX, MouseY);

            // Dimensiunea butoanelor (trebuie să fie aceeași cu cea din DrawHUD)
            FVector2D ButtonSize(200.f, 50.f);

            // Parcurgem butoanele salvate în MenuButtonPositions
            for (int32 i = 0; i < MenuButtonPositions.Num(); ++i)
            {
                FVector2D ButtonPos = MenuButtonPositions[i];

                // Verifică dacă mouse-ul este în interiorul butonului
                if (MousePos.X >= ButtonPos.X && MousePos.X <= ButtonPos.X + ButtonSize.X &&
                    MousePos.Y >= ButtonPos.Y && MousePos.Y <= ButtonPos.Y + ButtonSize.Y)
                {
                    ACPP_Shooter2GameMode* GM6 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
                    // Am găsit butonul apăsat, facem acțiunea aferentă
                    switch (i)
                    {
                    case 0: // Play
                        UE_LOG(LogTemp, Warning, TEXT("Play pressed"));
                        if (GM6)
                        {
                            MaxHealth = 100 + GM6->CurrentHealthLevel - 1;
                            CurrentHealth = 100 + GM6->CurrentHealthLevel - 1;
                        }
                        bShowMainMenu = false;  // Ascundem meniul
                        CloseMainMenu();
                        //CloseMainMenu();
                        break;
                    case 1:
                        UE_LOG(LogTemp, Warning, TEXT("Store pressed"));
                        bShowStore = true;
                        bShowMainMenu = false;
                        break;
                    case 2: // Achievements
                        UE_LOG(LogTemp, Warning, TEXT("Achievements pressed"));
                        // Aici poți deschide meniul de achievements
                        bShowAchievements = true;
                        bShowStore = false;
                        bShowMainMenu = false;
                        break;

                    case 3: // Settings
                        UE_LOG(LogTemp, Warning, TEXT("Settings pressed"));
                        // Aici poți deschide setările
                        bShowSettings = true;
                        bShowMainMenu = false;
                        bShowStore = false;
                        break;

                    case 4: // Exit
                        if (bShowMainMenu == true && bShowStore == false) {
                            UE_LOG(LogTemp, Warning, TEXT("Exit pressed"));
                            UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
                            break;
                        }
                        break;
                    }

                    // Odată găsit și procesat butonul, ieșim din buclă
                    break;
                }
            }
        }
    }
    if (bShowMainMenu)
    {
        if (APlayerController* PC = GetOwningPlayerController())
        {
            if (!PC->IsPaused())
            {
               // PC->SetPause(true);
            }
        }
    }
    else
    {
        if (APlayerController* PC = GetOwningPlayerController())
        {
            if (PC->IsPaused())
            {
               // PC->SetPause(false);
            }
        }
    }

    static float LastDamageTime = 0.f;
    float CurrentTime = GetWorld()->GetTimeSeconds();

    float DamageCooldown = 2.0f; // cât de des poate lua damage (1 sec)

    for (TActorIterator<AEnemy_Bot> It(GetWorld()); It; ++It)
    {
        AEnemy_Bot* Bot = *It;
        if (!Bot) continue;

        float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), Bot->GetActorLocation());

        if (Distance < 120.f && (CurrentTime - LastDamageTime) > DamageCooldown && Bot->CurrentHealth > 0)
        {
            // Aplică damage jucătorului
            ShowBloodOverlay();
            LastDamageTime = CurrentTime;
            if (CurrentHealth - 10 < 0)
            {
                CurrentHealth = 0;
            }
            else
            {
                CurrentHealth = CurrentHealth - 10;
            }
            if (SwordSound)
            {
                float CurrentVolume = VolumeLevel / 10.0f;
                UGameplayStatics::PlaySoundAtLocation(this, SwordSound, GetActorLocation(),CurrentVolume);
            }
            UE_LOG(LogTemp, Warning, TEXT("💢 Jucătorul a luat damage de la bot!"));
        }
    }

    //pentru boss
    float BossDamageCooldown = 2.0f; // Boss atacă mai des
    float BossDamageRange = 200.f;   // Boss are range mai mare
    int32 BossDamage = 25;           // Boss face mai mult damage
    static float LastBossDamageTime = 0.f;
    for (TActorIterator<ABossEnemyBot> BossIt(GetWorld()); BossIt; ++BossIt)
    {
        ABossEnemyBot* Boss = *BossIt;
        if (!Boss) continue;

        float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), Boss->GetActorLocation());

        if (Distance < BossDamageRange &&
            (CurrentTime - LastBossDamageTime) > BossDamageCooldown &&
            Boss->CurrentHealth > 0)
        {
            // Aplică damage de la boss
            ShowBloodOverlay();
            LastBossDamageTime = CurrentTime;

            if (CurrentHealth - BossDamage < 0)
            {
                CurrentHealth = 0;
            }
            else
            {
                CurrentHealth = CurrentHealth - BossDamage;
            }

            if (SwordSound)
            {
                float CurrentVolume = VolumeLevel / 10.0f;
                UGameplayStatics::PlaySoundAtLocation(this, SwordSound, GetActorLocation(), CurrentVolume * 1.2f); // Boss mai tare
            }

            UE_LOG(LogTemp, Warning, TEXT("💀 Jucătorul a luat %d damage de la BOSS!"), BossDamage);
            break; // Oprește după primul hit
        }
    }



    for (TActorIterator<AEnemyRangedBot> It(GetWorld()); It; ++It)
    {
        AEnemyRangedBot* Bot2 = *It;
        if (!Bot2) continue;

        float Distance2 = FVector::Dist(PlayerPawn->GetActorLocation(), Bot2->GetActorLocation());

        if (Distance2 < 120.f && (CurrentTime - LastDamageTime) > DamageCooldown && Bot2->CurrentHealth > 0)
        {
            // Aplică damage jucătorului
            ShowBloodOverlay();
            LastDamageTime = CurrentTime;
            if (CurrentHealth - 10 < 0)
            {
                CurrentHealth = 0;
            }
            else
            {
                CurrentHealth = CurrentHealth - 10;
            }
            if (SwordSound)
            {
                UGameplayStatics::PlaySoundAtLocation(this, SwordSound, GetActorLocation());
            }
            UE_LOG(LogTemp, Warning, TEXT("💢 Jucătorul a luat damage de la bot!"));
        }
    }


    // În Tick(float DeltaTime)
    FString w1 = GetWeaponInSlot(1);
    FString w2 = GetWeaponInSlot(2);
    if (SelectedWeaponSlot == 1)
    {
        if (w1.Contains("AKM") || w1.Contains("Scar") || w1.Contains("M416") || w1.Contains("M762") || w1.Contains("M249")
            || w1.Contains("Groza") || w1.Contains("DP28"))
        {
            AutomaticWeapon = true;
        }
        else
        {
            AutomaticWeapon = false;
        }
    }
    if (SelectedWeaponSlot == 2)
    {
        if (w2.Contains("AKM") || w2.Contains("Scar") || w2.Contains("M416") || w2.Contains("M762") || w2.Contains("M249")
            || w2.Contains("Groza") || w2.Contains("DP28"))
        {
            AutomaticWeapon = true;
        }
        else
        {
            AutomaticWeapon = false;
        }
    }

    if (APlayerController* PC = GetOwningPlayerController())
    {
        ACPP_Shooter2GameMode* GM8 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
        if (GM8)
        {
            if (PC->WasInputKeyJustPressed(EKeys::Q)) {
                UE_LOG(LogTemp, Warning, TEXT("Am apasat tasta Q"));
                bShowSettings = false;
                bShowMainMenu = true;
                bShowStore = false;
                bShowAchievements = false;
            }
        }
        if (PC->WasInputKeyJustPressed(EKeys::One))
        {
            SelectSlot1();
            SelectedWeaponSlot = 1;
        }
        else if (PC->WasInputKeyJustPressed(EKeys::Two))
        {
            SelectSlot2();
            SelectedWeaponSlot = 2;
        }
        if (PC->WasInputKeyJustPressed(EKeys::Three))
        {
            SelectGrenade();
            SelectedWeaponSlot = 3;
        }
        if (PC->WasInputKeyJustPressed(EKeys::Four))
        {
            SelectMolotov_Cocktail();
            SelectedWeaponSlot = 4;
        }
        if (PC->WasInputKeyJustPressed(EKeys::Five))
        {
            SelectSlot5();
            SelectedWeaponSlot = 5;
        }
        if (PC->WasInputKeyJustPressed(EKeys::E))
        {
            ToggleInventory();
        }
        if (PC->WasInputKeyJustPressed(EKeys::R))
        {
            if (SelectedWeaponSlot == 1) {
                FString nume_arma1 = GetWeaponInSlot(1);
                if (!nume_arma1.Contains("Empty") && AmmoInMag1 < MaxAmmoInMag1) {
                    bIsReloading = true;
                    ReloadTimer = 0.f;
                    ReloadingWeaponName = nume_arma1;
                    if (ReloadSound)
                    {
                        if (VolumeLevel > 0)
                        {
                            float CurrentVolume = VolumeLevel / 10.0f;
                            FVector Location14 = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
                            UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, Location14, CurrentVolume);
                        }
                        // La VolumeLevel = 0, nu reda sunetul deloc
                    }
                    //EBulletType bul1 = Match_Weapon_with_bullets(nume_arma1);
                    //ReloadWeapon(bul1, 1);
                }
            }
            if (SelectedWeaponSlot == 2) {
                FString nume_arma2 = GetWeaponInSlot(2);
                if (!nume_arma2.Contains("Empty") && AmmoInMag2 < MaxAmmoInMag2) {
                    bIsReloading = true;
                    ReloadTimer = 0.f;
                    ReloadingWeaponName = nume_arma2;
                    if (ReloadSound)
                    {
                        FVector Location14 = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
                        UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, Location14);
                    }
                    //EBulletType bul2 = Match_Weapon_with_bullets(nume_arma2);
                    //ReloadWeapon(bul2, 2);
                }
            }
        }
        if (bIsInventoryOpen && PC->WasInputKeyJustPressed(EKeys::LeftMouseButton) && (SelectedWeaponSlot==1 || SelectedWeaponSlot == 2))
        {
            //UE_LOG(LogTemp, Log, TEXT("Am dat click stanga cand era inventory deschis"));
            float MouseX, MouseY;
            if (PC->GetMousePosition(MouseX, MouseY))
            {
                // Verifică dacă mouse-ul este peste butonul Close
                if (IsPointInRect(FVector2D(MouseX, MouseY), CloseButtonPos, CloseButtonSize))
                {
                    ToggleInventory();  // Închide inventarul
                    return;
                }
            }
        }
        if (PC->WasInputKeyJustPressed(EKeys::P) && bShowBulletIcon == true)
        {
            UE_LOG(LogTemp, Warning, TEXT("Pot da pick"));
            PickBullet = true;
            //UGameplayStatics::PlaySound2D(this, PickupWeaponSound);
        }
        if (PC->WasInputKeyJustPressed(EKeys::X))
        {
            UE_LOG(LogTemp, Warning, TEXT("Animatie"));
            //AnimateEnergyDrink(5.f,4.f);
            bIsDrinking = true;
            SwayTime2 = 0.f;
            //UGameplayStatics::PlaySound2D(this, PickupWeaponSound);
        }
        if (PC->WasInputKeyJustPressed(EKeys::LeftMouseButton) && bhasweapon && (SelectedWeaponSlot == 1 || SelectedWeaponSlot == 2)
            && bShowMainMenu == false && bShowStore == false && bShowSettings == false && bShowAchievements == false)
        {
            if (AutomaticWeapon)
            {
                StartAutoFire();
            }
            else
            {
                SpawnBullet();
            }
        }

        // STOP FIRE
        if (PC->WasInputKeyJustReleased(EKeys::LeftMouseButton) && AutomaticWeapon)
        {
            StopAutoFire();
        }
        if (PC->WasInputKeyJustPressed(EKeys::M))
        {
            bSeeMap = !bSeeMap;

            if (bSeeMap)
            {
                UE_LOG(LogTemp, Log, TEXT("vREAU SA VAD HARTA MARE"));
                MiniMapCapture->OrthoWidth = 200000.f;
                // Afișează harta mare aici
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("NU MAI VREAU SA VAD HARTA"));
                MiniMapCapture->OrthoWidth = 2000.f;
                // Ascunde harta mare aici
            }
            if (bSeeMap && SceneCaptureActor)
            {
                MiniMapCapture = SceneCaptureActor->GetCaptureComponent2D();
                if (MiniMapCapture && MiniMapCapture->ProjectionType == ECameraProjectionMode::Orthographic)
                {
                    MiniMapCapture->OrthoWidth = 200000.f;
                }
                else
                {
                    MiniMapCapture->OrthoWidth = 2000.f;
                }
            }
        }



        if (PC->WasInputKeyJustPressed(EKeys::RightMouseButton) && bhasweapon == true && (SelectedWeaponSlot == 1 || SelectedWeaponSlot == 2))
        {
           // UE_LOG(LogTemp, Log, TEXT("Am apasat click dreapta"));
            //SpawnBullet();
            ViewWithScope();
            if (ScopeSound)
            {
                if (VolumeLevel > 0)
                {
                    float CurrentVolume = VolumeLevel / 10.0f;
                    FVector Location12 = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
                    UGameplayStatics::PlaySoundAtLocation(this, ScopeSound, Location12, CurrentVolume);
                }
                // La VolumeLevel = 0, nu reda sunetul deloc
            }
        }
    }

    FString SelectedWeaponName;

    if (SelectedWeaponSlot == 1)
    {
        SelectedWeaponName = WeaponSlot1;
    }
    else if (SelectedWeaponSlot == 2)
    {
        SelectedWeaponName = WeaponSlot2;
    }

    //UE_LOG(LogTemp, Log, TEXT("Arma din slotul %d este: %s"), SelectedWeaponSlot, *SelectedWeaponName);
    ACharacter* PlayerCharacter = Cast<ACharacter>(GetOwningPlayerController()->GetPawn());
    if (!PlayerCharacter) return;
    
    //
    //
    FVector CurrentLocation = PlayerCharacter->GetActorLocation();
    FVector CurrentVelocity = (CurrentLocation - LastPlayerLocation) / DeltaSeconds;

    // Calcul accelerație (diferența vitezei între frame-uri)
    FVector Acceleration = (CurrentVelocity - LastVelocity) / DeltaSeconds;
    float VerticalAcceleration = Acceleration.Z; // accelerație pe verticală (poate fi pozitivă sau negativă)

    // Logging viteze și accelerații
    UE_LOG(LogTemp, Warning, TEXT("CurrentVelocity.Z: %f, LastVelocity.Z: %f, VerticalAcceleration: %f"),
        CurrentVelocity.Z, LastVelocity.Z, VerticalAcceleration);

    // Verificare pentru impact (ex: o scădere bruscă a vitezei verticale)
    float ImpactThreshold = 1001.f;  // prag de schimbare rapidă în viteză
    if ((CurrentVelocity.Z - LastVelocity.Z) > ImpactThreshold)
    {
        float DamageAmount = FMath::Abs(CurrentVelocity.Z - LastVelocity.Z) * 0.05f;
        CurrentHealth = FMath::Max(0.f, CurrentHealth - DamageAmount);
        if (FallingSound) {
            float CurrentVolume = VolumeLevel / 10.0f;
            UGameplayStatics::PlaySoundAtLocation(this, FallingSound, GetActorLocation(),CurrentVolume);
        }
        ShowBloodOverlay();
        UE_LOG(LogTemp, Warning, TEXT("Impact detected! Damage applied: %f"), DamageAmount);
    }

    // Actualizare pentru următorul tick
    LastVelocity = CurrentVelocity;
    LastPlayerLocation = CurrentLocation;

    bool bIsMoving = !CurrentLocation.Equals(LastPlayerLocation, 0.01f);
    float SwayAmplitude = 5.f; // Cât de mare e swing-ul
    float SwaySpeed = 4.f;      // Cât de rapid e swing-ul (Hz)

    float SwingOffset = 0.f;
    if (bIsMoving)
    {
        SwayTime += GetWorld()->GetDeltaSeconds();
        SwingOffset = FMath::Sin(SwayTime * SwaySpeed) * SwayAmplitude;
    }
    else
    {
        SwayTime = 0.f;
    }
    FVector Forward = PlayerCharacter->GetActorForwardVector();
    FVector Right = PlayerCharacter->GetActorRightVector();
    FVector Up = PlayerCharacter->GetActorUpVector();
    if (SelectedWeaponName.Contains(TEXT("AKM"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::AKM, 5.f, 4.f);
    }
    if (SelectedWeaponName.Contains(TEXT("VSS"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::VSS, 5.f, 4.f);
        UE_LOG(LogTemp, Log, TEXT("Arma contine VSS."));
    }
    if (SelectedWeaponName.Contains(TEXT("Scar"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::Scar, 5.f, 4.f);
        UE_LOG(LogTemp, Log, TEXT("Arma contine Scar."));
    }
    if (SelectedWeaponName.Contains(TEXT("M416"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::M416, 5.f, 4.f);
        UE_LOG(LogTemp, Log, TEXT("Arma contine M416."));
    }
    if (SelectedWeaponName.Contains(TEXT("M762"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::M762, 5.f, 4.f);
        UE_LOG(LogTemp, Log, TEXT("Arma contine M762."));
    }
    if (SelectedWeaponName.Contains(TEXT("M249"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::M249, 5.f, 4.f);
        UE_LOG(LogTemp, Log, TEXT("Arma contine M249."));
    }
    if (SelectedWeaponName.Contains(TEXT("AWM"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::AWM, 5.f, 4.f);
        UE_LOG(LogTemp, Log, TEXT("Arma contine AWM."));
    }
    if (SelectedWeaponName.Contains(TEXT("SKS"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::SKS, 5.f, 4.f);
        UE_LOG(LogTemp, Log, TEXT("Arma contine SKS."));
    }
    if (SelectedWeaponName.Contains(TEXT("Groza"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::Groza, 5.f, 4.f);
        UE_LOG(LogTemp, Log, TEXT("Arma contine Groza."));
    }
    if (SelectedWeaponName.Contains(TEXT("KAR98K"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::KAR98K, 5.f, 4.f);
        UE_LOG(LogTemp, Log, TEXT("Arma contine KAR98K."));
    }
    if (SelectedWeaponName.Contains(TEXT("S686"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::S686, 5.f, 4.f);
        UE_LOG(LogTemp, Log, TEXT("Arma contine S686."));
    }
    if (SelectedWeaponName.Contains(TEXT("DP28"))) {
        SpawnOrUpdateVisualWeapon(EWeaponType::DP28, 5.f, 4.f);
        UE_LOG(LogTemp, Log, TEXT("Arma contine DP28."));
    }
}
bool ACPP_Shooter2HUD::IsPointInRect(const FVector2D& Point, const FVector2D& RectTopLeft, const FVector2D& RectSize)
{
    return Point.X >= RectTopLeft.X && Point.X <= RectTopLeft.X + RectSize.X &&
        Point.Y >= RectTopLeft.Y && Point.Y <= RectTopLeft.Y + RectSize.Y;
}
void ACPP_Shooter2HUD::OpenMainMenu()
{
    bShowMainMenu = true;

    APlayerController* PC = GetOwningPlayerController();
    if (PC)
    {
        PC->bShowMouseCursor = true;
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);


       // PC->SetPause(true);  // Pune jocul pe pauză
    }
}

void ACPP_Shooter2HUD::CloseMainMenu()
{
    bShowMainMenu = false;

    APlayerController* PC = GetOwningPlayerController();
    if (PC)
    {
        PC->bShowMouseCursor = false;

        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);

       // PC->SetPause(false);  // Deblochează jocul
    }
}

void ACPP_Shooter2HUD::ToggleInventory()
{
    bIsInventoryOpen = !bIsInventoryOpen;
    bIsInventoryVisible = bIsInventoryOpen;

    if (APlayerController* PC = GetOwningPlayerController())
    {
        if (bIsInventoryOpen)
        {
           // UE_LOG(LogTemp, Log, TEXT("Inventory opened."));

            PC->bShowMouseCursor = true;

            FInputModeGameAndUI InputMode;
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            InputMode.SetHideCursorDuringCapture(false);
            InputMode.SetWidgetToFocus(nullptr); // Doar dacă ai un widget UMG

            PC->SetInputMode(InputMode);
        }
        else
        {
           // UE_LOG(LogTemp, Log, TEXT("Inventory closed."));

            PC->bShowMouseCursor = false;

            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
        }
    }
}

void ACPP_Shooter2HUD::SetWeaponInSlot(int32 SlotIndex, const FString& WeaponName, UTexture2D* WeaponIcon)
{
    if (SlotIndex == 1)
    {
        WeaponSlot1 = WeaponName;
        WeaponIcon1 = WeaponIcon;
    }
    else if (SlotIndex == 2)
    {
        WeaponSlot2 = WeaponName;
        WeaponIcon2 = WeaponIcon;
    }
}

FString ACPP_Shooter2HUD::GetWeaponInSlot(int32 SlotIndex) const
{
    if (SlotIndex == 1)
        return WeaponSlot1;
    else if (SlotIndex == 2)
        return WeaponSlot2;
    if (SlotIndex == 3)
        return GrenadeSlot;

    return TEXT("Invalid Slot");
}

EBulletType ACPP_Shooter2HUD::Match_Weapon_with_bullets(const FString& WeaponName)
{
    if (WeaponName.Contains(TEXT("AKM")) ||
        WeaponName.Contains(TEXT("M762")) ||
        WeaponName.Contains(TEXT("Groza")) ||
        WeaponName.Contains(TEXT("SKS")) ||
        WeaponName.Contains(TEXT("DP28")))
    {
        return EBulletType::BT_762;
    }
    else if (WeaponName.Contains(TEXT("M416")) ||
        WeaponName.Contains(TEXT("Scar")))
    {
        return EBulletType::BT_556;
    }
    else if (WeaponName.Contains(TEXT("VSS")))
    {
        return EBulletType::BT_9MM;
    }
    else if (WeaponName.Contains(TEXT("AWM")))
    {
        return EBulletType::BT_9MM;
    }
    else if (WeaponName.Contains(TEXT("KAR98K")))
    {
        return EBulletType::BT_762; // KAR98K folosește 7.62 în PUBG
    }
    else if (WeaponName.Contains(TEXT("S686")))
    {
        return EBulletType::BT_762;
    }
    else if (WeaponName.Contains(TEXT("M249")))
    {
        return EBulletType::BT_556;
    }

    return EBulletType::BT_None; // default dacă nu găsește arma
}

void ACPP_Shooter2HUD::DrawShotgunTarget()
{
    if (!Canvas) return;

    FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

    float Radius = 40.0f;             // Distanța de la centru până la arc
    float ArcLengthDegrees = 45.0f;   // Cât de mare este arcul (în grade)
    int Segments = 16;               // Câte segmente are fiecare arc (mai multe => mai fin)
    float LineThickness = 2.0f;
    FLinearColor Color = FLinearColor::Red;

    // Punctul central
    float DotSize = 4.0f;
    DrawRect(Color, Center.X - DotSize * 0.5f, Center.Y - DotSize * 0.5f, DotSize, DotSize);

    // Desenează 4 arce: sus, jos, stânga, dreapta
    TArray<float> ArcCenterAngles = { 270.0f, 90.0f, 180.0f, 0.0f }; // În grade, începând de sus (sens trigonometric)

    for (float ArcCenterDeg : ArcCenterAngles)
    {
        float StartAngleDeg = ArcCenterDeg - (ArcLengthDegrees * 0.5f);
        float Step = ArcLengthDegrees / Segments;

        for (int i = 0; i < Segments; ++i)
        {
            float AngleA = FMath::DegreesToRadians(StartAngleDeg + i * Step);
            float AngleB = FMath::DegreesToRadians(StartAngleDeg + (i + 1) * Step);

            FVector2D PointA = Center + FVector2D(FMath::Cos(AngleA), FMath::Sin(AngleA)) * Radius;
            FVector2D PointB = Center + FVector2D(FMath::Cos(AngleB), FMath::Sin(AngleB)) * Radius;

            DrawLine(PointA.X, PointA.Y, PointB.X, PointB.Y, Color, LineThickness);
        }
    }
}



void ACPP_Shooter2HUD::DrawCrosshair()
{
    if (!Canvas) return;

    FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
    float CrosshairSize = 13.0f;
    float LineThickness = 3.0f; // Ajustează grosimea aici

    FLinearColor CrosshairColor = FLinearColor::Red;

    // Linie orizontală
    DrawLine(Center.X - CrosshairSize, Center.Y, Center.X + CrosshairSize, Center.Y, CrosshairColor, LineThickness);

    // Linie verticală
    DrawLine(Center.X, Center.Y - CrosshairSize, Center.X, Center.Y + CrosshairSize, CrosshairColor, LineThickness);
}

void ACPP_Shooter2HUD::HandleBulletNearbyMessage(FString Message)
{
    if (Message.Contains(TEXT("Esti langa o cutie cu gloante de tip: 5.56mm")))
    {
        Nearby556Count++;
    }
    else if (Message.Contains(TEXT("Ai iesit din zona cutiei cu gloante de tip: 5.56mm")))
    {
        Nearby556Count = FMath::Max(0, Nearby556Count - 1);
    }
    else if (Message.Contains(TEXT("Esti langa o cutie cu gloante de tip: 7.62mm")))
    {
        Nearby762Count++;
    }
    else if (Message.Contains(TEXT("Ai iesit din zona cutiei cu gloante de tip: 7.62mm")))
    {
        Nearby762Count = FMath::Max(0, Nearby762Count - 1);
    }
    else if (Message.Contains(TEXT("Esti langa o cutie cu gloante de tip: 9mm")))
    {
        Nearby9mmCount++;
    }
    else if (Message.Contains(TEXT("Ai iesit din zona cutiei cu gloante de tip: 9mm")))
    {
        Nearby9mmCount = FMath::Max(0, Nearby9mmCount - 1);
    }

    // Actualizează mesajul afișat în HUD, de exemplu afișează primul tip detectat (sau poți face o logică mai complexă)
    if (Nearby556Count > 0)
    {
        BulletMessage = TEXT("Esti langa o cutie cu gloante de tip: 5.56mm");
        bShowBulletIcon = true;
    }
    else if (Nearby762Count > 0)
    {
        BulletMessage = TEXT("Esti langa o cutie cu gloante de tip: 7.62mm");
        bShowBulletIcon = true;
    }
    else if (Nearby9mmCount > 0)
    {
        BulletMessage = TEXT("Esti langa o cutie cu gloante de tip: 9mm");
        bShowBulletIcon = true;
    }
    else
    {
        BulletMessage = TEXT("");
        bShowBulletIcon = false;
    }
}


void ACPP_Shooter2HUD::DrawGrenadeImpactPoint()
{
    APawn* OwningPawn = GetOwningPawn();
    if (!OwningPawn || !Canvas) return;

    // Parametri inițiali ca în AGrenade::BeginPlay()
    FVector StartLocation = OwningPawn->GetActorLocation() + FVector(0, 0, 50); // Înălțimea de lansare
    FVector LaunchVelocity = OwningPawn->GetActorForwardVector() * 2000.f; // Viteza inițială

    FPredictProjectilePathParams PathParams;
    PathParams.StartLocation = StartLocation;
    PathParams.LaunchVelocity = LaunchVelocity;
    PathParams.bTraceComplex = true;
    PathParams.ProjectileRadius = 20.f;  // Conform radiusului din CollisionComponent-ul grenadei
    PathParams.MaxSimTime = 3.0f;        // Timerul exploziei
    PathParams.SimFrequency = 60.0f;     // Update fin
    PathParams.TraceChannel = ECC_WorldStatic;
    PathParams.ActorsToIgnore.Add(OwningPawn);
    PathParams.OverrideGravityZ = -980.f;  // Gravitația standard Unreal

    FPredictProjectilePathResult PathResult;
    bool bHit = UGameplayStatics::PredictProjectilePath(GetWorld(), PathParams, PathResult);
    if (bHit)
    {
       // UE_LOG(LogTemp, Log, TEXT("Nimereste grenada"));
        FVector2D ImpactScreen;
        if (PlayerOwner->ProjectWorldLocationToScreen(PathResult.HitResult.Location, ImpactScreen))
        {
            float Size = 200.f; // Dimensiunea dreptunghiului pe ecran
            DrawRect(FLinearColor::Red, ImpactScreen.X - Size * 0.5f, ImpactScreen.Y - Size * 0.5f, Size, Size);
        }
    }
    else {
       // UE_LOG(LogTemp, Log, TEXT("Nu nimereste grenada"));
    }
}
void ACPP_Shooter2HUD::StartDrinking()
{
    bIsDrinking = true;
    SwayTime = 0.f;

    if (HandMeshComponent)
        HandMeshComponent->SetVisibility(true);
    if (DrinkMeshComponent)
        DrinkMeshComponent->SetVisibility(true);
}
void ACPP_Shooter2HUD::DrawGauge(FVector2D Pos, float Radius, float MaxValue, float Value, FString Label, FColor Color, int NumTicks)
{
    float AngleStart = 150.f;
    float AngleEnd = 390.f;

    // 🔵 Desenează cerc exterior
    for (int i = 0; i <= 360; i += 5)
    {
        float AngleRad = FMath::DegreesToRadians(i);
        FVector2D CurrentPoint = Pos + FVector2D(FMath::Cos(AngleRad), FMath::Sin(AngleRad)) * (Radius + 2.f);

        if (i > 0)
        {
            DrawLine(LastPoint.X, LastPoint.Y, CurrentPoint.X, CurrentPoint.Y, FLinearColor::White, 1.0f);
        }

        LastPoint = CurrentPoint;
    }

    // 🔲 Ticks și valori
    for (int i = 0; i <= NumTicks; ++i)
    {
        float Alpha = i / (float)NumTicks;
        float AngleDeg = FMath::Lerp(AngleStart, AngleEnd, Alpha);
        float AngleRad = FMath::DegreesToRadians(AngleDeg);

        FVector2D Inner = Pos + FVector2D(FMath::Cos(AngleRad), FMath::Sin(AngleRad)) * (Radius * 0.8f);
        FVector2D Outer = Pos + FVector2D(FMath::Cos(AngleRad), FMath::Sin(AngleRad)) * Radius;

        DrawLine(Inner.X, Inner.Y, Outer.X, Outer.Y, FLinearColor::White, 1.f);

        // 🔤 Text pentru fiecare valoare
        float TickValue = FMath::Lerp(0.f, MaxValue, Alpha);
        FString TickText = FString::Printf(TEXT("%.0f"), TickValue);
        FVector2D TextPos = Pos + FVector2D(FMath::Cos(AngleRad), FMath::Sin(AngleRad)) * (Radius + 15.f);
        DrawText(TickText, FColor::White, TextPos.X - 10.f, TextPos.Y - 7.f);
    }

    // ⛳ Indicatorul
    float ValueAlpha = FMath::Clamp(Value / MaxValue, 0.f, 1.f);
    float IndicatorAngle = FMath::Lerp(AngleStart, AngleEnd, ValueAlpha);
    float IndicatorRad = FMath::DegreesToRadians(IndicatorAngle);

    FVector2D End = Pos + FVector2D(FMath::Cos(IndicatorRad), FMath::Sin(IndicatorRad)) * (Radius * 0.9f);
    DrawLine(Pos.X, Pos.Y, End.X, End.Y, Color, 3.f);

    // 🎯 Text pe centru - valoarea
    // Ia fontul default
    UFont* Font = GEngine->GetMediumFont();

    FString ValueText = FString::Printf(TEXT("%d"), FMath::RoundToInt(Value));
    float TextWidth, TextHeight;
    GetTextSize(ValueText, TextWidth, TextHeight, Font);

    float Scale = 2.0f;

    DrawText(ValueText, FColor::White, Pos.X - (TextWidth * Scale) / 2.f, Pos.Y - (TextHeight * Scale) / 2.f, Font, Scale, false);

    float LabelScale = 1.5f;
    DrawText(Label, FColor::White, Pos.X - 40.f, Pos.Y + Radius + 40.f, Font, LabelScale, false);
}
void ACPP_Shooter2HUD::UpdateHUD()
{
    ACPP_Shooter2GameMode* GM = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
    if (GM)
    {
        Coins = GM->GetCurrentCoins();
        UnlockedWeapons = GM->GetCurrentUnlockedWeapons();

        // Acum actualizează UI-ul folosind Coins și Weapons
        UE_LOG(LogTemp, Log, TEXT("HUD update: Coins=%d, Weapons=%d"), Coins, UnlockedWeapons.Num());

        // ... actualizează widget-uri, texte, etc
    }
}

void ACPP_Shooter2HUD::DrawHUD()
{
    Super::DrawHUD();

    APawn* ControlledPawn = GetOwningPawn();
    if (!ControlledPawn) return;
    //afisare banuti pe ecran

    BackButtonPos2 = FVector2D(650.f, 640.f);

    ACPP_Shooter2GameMode* GM = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
    if (GM && Coin_image)
    {
        Coins = GM->GetCurrentCoins();

        // ==== Desenează iconul coin (de 2x mai mare și mai sus) ====
        FVector2D CoinPos(50.f, 20.f);   // mai sus pe ecran
        FVector2D CoinSize(64.f, 64.f);  // 2x mai mare

        FCanvasTileItem CoinTile(CoinPos, Coin_image->Resource, CoinSize, FLinearColor::White);
        CoinTile.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(CoinTile);

        // ==== Desenează valoarea după icon (2x mai mare) ====
        FString CoinsText = FString::Printf(TEXT("%d"), Coins);

        UFont* Font23 = GEngine->GetLargeFont();
        float TextW51, TextH51;
        Canvas->StrLen(Font23, CoinsText, TextW51, TextH51);

        FVector2D TextPos(CoinPos.X + CoinSize.X + 15.f, CoinPos.Y + (CoinSize.Y - TextH51 * 2.f) / 2.f);

        // Scale la 2x pentru text
        FVector2D Scale(2.f, 2.f);

        // Offset-uri pentru contur/bold fake
        FVector2D Offsets[4] = {
            FVector2D(-1.f, 0.f),
            FVector2D(1.f, 0.f),
            FVector2D(0.f, -1.f),
            FVector2D(0.f, 1.f)
        };

        // Desenează outline galben de mai multe ori
        for (auto& Offset : Offsets)
        {
            FCanvasTextItem OutlineItem(TextPos + Offset, FText::FromString(CoinsText), Font23, FLinearColor::Yellow);
            OutlineItem.Scale = Scale;
            Canvas->DrawItem(OutlineItem);
        }

        // Textul principal (galben, cu shadow negru)
        FCanvasTextItem TextItem(TextPos, FText::FromString(CoinsText), Font23, FLinearColor::Yellow);
        TextItem.EnableShadow(FLinearColor::Black, FVector2D(2.f, 2.f));
        TextItem.Scale = Scale;
        Canvas->DrawItem(TextItem);
    }




    // Verifică dacă pawn-ul e o mașină de tip ACar
    ACar* ControlledCar = Cast<ACar>(ControlledPawn);
    if (ControlledCar)
    {
        PlayerInCar = true;
        float Viteza = ControlledCar->viteza;
        float Turatie = ControlledCar->turatie;
        int Treapta = ControlledCar->treapta;

        // Afișare pe ecran
        //FString HudText = FString::Printf(TEXT("Viteza: %.2f m/s | Turație: %.2f | Treapta: %d"),
          //  Viteza, Turatie, Treapta);
        //GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, HudText);
    }
    else
    {
        PlayerInCar = false;
    }
    if (PlayerInCar == true)
    {
        CurrentSpeed = ControlledCar->viteza * 3.6f; // m/s -> km/h
        CurrentRPM = ControlledCar->turatie;
        ACPP_Shooter2GameMode* GM22 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
        if (CurrentSpeed > GM22->GetTopSpeed()) {
            GM22->SetTopSpeed(CurrentSpeed);
        }

        // Cadrane
        FVector2D Center(Canvas->SizeX / 2.f, Canvas->SizeY / 2.f);
        FVector2D RPMPos = FVector2D(Center.X - 150.f, Canvas->SizeY - 300.f);
        FVector2D SpeedPos = FVector2D(Center.X + 250.f, Canvas->SizeY - 300.f);

        DrawGauge(RPMPos, 120.f, 8000.f, CurrentRPM, TEXT("RPM"), FColor::Red,8);
        DrawGauge(SpeedPos, 120.f, 240.f, CurrentSpeed, TEXT("KM/H"), FColor::Blue,12);
    }

    //Aici fac logica pentru a afisa meniul principal
    ACPP_Shooter2GameMode* GM2 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
    int32 EngineLevel = 1;
    int32 TurboLevel = 1;
    int32 WeightLevel = 1;
    int32 HealthLevel = 1;
    if (GM2)
    {
        EngineLevel = GM2->GetCurrentEngineLevel();
        TurboLevel = GM2->GetCurrentTurboLevel();
        WeightLevel = GM2->GetCurrentWeightLevel();
        HealthLevel = GM2->GetCurrentHealthLevel();

        UE_LOG(LogTemp, Log, TEXT("Current upgrades: Engine=%d, Turbo=%d, Weight=%d"),
            EngineLevel, TurboLevel, WeightLevel);
    }


    if (!bShowMainMenu && bShowStore && Canvas)
    {
        UFont* Font = GEngine->GetLargeFont();
        FVector2D CarPos(100.f, 100.f); // poziția pe ecran
        FVector2D CarSize(470.f, 220.f);

        if (Car_image)
        {
            FCanvasTileItem TileItem(CarPos, Car_image->Resource, CarSize, FLinearColor::White);
            TileItem.BlendMode = SE_BLEND_Translucent;
            Canvas->DrawItem(TileItem);
        }

        // ==== Barele de upgrade ====
        FVector2D StartPos(650.f, 200.f); // unde încep barele
        FVector2D BarSize(300.f, 25.f);
        float BarSpacing = 80.f; // distanța între bare

        struct FUpgradeBar
        {
            FString Label;
            int32 Level;
        };

        TArray<FUpgradeBar> Bars;
        Bars.AddDefaulted();
        Bars.Last().Label = TEXT("Engine");
        Bars.Last().Level = EngineLevel;

        Bars.AddDefaulted();
        Bars.Last().Label = TEXT("Turbo");
        Bars.Last().Level = TurboLevel;

        Bars.AddDefaulted();
        Bars.Last().Label = TEXT("Weight");
        Bars.Last().Level = WeightLevel;

        Bars.AddDefaulted(); 
        Bars.Last().Label = TEXT("Health"); 
        Bars.Last().Level = HealthLevel; // Noua bară


        for (int i = 0; i < Bars.Num(); i++)
        {
            FVector2D BarPos = StartPos + FVector2D(0.f, i * BarSpacing);

            // Fundal bară (gri)
            DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f), BarPos.X, BarPos.Y, BarSize.X, BarSize.Y);

            // Umplere bară în funcție de level (1–20)
            if (i < 3) {
                float FillPercent = FMath::Clamp(Bars[i].Level / 20.f, 0.f, 1.f);
                DrawRect(FLinearColor(0.f, 0.7f, 0.f, 0.9f), BarPos.X, BarPos.Y, BarSize.X * FillPercent, BarSize.Y);
            }
            if (i == 3)
            {
                float FillPercent2 = FMath::Clamp(Bars[i].Level / 51.f, 0.f, 1.f);
                DrawRect(FLinearColor(0.f, 0.7f, 0.f, 0.9f), BarPos.X, BarPos.Y, BarSize.X* FillPercent2, BarSize.Y);
            }

            // Text cu numele și level-ul
            float TextW, TextH;
            if (i < 3) {
                FString Label = FString::Printf(TEXT("%s: %d / 20"), *Bars[i].Label, Bars[i].Level);
               
                Canvas->StrLen(Font, Label, TextW, TextH);
                FVector2D TextPos(BarPos.X + (BarSize.X - TextW) / 2.f, BarPos.Y - TextH - 5.f);

                FCanvasTextItem TextItem(TextPos, FText::FromString(Label), Font, FLinearColor::White);
                TextItem.EnableShadow(FLinearColor::Black);
                Canvas->DrawItem(TextItem);
            }
            else
            {
                FString Label = FString::Printf(TEXT("%s: %d / 51"), *Bars[i].Label, Bars[i].Level);
                Canvas->StrLen(Font, Label, TextW, TextH);
                FVector2D TextPos(BarPos.X + (BarSize.X - TextW) / 2.f, BarPos.Y - TextH - 5.f);

                FCanvasTextItem TextItem(TextPos, FText::FromString(Label), Font, FLinearColor::White);
                TextItem.EnableShadow(FLinearColor::Black);
                Canvas->DrawItem(TextItem);
            }

            // ==== BUTON Upgrade ====
            FVector2D BtnPos(BarPos.X, BarPos.Y + BarSize.Y + 5.f);
            FVector2D BtnSize(100.f, 30.f);

            DrawRect(FLinearColor(0.f, 0.5f, 1.f, 0.8f), BtnPos.X, BtnPos.Y, BtnSize.X, BtnSize.Y);

            FString BtnLabel = TEXT("Upgrade");
            Canvas->StrLen(Font, BtnLabel, TextW, TextH);
            FVector2D BtnTextPos(BtnPos.X + (BtnSize.X - TextW) / 2.f, BtnPos.Y + (BtnSize.Y - TextH) / 2.f);

            FCanvasTextItem BtnText(BtnTextPos, FText::FromString(BtnLabel), Font, FLinearColor::White);
            BtnText.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(BtnText);

            // ==== PREȚUL în dreapta butonului ====
            FString PriceText = FString::Printf(TEXT("%d"), 1);
            if (i < 2)
            {
                PriceText = FString::Printf(TEXT("%d"), Bars[i].Level*10);
            }
            if (i == 2) {
                PriceText = FString::Printf(TEXT("%d"), Bars[i].Level*7);
            }
            if (i == 3)
            {
                PriceText = FString::Printf(TEXT("%d"), Bars[i].Level);
            }
            Canvas->StrLen(Font, PriceText, TextW, TextH);

            // Scale dorit (2x mai mare)
            FVector2D Scale(2.f, 2.f);

            // Ajustăm poziția verticală pentru a rămâne centrată cu butonul
            FVector2D PricePos(BtnPos.X + BtnSize.X + 10.f, BtnPos.Y + (BtnSize.Y - TextH * Scale.Y) / 2.f);

            FCanvasTextItem PriceItem(PricePos, FText::FromString(PriceText), Font, FLinearColor::Yellow);
            PriceItem.EnableShadow(FLinearColor::Black);

            // Aplicăm scale-ul
            PriceItem.Scale = Scale;

            Canvas->DrawItem(PriceItem);
            // ==== ICON COIN după preț ====
            // ==== ICON COIN după preț ====
            if (Coin_image)
            {
                FVector2D CoinSize(40.f, 40.f); // mai mare decât înainte
                // Poziția X: după text, luând în calcul scale-ul textului
                FVector2D CoinPos(PricePos.X + TextW * PriceItem.Scale.X + 0.f,
                    PricePos.Y + (TextH * PriceItem.Scale.Y - CoinSize.Y) / 2.f);

                FCanvasTileItem CoinTile(CoinPos, Coin_image->Resource, CoinSize, FLinearColor::White);
                CoinTile.BlendMode = SE_BLEND_Translucent;
                Canvas->DrawItem(CoinTile);
            }
        }
        // ==== Buton Back ====
        FVector2D BackPos(650.f, StartPos.Y + Bars.Num() * BarSpacing + 120.f);
        FVector2D BackSize(150.f, 40.f);

        DrawRect(FLinearColor(0.8f, 0.f, 0.f, 0.8f), BackPos.X, BackPos.Y, BackSize.X, BackSize.Y);

        FString BackLabel = TEXT("Back");
        float TextW, TextH;
        Canvas->StrLen(Font, BackLabel, TextW, TextH);
        FVector2D BackTextPos(BackPos.X + (BackSize.X - TextW) / 2.f, BackPos.Y + (BackSize.Y - TextH) / 2.f);

        FCanvasTextItem BackText(BackTextPos, FText::FromString(BackLabel), Font, FLinearColor::White);
        BackText.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(BackText);
        UpgradeButtonPositions.Empty();
        UpgradeButtonSizes.Empty();

        for (int i = 0; i < Bars.Num(); i++)
        {
            FVector2D BtnPos(650.f, 200.f + i * BarSpacing + BarSize.Y + 5.f);
            FVector2D BtnSize(100.f, 30.f);

            // ... aici desenai deja butonul

            UpgradeButtonPositions.Add(BtnPos);
            UpgradeButtonSizes.Add(BtnSize);
        }

        // Back button
        BackButtonPos = FVector2D(650.f, StartPos.Y + Bars.Num() * BarSpacing + 120.f);
        BackButtonSize = FVector2D(150.f, 40.f);

        //butoane de cumparat arme
        // 
        // ==== Butoane de cumpărat arme (în dreapta) ====
       // ==== Coloana de arme mai la dreapta ====
        FVector2D BtnSize(100.f, 30.f);
        FVector2D WeaponsStartPos(UpgradeButtonPositions[0].X + BtnSize.X + 460.f, 20.f); // mai la dreapta
        FVector2D WeaponBtnSize(120.f, 40.f);
        float WeaponBtnSpacing = 80.f;
        WeaponButtonPositions.Empty();
        WeaponButtonSizes.Empty();

        for (int i = 0; i < WeaponNames.Num(); i++)
        {
            FVector2D BtnPos = WeaponsStartPos + FVector2D(0.f, i * WeaponBtnSpacing);

            // ==== FUNDAL ALBASTRU ====
            FVector2D BackgroundSize(176.f, 80.f);
            FVector2D BackgroundPos(BtnPos.X - BackgroundSize.X - 10.f, BtnPos.Y + (WeaponBtnSize.Y - BackgroundSize.Y) / 2.f);

            DrawRect(FLinearColor(0.f, 0.f, 1.f, 0.8f), BackgroundPos.X + 5.f, BackgroundPos.Y + 5.f, BackgroundSize.X - 10.f, BackgroundSize.Y - 10.f);

            // ==== ICON ARMA ====
            if (WeaponIcons.IsValidIndex(i) && WeaponIcons[i])
            {
                FVector2D IconSize(176.f, 80.f);
                FVector2D IconPos = BackgroundPos;
                FCanvasTileItem IconTile(IconPos, WeaponIcons[i]->Resource, IconSize, FLinearColor::White);
                IconTile.BlendMode = SE_BLEND_Translucent;
                Canvas->DrawItem(IconTile);
            }

            // ==== BUTON ====
            DrawRect(FLinearColor(0.f, 0.5f, 1.f, 0.8f), BtnPos.X, BtnPos.Y, WeaponBtnSize.X, WeaponBtnSize.Y);

            // ==== Text pe buton (numele armei) ====
            float TextW100, TextH100;
            Canvas->StrLen(Font, WeaponNames[i], TextW100, TextH100);
            FVector2D BtnTextPos(BtnPos.X + (WeaponBtnSize.X - TextW100) / 2.f, BtnPos.Y + (WeaponBtnSize.Y - TextH100) / 2.f);

            FCanvasTextItem BtnText(BtnTextPos, FText::FromString(WeaponNames[i]), Font, FLinearColor::White);
            BtnText.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(BtnText);

            // ==== PREȚ sau OWNED ====
            FString PriceText;
            if (GM && GM->GetOwnedWeaponNames().Contains(WeaponNames[i]))
            {
                PriceText = TEXT("Owned");
            }
            else
            {
                PriceText = FString::Printf(TEXT("%d"), WeaponPrices[i]);
            }

            Canvas->StrLen(Font, PriceText, TextW100, TextH100);
            FVector2D PricePos(BtnPos.X + WeaponBtnSize.X + 10.f, BtnPos.Y + (WeaponBtnSize.Y - TextH100) / 2.f);

            FCanvasTextItem PriceItem(PricePos, FText::FromString(PriceText), Font,
                PriceText == TEXT("Owned") ? FLinearColor::Green : FLinearColor::Yellow);
            PriceItem.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(PriceItem);

            // ==== Icon moneda doar dacă nu e "Owned" ====
            if (PriceText != TEXT("Owned") && Coin_image)
            {
                FVector2D CoinSize(30.f, 30.f);
                FVector2D CoinPos(PricePos.X + TextW100 + 5.f, PricePos.Y + (TextH100 - CoinSize.Y) / 2.f);
                FCanvasTileItem CoinTile(CoinPos, Coin_image->Resource, CoinSize, FLinearColor::White);
                CoinTile.BlendMode = SE_BLEND_Translucent;
                Canvas->DrawItem(CoinTile);
            }

            WeaponButtonPositions.Add(BtnPos);
            WeaponButtonSizes.Add(WeaponBtnSize);
        }


        //parte de afisare grenade in store
        // ==== Iconițe Grenade/Molotov/Smoke + butoane Buy ====
// Poziția de start (sub mașină)
        FVector2D ThrowablesStartPos(CarPos.X, CarPos.Y + CarSize.Y + 50.f);
        FVector2D IconSize(80.f, 80.f);
        FVector2D BuyBtnSize(100.f, 30.f);
        float ThrowablesSpacing = 150.f; // distanța între coloane (icon+buton)

        struct FThrowableItem
        {
            UTexture2D* Icon;
            FString Label;
            int32 Price;
        };

        // Array cu cele 3 iteme
        TArray<FThrowableItem> Throwables;
        Throwables.Add({ GrenadeIcon, TEXT("Grenade"), 40 });
        Throwables.Add({ Slot4Icon, TEXT("Molotov"), 35 });
        Throwables.Add({ WeaponIcon5,   TEXT("Smoke"),   15 });

        ThrowableButtonPositions.Empty();
        ThrowableButtonSizes.Empty();

        for (int i = 0; i < Throwables.Num(); i++)
        {
            FVector2D IconPos = ThrowablesStartPos + FVector2D(i * ThrowablesSpacing, 0.f);

            // Desenăm iconul
            if (Throwables[i].Icon)
            {
                FCanvasTileItem IconTile(IconPos, Throwables[i].Icon->Resource, IconSize, FLinearColor::White);
                IconTile.BlendMode = SE_BLEND_Translucent;
                Canvas->DrawItem(IconTile);
            }

            // Buton Buy sub icon
            FVector2D BtnPos = IconPos + FVector2D((IconSize.X - BuyBtnSize.X) / 2.f, IconSize.Y + 10.f);
            DrawRect(FLinearColor(0.f, 0.5f, 1.f, 0.8f), BtnPos.X, BtnPos.Y, BuyBtnSize.X, BuyBtnSize.Y);

            FString BtnLabel = TEXT("Buy");
            float TextW1500, TextH1500;
            Canvas->StrLen(Font, BtnLabel, TextW1500, TextH1500);
            FVector2D BtnTextPos(BtnPos.X + (BuyBtnSize.X - TextW1500) / 2.f, BtnPos.Y + (BuyBtnSize.Y - TextH1500) / 2.f);

            FCanvasTextItem BtnText(BtnTextPos, FText::FromString(BtnLabel), Font, FLinearColor::White);
            BtnText.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(BtnText);

            // Preț + coin lângă buton
            FString PriceText = FString::Printf(TEXT("%d"), Throwables[i].Price);
            Canvas->StrLen(Font, PriceText, TextW1500, TextH1500);
            FVector2D PricePos(BtnPos.X + BuyBtnSize.X + 10.f, BtnPos.Y + (BuyBtnSize.Y - TextH) / 2.f);

            FCanvasTextItem PriceItem(PricePos, FText::FromString(PriceText), Font, FLinearColor::Yellow);
            PriceItem.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(PriceItem);

            if (Coin_image)
            {
                FVector2D CoinSize(30.f, 30.f);
                FVector2D CoinPos(PricePos.X + TextW + 5.f, PricePos.Y + (TextH - CoinSize.Y) / 2.f);
                FCanvasTileItem CoinTile(CoinPos, Coin_image->Resource, CoinSize, FLinearColor::White);
                CoinTile.BlendMode = SE_BLEND_Translucent;
                Canvas->DrawItem(CoinTile);
            }

            // Salvăm pozițiile pentru click detection
            ThrowableButtonPositions.Add(BtnPos);
            ThrowableButtonSizes.Add(BuyBtnSize);
        }


        // ==== Iconițe pentru gloanțe + butoane Buy ====
// Poziția de start (sub grenade)
        FVector2D AmmoStartPos(ThrowablesStartPos.X, ThrowablesStartPos.Y + IconSize.Y + BuyBtnSize.Y + 80.f);
        FVector2D AmmoIconSize(80.f, 80.f);
        FVector2D AmmoBuyBtnSize(100.f, 30.f);
        float AmmoSpacing = 170.f; // distanța între coloane

        struct FAmmoItem
        {
            UTexture2D* Icon;
            FString Label;
            int32 Price;
        };

        TArray<FAmmoItem> AmmoItems;
        AmmoItems.Add({ Slot556Icon, TEXT("5.56mm"), 100 });
        AmmoItems.Add({ Slot762Icon, TEXT("7.62mm"), 100 });
        AmmoItems.Add({ Slot9Icon, TEXT("9mm"), 100 });

        AmmoButtonPositions.Empty();
        AmmoButtonSizes.Empty();

        for (int i = 0; i < AmmoItems.Num(); i++)
        {
            FVector2D IconPos = AmmoStartPos + FVector2D(i * AmmoSpacing, 0.f);

            // Desenăm iconul gloanțelor
            if (AmmoItems[i].Icon)
            {
                FCanvasTileItem IconTile(IconPos, AmmoItems[i].Icon->Resource, AmmoIconSize, FLinearColor::White);
                IconTile.BlendMode = SE_BLEND_Translucent;
                Canvas->DrawItem(IconTile);
            }

            // Buton Buy sub icon
            FVector2D BtnPos = IconPos + FVector2D((AmmoIconSize.X - AmmoBuyBtnSize.X) / 2.f, AmmoIconSize.Y + 10.f);
            DrawRect(FLinearColor(0.f, 0.5f, 1.f, 0.8f), BtnPos.X, BtnPos.Y, AmmoBuyBtnSize.X, AmmoBuyBtnSize.Y);

            FString BtnLabel = TEXT("Buy");
            float TextWAmmo, TextHAmmo;
            Canvas->StrLen(Font, BtnLabel, TextWAmmo, TextHAmmo);
            FVector2D BtnTextPos(BtnPos.X + (AmmoBuyBtnSize.X - TextWAmmo) / 2.f, BtnPos.Y + (AmmoBuyBtnSize.Y - TextHAmmo) / 2.f);

            FCanvasTextItem BtnText(BtnTextPos, FText::FromString(BtnLabel), Font, FLinearColor::White);
            BtnText.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(BtnText);

            // Preț + coin
            FString PriceText = FString::Printf(TEXT("%d"), AmmoItems[i].Price);
            Canvas->StrLen(Font, PriceText, TextWAmmo, TextHAmmo);
            FVector2D PricePos(BtnPos.X + AmmoBuyBtnSize.X + 10.f, BtnPos.Y + (AmmoBuyBtnSize.Y - TextHAmmo) / 2.f);

            FCanvasTextItem PriceItem(PricePos, FText::FromString(PriceText), Font, FLinearColor::Yellow);
            PriceItem.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(PriceItem);

            if (Coin_image)
            {
                FVector2D CoinSize(30.f, 30.f);
                FVector2D CoinPos(PricePos.X + TextWAmmo + 5.f, PricePos.Y + (TextHAmmo - CoinSize.Y) / 2.f);
                FCanvasTileItem CoinTile(CoinPos, Coin_image->Resource, CoinSize, FLinearColor::White);
                CoinTile.BlendMode = SE_BLEND_Translucent;
                Canvas->DrawItem(CoinTile);
            }

            // Salvăm pozițiile pentru click detection
            AmmoButtonPositions.Add(BtnPos);
            AmmoButtonSizes.Add(AmmoBuyBtnSize);
        }
    }
    if (bShowAchievements && Canvas && !bShowMainMenu && !bShowSettings && !bShowStore)
    {
        UFont* Font = GEngine->GetLargeFont();

        // ==== CONSTANTE DE LAYOUT ====
        const float AchievementWidth = 200.f; // bara de progres
        const float AchievementHeight = 30.f;
        const float BtnWidth = 100.f;
        const float BtnHeight = 30.f;
        const float BtnSpacing = 20.f;
        const float RowSpacing = 76.f; // mai mare pentru dreptunghi
        const float ColumnSpacing = 525.f; // coloanele mai departate
        const float RectPadding = 10.f; // padding pentru dreptunghi

        float TextW, TextH;

        // Centru pe ecran
        float CenterX = Canvas->SizeX / 2.f;
        float StartY = 32.f;

        // Coloanele: stanga si dreapta, mutate 200 px la stanga
        FVector2D ColumnPositions[2] = {
            FVector2D(CenterX - AchievementWidth - ColumnSpacing / 2.f - 200.f, StartY),
            FVector2D(CenterX + ColumnSpacing / 2.f - 200.f, StartY)
        };

        struct FAchievement
        {
            FString Name;
            FString Description;
            float Progress;
            bool bClaimed;
            int Price;
            int CurrentStage;    // Pentru Muscle Car achievement
            int MaxStage;        // Pentru Muscle Car achievement
            bool bIsMultiStage;  // Pentru a identifica achievementurile cu stagii
        };

        TArray<FAchievement> Achievements;
        ACPP_Shooter2GameMode* GM20 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
        
        // ==== MUSCLE CAR ACHIEVEMENT - Multi-stage ====
        // Presupunem că ai variabile pentru nivelul motorului și stagiul achievementului
        int CurrentEngineLevel = GM20->GetCurrentEngineLevel(); // înlocuiește cu GetCurrentEngineLevel()
        int CurrentTurboLevel = GM20->GetCurrentTurboLevel();
        int CurrentWeightLevel = GM20->GetCurrentWeightLevel();
        int CurrentHealthLevel = GM20->GetCurrentHealthLevel();
        float TopSpeed = GM20->GetTopSpeed();
        float CarDistance = GM20->GetCarDistance();
        int CurrentKills = GM20->GetTotalKills();
        int Games = GM20->GetGames();
        int Money = (CurrentEngineLevel + 1) * CurrentEngineLevel * 10 / 2 + CurrentTurboLevel * (CurrentTurboLevel + 1) * 10 / 2 +
            CurrentWeightLevel * (CurrentWeightLevel + 1) * 7 / 2 + CurrentHealthLevel * (CurrentHealthLevel + 1) / 2;
        float MaxKillDistance = GM20->GetMaxKillDistance();
        int TotalGrenades =GM20->GetTotalGrenades();
        float PlayerDistance = GM20->GetPlayerDistance();
        int NumberDies = GM20->GetNumberDies();
        int BossesKilled = GM20->GetBossesKilled();
        int WeaponCount = (GM20->GetOwnedWeaponNames()).Num();

        // Această variabilă trebuie să fie salvată persistent în joc
        // Pentru demo, folosesc o valoare statică - în realitate ar trebui salvată
        int MuscleCarStage = GM20->GetAchievementState(0); // Stagiul curent: 1, 2, 3, sau 4
        int TurboCarStage = GM20->GetAchievementState(1);
        int WeightCarStage = GM20->GetAchievementState(2);
        int HealthStage = GM20->GetAchievementState(3);
        int SpeedStage = GM20->GetAchievementState(4);
        int DistanceStage = GM20->GetAchievementState(5);
        int KillStage = GM20->GetAchievementState(6);
        int DamageStage = GM20->GetAchievementState(7);
        int GameStage = GM20->GetAchievementState(8);
        int MoneyStage = GM20->GetAchievementState(9);
        int RangeStage = GM20->GetAchievementState(10);
        int GrenadeStage = GM20->GetAchievementState(11);
        int WalkingStage = GM20->GetAchievementState(12);
        int DieStage = GM20->GetAchievementState(13);
        int BossStage = GM20->GetAchievementState(14);
        int WeaponStage = GM20->GetAchievementState(15);
        // Nivelurile țintă pentru fiecare stagiu
        int StageTargets[4] = { 5, 10, 15, 20 };
        FString StageNames[4] = { TEXT("Muscle Car I"), TEXT("Muscle Car II"), TEXT("Muscle Car III"), TEXT("Muscle Car IV") };

        // Calculăm progresul pentru stagiul curent
        int CurrentTarget = StageTargets[MuscleCarStage];
        float StageProgress = FMath::Clamp((float)CurrentEngineLevel / (float)CurrentTarget, 0.f, 1.f);
        bool CanClaim = (CurrentEngineLevel >= CurrentTarget && MuscleCarStage <= 4);

        FAchievement MuscleCarAch;
        MuscleCarAch.Name = (MuscleCarStage <= 3) ? StageNames[MuscleCarStage] : TEXT("Muscle Car Complete");
        MuscleCarAch.Description = (MuscleCarStage <= 3) ?
            FString::Printf(TEXT("Upgrade your engine to level %d"), CurrentTarget) :
            TEXT("All engine upgrades completed!");
        MuscleCarAch.Progress = StageProgress;
        MuscleCarAch.bClaimed = CanClaim;
        MuscleCarAch.Price = (MuscleCarStage <= 3) ? StagePrices[MuscleCarStage] : 0;
        MuscleCarAch.CurrentStage = MuscleCarStage;
        MuscleCarAch.MaxStage = 4;
        MuscleCarAch.bIsMultiStage = true;
        Achievements.Add(MuscleCarAch);

        //parte turbo
        FString StageNames2[4] = { TEXT("Full Throttle I"), TEXT("Full Throttle II"), TEXT("Full Throttle III"), TEXT("Full Throttle IV") };
        int CurrentTarget2 = StageTargets[TurboCarStage];
        float StageProgress2 = FMath::Clamp((float)CurrentTurboLevel / (float)CurrentTarget2, 0.f, 1.f);
        bool CanClaim2 = (CurrentTurboLevel >= CurrentTarget2 && TurboCarStage <= 4);


        FAchievement TurboCarAch;
        TurboCarAch.Name = (TurboCarStage <= 3) ? StageNames2[TurboCarStage] : TEXT("Turbo Car Complete");
        TurboCarAch.Description = (TurboCarStage <= 3) ?
            FString::Printf(TEXT("Upgrade your turbo to level %d"), CurrentTarget2) :
            TEXT("All turbo upgrades completed!");
        TurboCarAch.Progress = StageProgress2;
        TurboCarAch.bClaimed = CanClaim2;
        TurboCarAch.Price = (TurboCarStage <= 3) ? StagePrices[TurboCarStage] : 0;
        TurboCarAch.CurrentStage = TurboCarStage;
        TurboCarAch.MaxStage = 4;
        TurboCarAch.bIsMultiStage = true;
        Achievements.Add(TurboCarAch);

        //parte weight
        FString StageNames3[4] = { TEXT("Like a Feather I"), TEXT("Like a Feather II"), TEXT("Like a Feather III"), TEXT("Like a Feather IV") };
        int CurrentTarget3 = StageTargets[WeightCarStage];
        float StageProgress3 = FMath::Clamp((float)CurrentWeightLevel / (float)CurrentTarget3, 0.f, 1.f);
        bool CanClaim3 = (CurrentWeightLevel >= CurrentTarget3 && WeightCarStage <= 4);

        FAchievement WeightCarAch;
        WeightCarAch.Name = (WeightCarStage <= 3) ? StageNames3[WeightCarStage] : TEXT("Weight Car Complete");
        WeightCarAch.Description = (WeightCarStage <= 3) ?
            FString::Printf(TEXT("Upgrade your weight to level %d"), CurrentTarget3) :
            TEXT("All weight upgrades completed!");
        WeightCarAch.Progress = StageProgress3;
        WeightCarAch.bClaimed = CanClaim3;
        WeightCarAch.Price = (WeightCarStage <= 3) ? StagePrices[WeightCarStage] : 0;
        WeightCarAch.CurrentStage = WeightCarStage;
        WeightCarAch.MaxStage = 4;
        WeightCarAch.bIsMultiStage = true;
        Achievements.Add(WeightCarAch);

        //parte HP
        //iron heart
        FString StageNames5[5] = { TEXT("Iron Heart I"), TEXT("Iron Heart II"), TEXT("Iron Heart III"), TEXT("Iron Heart IV"), TEXT("Iron Heart V") };
        int StageTargets5[5] = { 10, 20, 30, 40, 50 };
        int CurrentTarget5 = StageTargets5[HealthStage];
        float StageProgress5 = FMath::Clamp((float)CurrentHealthLevel / (float)CurrentTarget5, 0.f, 1.f);
        bool CanClaim5 = (CurrentHealthLevel >= CurrentTarget5 && HealthStage <= 5);

        FAchievement HealthAch;
        HealthAch.Name = (HealthStage <= 4) ? StageNames5[HealthStage] : TEXT("HP maxed out");
        HealthAch.Description = (HealthStage <= 4) ?
            FString::Printf(TEXT("Upgrade your health to level %d"), CurrentTarget5) :
            TEXT("All HP upgrades completed!");
        HealthAch.Progress = StageProgress5;
        HealthAch.bClaimed = CanClaim5;
        HealthAch.Price = (HealthStage <= 4) ? StagePrices5[HealthStage] : 0;
        HealthAch.CurrentStage = HealthStage;
        HealthAch.MaxStage = 5;
        HealthAch.bIsMultiStage = true;
        Achievements.Add(HealthAch);

        //speed
        FString StageNames6[8] = { TEXT("Speed of Light I"), TEXT("Speed of Light II"), TEXT("Speed of Light III"), 
            TEXT("Speed of Light IV"), TEXT("Speed of Light V"), TEXT("Speed of Light VI"), TEXT("Speed of Light VII"), TEXT("Speed of Light VIII") };
        int StageTargets6[8] = { 180, 215, 250, 285, 320, 355, 390, 425 };
        int CurrentTarget6 = StageTargets6[SpeedStage];
        float StageProgress6 = FMath::Clamp((float)TopSpeed / (float)CurrentTarget6, 0.f, 1.f);
        bool CanClaim6 = ((int)TopSpeed >= CurrentTarget6 && SpeedStage <= 8);

        FAchievement SpeedAch;
        SpeedAch.Name = (SpeedStage <= 7) ? StageNames6[SpeedStage] : TEXT("You are fast!");
        SpeedAch.Description = (SpeedStage <= 7) ?
            FString::Printf(TEXT("Reach top speed of %d"), CurrentTarget6) :
            TEXT("All speed records completed!");
        SpeedAch.Progress = StageProgress6;
        SpeedAch.bClaimed = CanClaim6;
        SpeedAch.Price = (SpeedStage <= 7) ? StagePrices6[SpeedStage] : 0;
        SpeedAch.CurrentStage = SpeedStage;
        SpeedAch.MaxStage = 8;
        SpeedAch.bIsMultiStage = true;
        Achievements.Add(SpeedAch);


        //distance in car
        FString StageNames7[8] = { TEXT("Baby Steps"), TEXT("Learn to drive"), TEXT("First 5 kilometers"),
            TEXT("Good driver"), TEXT("Explorer"), TEXT("Between cities"), TEXT("Addicted"), TEXT("Trucker") };
        int CurrentTarget7 = StageTargets7[DistanceStage];
        float StageProgress7 = FMath::Clamp((float)CarDistance / (float)CurrentTarget7, 0.f, 1.f);
        bool CanClaim7 = ((int)CarDistance >= CurrentTarget7 && DistanceStage <= 8);

        FAchievement DistanceAch;
        DistanceAch.Name = (DistanceStage <= 7) ? StageNames7[DistanceStage] : TEXT("Long runner");
        DistanceAch.Description = (DistanceStage <= 7) ?
            FString::Printf(TEXT("Travel in a car for %d km"), CurrentTarget7/1000) :
            TEXT("You covered 300 km of distance");
        DistanceAch.Progress = StageProgress7;
        DistanceAch.bClaimed = CanClaim7;
        DistanceAch.Price = (DistanceStage <= 7) ? StagePrices7[DistanceStage] : 0;
        DistanceAch.CurrentStage = DistanceStage;
        DistanceAch.MaxStage = 8;
        DistanceAch.bIsMultiStage = true;
        Achievements.Add(DistanceAch);

        //kills
        FString StageNames8[8] = { TEXT("First Blood"), TEXT("Rookie"), TEXT("Novice"),
           TEXT("Apprentice"), TEXT("Challenger"), TEXT("Expert"), TEXT("Master Assasin"), TEXT("Legend") };
        int CurrentTarget8 = StageTargets8[KillStage];
        float StageProgress8 = FMath::Clamp((float)CurrentKills / (float)CurrentTarget8, 0.f, 1.f);
        bool CanClaim8 = ((int)CurrentKills >= CurrentTarget8 && KillStage <= 8);

        FAchievement KillAch;
        KillAch.Name = (KillStage <= 7) ? StageNames8[KillStage] : TEXT("Serial killer");
        KillAch.Description = (KillStage <= 7) ?
            FString::Printf(TEXT("Kill %d enemies"), CurrentTarget8) :
            TEXT("You killed everyone!");
        KillAch.Progress = StageProgress8;
        KillAch.bClaimed = CanClaim8;
        KillAch.Price = (KillStage <= 7) ? StagePrices8[KillStage] : 0;
        KillAch.CurrentStage = KillStage;
        KillAch.MaxStage = 8;
        KillAch.bIsMultiStage = true;
        Achievements.Add(KillAch);

        //damage dealt
        FString StageNames9[8] = { TEXT("First Bullet"), TEXT("Rookie"), TEXT("Novice"),
           TEXT("Well Known"), TEXT("Challenger"), TEXT("Military Squad"), TEXT("Born To Be Assasin"), TEXT("God of damage") };
        int CurrentTarget9 = StageTargets9[DamageStage];
        float StageProgress9 = FMath::Clamp((float)CurrentKills*120.f / (float)CurrentTarget9, 0.f, 1.f);
        bool CanClaim9 = ((int)CurrentKills*120 >= CurrentTarget9 && DamageStage <= 8);

        FAchievement DamageAch;
        DamageAch.Name = (DamageStage <= 7) ? StageNames9[DamageStage] : TEXT("Fully armed");
        DamageAch.Description = (DamageStage <= 7) ?
            FString::Printf(TEXT("Deal %d damage to enemies"), CurrentTarget9) :
            TEXT("Damage lord");
        DamageAch.Progress = StageProgress9;
        DamageAch.bClaimed = CanClaim9;
        DamageAch.Price = (DamageStage <= 7) ? StagePrices9[DamageStage] : 0;
        DamageAch.CurrentStage = DamageStage;
        DamageAch.MaxStage = 8;
        DamageAch.bIsMultiStage = true;
        Achievements.Add(DamageAch);

        //open the game how many times
        FString StageNames10[8] = { TEXT("First Game"), TEXT("Rookie"), TEXT("Novice"),
           TEXT("Player"), TEXT("Addicted"), TEXT("Love the game"), TEXT("Tryhard"), TEXT("Go touch some grass") };
        int CurrentTarget10 = StageTargets10[GameStage];
        float StageProgress10 = FMath::Clamp((float)Games / (float)CurrentTarget10, 0.f, 1.f);
        bool CanClaim10 = ((int)Games >= CurrentTarget10 && GameStage <= 8);

        FAchievement GameAch;
        GameAch.Name = (GameStage <= 7) ? StageNames10[GameStage] : TEXT("True player");
        GameAch.Description = (GameStage <= 7) ?
            FString::Printf(TEXT("Enter %d times in the game"), CurrentTarget10) :
            TEXT("Player");
        GameAch.Progress = StageProgress10;
        GameAch.bClaimed = CanClaim10;
        GameAch.Price = (GameStage <= 7) ? StagePrices10[GameStage] : 0;
        GameAch.CurrentStage = GameStage;
        GameAch.MaxStage = 8;
        GameAch.bIsMultiStage = true;
        Achievements.Add(GameAch);

        //moneystage si money
        FString StageNames11[8] = { TEXT("First Purchase"), TEXT("Rookie"), TEXT("Novice"),
           TEXT("High Spender"), TEXT("Rich"), TEXT("Millionaire"), TEXT("Tryhard"), TEXT("All maxed") };
        int CurrentTarget11 = StageTargets11[MoneyStage];
        float StageProgress11 = FMath::Clamp((float)Money / (float)CurrentTarget11, 0.f, 1.f);
        bool CanClaim11 = ((int)Money >= CurrentTarget11 && MoneyStage <= 8);

        FAchievement MoneyAch;
        MoneyAch.Name = (MoneyStage <= 7) ? StageNames11[MoneyStage] : TEXT("Spender");
        MoneyAch.Description = (MoneyStage <= 7) ?
            FString::Printf(TEXT("Spend %d money in the game"), CurrentTarget11) :
            TEXT("Player");
        MoneyAch.Progress = StageProgress11;
        MoneyAch.bClaimed = CanClaim11;
        MoneyAch.Price = (MoneyStage <= 7) ? StagePrices11[MoneyStage] : 0;
        MoneyAch.CurrentStage = MoneyStage;
        MoneyAch.MaxStage = 8;
        MoneyAch.bIsMultiStage = true;
        Achievements.Add(MoneyAch);

        //max kill distance
        FString StageNames12[8] = { TEXT("Close Range"), TEXT("A few steps away"), TEXT("Novice"),
          TEXT("Pretty Accurate"), TEXT("First Sniper"), TEXT("Long Range"), TEXT("Tryhard"), TEXT("Sharpshooter") };
        int CurrentTarget12 = StageTargets12[RangeStage];
        float StageProgress12 = FMath::Clamp((float)MaxKillDistance / (float)CurrentTarget12, 0.f, 1.f);
        bool CanClaim12 = ((int)MaxKillDistance >= CurrentTarget12 && RangeStage <= 8);

        FAchievement RangeAch;
        RangeAch.Name = (RangeStage <= 7) ? StageNames12[RangeStage] : TEXT("Very long range");
        RangeAch.Description = (RangeStage <= 7) ?
            FString::Printf(TEXT("Kill a bot from %d meters away"), CurrentTarget12) :
            TEXT("Player");
        RangeAch.Progress = StageProgress12;
        RangeAch.bClaimed = CanClaim12;
        RangeAch.Price = (RangeStage <= 7) ? StagePrices12[RangeStage] : 0;
        RangeAch.CurrentStage = RangeStage;
        RangeAch.MaxStage = 8;
        RangeAch.bIsMultiStage = true;
        Achievements.Add(RangeAch);

        //grenade achievement
        FString StageNames13[8] = { TEXT("First Toss"), TEXT("Baby Boom"), TEXT("Thrower"),
         TEXT("Fragment fan"), TEXT("Blast Master"), TEXT("Explosive Expert"), TEXT("Master of Mayhem"), TEXT("The Walking Explosion") };
        int CurrentTarget13 = StageTargets13[GrenadeStage];
        float StageProgress13 = FMath::Clamp((float)TotalGrenades / (float)CurrentTarget13, 0.f, 1.f);
        bool CanClaim13 = ((int)TotalGrenades >= CurrentTarget13 && GrenadeStage <= 8);

        FAchievement GrenadeAch;
        GrenadeAch.Name = (GrenadeStage <= 7) ? StageNames13[GrenadeStage] : TEXT("TNT Guy");
        GrenadeAch.Description = (GrenadeStage <= 7) ?
            FString::Printf(TEXT("Throw %d grenades of any type."), CurrentTarget13) :
            TEXT("Player");
        GrenadeAch.Progress = StageProgress13;
        GrenadeAch.bClaimed = CanClaim13;
        GrenadeAch.Price = (GrenadeStage <= 7) ? StagePrices13[GrenadeStage] : 0;
        GrenadeAch.CurrentStage = GrenadeStage;
        GrenadeAch.MaxStage = 8;
        GrenadeAch.bIsMultiStage = true;
        Achievements.Add(GrenadeAch);


        //Walking achievement
        FString StageNames14[8] = { TEXT("First Steps"), TEXT("On the move"), TEXT("Traveler"),
         TEXT("Pathfinder"), TEXT("Wanderer"), TEXT("Nomad"), TEXT("Marathoner"), TEXT("Globetrotter") };
        int CurrentTarget14 = StageTargets14[WalkingStage];
        float StageProgress14 = FMath::Clamp((float)PlayerDistance / (float)CurrentTarget14, 0.f, 1.f);
        bool CanClaim14 = ((int)PlayerDistance >= CurrentTarget14 && WalkingStage <= 8);

        FAchievement WalkingAch;
        WalkingAch.Name = (WalkingStage <= 7) ? StageNames14[WalkingStage] : TEXT("Too many steps");
        WalkingAch.Description = (WalkingStage <= 7) ?
            FString::Printf(TEXT("Walk %d meters on the map."), CurrentTarget14) :
            TEXT("Player");
        WalkingAch.Progress = StageProgress14;
        WalkingAch.bClaimed = CanClaim14;
        WalkingAch.Price = (WalkingStage <= 7) ? StagePrices14[WalkingStage] : 0;
        WalkingAch.CurrentStage = WalkingStage;
        WalkingAch.MaxStage = 8;
        WalkingAch.bIsMultiStage = true;
        Achievements.Add(WalkingAch);


        //numberdies achievement
        FString StageNames15[8] = { TEXT("First Die"), TEXT("Just A Scratch"), TEXT("Learning the Hard Way"),
        TEXT("Fallen Fighter"), TEXT("Respawn Regular"), TEXT("Death Collector"), TEXT("Veteran of Afterlife"), TEXT("Legend of the Fallen") };
        int CurrentTarget15 = StageTargets15[DieStage];
        float StageProgress15 = FMath::Clamp((float)NumberDies / (float)CurrentTarget15, 0.f, 1.f);
        bool CanClaim15 = ((int)NumberDies >= CurrentTarget15 && DieStage <= 8);

        FAchievement DieAch;
        DieAch.Name = (DieStage <= 7) ? StageNames15[DieStage] : TEXT("Respawn master");
        DieAch.Description = (DieStage <= 7) ?
            FString::Printf(TEXT("Die % d times"), CurrentTarget15) :
            TEXT("Player");
        DieAch.Progress = StageProgress15;
        DieAch.bClaimed = CanClaim15;
        DieAch.Price = (DieStage <= 7) ? StagePrices15[DieStage] : 0;
        DieAch.CurrentStage = DieStage;
        DieAch.MaxStage = 8;
        DieAch.bIsMultiStage = true;
        Achievements.Add(DieAch);

        //bosses killed
        FString StageNames16[8] = { TEXT("First Win"), TEXT("Boss Hunter"), TEXT("Slayer"),
        TEXT("Legendary Fighter"), TEXT("Conqueror"), TEXT("Dreaded Nemesis"), TEXT("Boss Exterminator"), TEXT("Ultimate Vanquisher") };
        int CurrentTarget16 = StageTargets16[BossStage];
        float StageProgress16 = FMath::Clamp((float)BossesKilled / (float)CurrentTarget16, 0.f, 1.f);
        bool CanClaim16 = ((int)BossesKilled >= CurrentTarget16 && BossStage <= 8);

        FAchievement BossAch;
        BossAch.Name = (BossStage <= 7) ? StageNames16[BossStage] : TEXT("Ultimate boss killer");
        BossAch.Description = (BossStage <= 7) ?
            FString::Printf(TEXT("Kill the big boss %d times"), CurrentTarget16) :
            TEXT("Player");
        BossAch.Progress = StageProgress16;
        BossAch.bClaimed = CanClaim16;
        BossAch.Price = (BossStage <= 7) ? StagePrices16[BossStage] : 0;
        BossAch.CurrentStage = BossStage;
        BossAch.MaxStage = 8;
        BossAch.bIsMultiStage = true;
        Achievements.Add(BossAch);

        //Achievement arme detinute
        FString StageNames17[5] = { TEXT("First Steel"), TEXT("Armed and Ready"), TEXT("Arsenal Builder"),
        TEXT("Weapon Master"), TEXT("Walking Armory")};
        int CurrentTarget17 = StageTargets17[WeaponStage];
        float StageProgress17 = FMath::Clamp((float)WeaponCount / (float)CurrentTarget17, 0.f, 1.f);
        bool CanClaim17 = ((int)WeaponCount >= CurrentTarget17 && WeaponStage <= 5);

        FAchievement WeaponAch;
        WeaponAch.Name = (WeaponStage <= 4) ? StageNames17[WeaponStage] : TEXT("Fully loaded, ready to fight");
        WeaponAch.Description = (WeaponStage <= 4) ?
            FString::Printf(TEXT("Buy weapons %d times"), CurrentTarget17) :
            TEXT("Player");
        WeaponAch.Progress = StageProgress17;
        WeaponAch.bClaimed = CanClaim17;
        WeaponAch.Price = (WeaponStage <= 4) ? StagePrices17[WeaponStage] : 0;
        WeaponAch.CurrentStage = WeaponStage;
        WeaponAch.MaxStage = 5;
        WeaponAch.bIsMultiStage = true;
        Achievements.Add(WeaponAch);



        // ==== ALTE ACHIEVEMENTURI (restul de 7) ====
        for (int i = 16; i < 16; i++)
        {
            FAchievement Ach;
            Ach.Name = FString::Printf(TEXT("Achievement %d"), i + 1);
            Ach.Description = FString::Printf(TEXT("This is a short description for achievement %d"), i + 1);
            Ach.Progress = FMath::Clamp((float)(i % 10) / 10.f, 0.f, 1.f);
            Ach.bClaimed = false;
            Ach.Price = (i + 1) * 10;
            Ach.CurrentStage = 1;
            Ach.MaxStage = 1;
            Ach.bIsMultiStage = false;
            Achievements.Add(Ach);
        }

        AchievementButtonPositions.Empty();
        AchievementButtonSizes.Empty();
        AchievementBarPositions.Empty();
        AchievementBarSizes.Empty();

        for (int i = 0; i < Achievements.Num(); i++)
        {
            int Column = i / 8;
            int Row = i % 8;

            FVector2D BasePos = ColumnPositions[Column] + FVector2D(0.f, Row * RowSpacing);

            // ==== Măsurăm textul pentru a ști dimensiunea dreptunghiului ====
            Canvas->StrLen(Font, Achievements[i].Name, TextW, TextH);
            float NameWidth = TextW;
            float NameHeight = TextH;

            Canvas->StrLen(Font, Achievements[i].Description, TextW, TextH);
            float DescWidth = TextW;
            float DescHeight = TextH;

            float RectWidth = FMath::Max(NameWidth, FMath::Max(DescWidth, AchievementWidth)) + 3.f * AchievementWidth / 4.f + 3 * RectPadding;
            float RectHeight = AchievementHeight + NameHeight + DescHeight + 2.3f * RectPadding;

            // ==== Dreptunghi fundal ====
            // Culoare diferită pentru Muscle Car achievement
            FLinearColor BgColor = (i < 16) ? FLinearColor(0.2f, 0.1f, 0.05f, 0.8f) : FLinearColor(0.15f, 0.15f, 0.15f, 0.8f);
            DrawRect(BgColor, BasePos.X, BasePos.Y + 5.f, RectWidth, RectHeight - 10.f);

            // ==== Text nume ====
            FVector2D TextPos(BasePos.X + RectPadding, BasePos.Y + RectPadding);
            // Culoare diferită pentru Muscle Car achievement
            FLinearColor NameColor = (i < 16) ? FLinearColor(1.f, 0.65f, 0.f, 1.f) : FLinearColor::White;
            FCanvasTextItem TextItem(TextPos, FText::FromString(Achievements[i].Name), Font, NameColor);
            TextItem.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(TextItem);

            // ==== Afișăm stagiul pentru Muscle Car ====
            if (Achievements[i].bIsMultiStage && Achievements[i].CurrentStage <= Achievements[i].MaxStage)
            {
                FString StageText = FString::Printf(TEXT("Stage %d/%d"), Achievements[i].CurrentStage, Achievements[i].MaxStage);
                Canvas->StrLen(Font, StageText, TextW, TextH);
                FVector2D StagePos(BasePos.X + RectWidth - TextW - RectPadding, BasePos.Y + RectPadding);
                FCanvasTextItem StageTextItem(StagePos, FText::FromString(StageText), Font, FLinearColor(0.8f, 0.8f, 0.8f, 1.f));
                StageTextItem.EnableShadow(FLinearColor::Black);
                Canvas->DrawItem(StageTextItem);
            }

            // ==== Text descriere ====
            FVector2D DescTextPos(BasePos.X + RectPadding, BasePos.Y + RectPadding + NameHeight + 5.f);
            FCanvasTextItem DescTextItem(DescTextPos, FText::FromString(Achievements[i].Description), Font, FLinearColor::Gray);
            DescTextItem.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(DescTextItem);

            // ==== Bara de progres ====
            FVector2D BarPos(BasePos.X + RectPadding, BasePos.Y + RectPadding + NameHeight + DescHeight + 10.f);
            FVector2D BarSize(AchievementWidth, AchievementHeight);
            DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f), BarPos.X, BarPos.Y, BarSize.X, BarSize.Y);

            // Culoare diferită pentru bara de progres a Muscle Car
            FLinearColor ProgressColor = (i < 16) ? FLinearColor(1.f, 0.5f, 0.f, 0.9f) : FLinearColor(0.f, 0.8f, 0.f, 0.9f);
            DrawRect(ProgressColor, BarPos.X, BarPos.Y, BarSize.X * Achievements[i].Progress, BarSize.Y);

            // ==== Buton Claim ====
            FVector2D BtnPos(BasePos.X + RectWidth + BtnSpacing, BarPos.Y);

            // Pentru Muscle Car, butonul se resetează la fiecare stagiu nou
            bool CanClaimCurrent = Achievements[i].bClaimed && (Achievements[i].CurrentStage <= Achievements[i].MaxStage);
            FLinearColor BtnColor;
            FString BtnLabel;

            //if (i == 0 || i==1 || i==2 || i==3 || i==4 || i==5 || i==6 || i==7 || i==8 || i==9 || i==10 || ) // Muscle Car achievement
            if(i<16)
            {
                if (Achievements[i].CurrentStage >= Achievements[i].MaxStage)
                {
                    BtnColor = FLinearColor(0.3f, 0.8f, 0.3f, 0.8f);
                    BtnLabel = TEXT("Complete");
                }
                else if (CanClaimCurrent)
                {
                    BtnColor = FLinearColor(0.f, 0.8f, 0.f, 0.8f);
                    BtnLabel = TEXT("Claim");
                }
                else
                {
                    BtnColor = FLinearColor(0.3f, 0.3f, 0.3f, 0.8f);
                    BtnLabel = TEXT("Locked");
                }
            }
            else 
            {
                BtnColor = Achievements[i].bClaimed ? FLinearColor(0.2f, 0.7f, 0.2f, 0.8f) : FLinearColor(0.f, 0.5f, 1.f, 0.8f);
                BtnLabel = Achievements[i].bClaimed ? TEXT("Claimed") : TEXT("Claim");
            }

            DrawRect(BtnColor, BtnPos.X, BtnPos.Y, BtnWidth, BtnHeight);

            Canvas->StrLen(Font, BtnLabel, TextW, TextH);
            FVector2D BtnTextPos(BtnPos.X + (BtnWidth - TextW) / 2.f, BtnPos.Y + (BtnHeight - TextH) / 2.f);
            FCanvasTextItem BtnText(BtnTextPos, FText::FromString(BtnLabel), Font, FLinearColor::White);
            BtnText.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(BtnText);

            // ==== Preț și iconiță coin ====
            if (Achievements[i].Price > 0)
            {
                FString PriceText = FString::Printf(TEXT("%d"), Achievements[i].Price);
                Canvas->StrLen(Font, PriceText, TextW, TextH);
                FVector2D PricePos(BtnPos.X + BtnWidth + 10.f, BtnPos.Y + (BtnHeight - TextH) / 2.f);
                FCanvasTextItem PriceItem(PricePos, FText::FromString(PriceText), Font, FLinearColor::Yellow);
                PriceItem.EnableShadow(FLinearColor::Black);
                Canvas->DrawItem(PriceItem);

                if (Coin_image)
                {
                    FVector2D CoinSize(30.f, 30.f);
                    FVector2D CoinPos(PricePos.X + TextW + 5.f, PricePos.Y + (TextH - CoinSize.Y) / 2.f);
                    FCanvasTileItem CoinTile(CoinPos, Coin_image->Resource, CoinSize, FLinearColor::White);
                    CoinTile.BlendMode = SE_BLEND_Translucent;
                    Canvas->DrawItem(CoinTile);
                }
            }

            // ==== Salvăm pozițiile pentru click ====
            AchievementButtonPositions.Add(BtnPos);
            AchievementButtonSizes.Add(FVector2D(BtnWidth, BtnHeight));
            AchievementBarPositions.Add(BarPos);
            AchievementBarSizes.Add(BarSize);
        }

        // ==== Buton Back ====
        FVector2D BackBtnPos(650.f, 640.f);
        FVector2D BackBtnSize(150.f, 40.f);
        DrawRect(FLinearColor(0.8f, 0.f, 0.f, 0.8f), BackBtnPos.X, BackBtnPos.Y, BackBtnSize.X, BackBtnSize.Y);

        Canvas->StrLen(Font, TEXT("Back"), TextW, TextH);
        FVector2D BackTextPos(BackBtnPos.X + (BackBtnSize.X - TextW) / 2.f, BackBtnPos.Y + (BackBtnSize.Y - TextH) / 2.f);
        FCanvasTextItem BackTextItem(BackTextPos, FText::FromString(TEXT("Back")), Font, FLinearColor::White);
        BackTextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(BackTextItem);

        BackButtonPos3 = BackBtnPos;
        BackButtonSize3 = BackBtnSize;
    }









    //settings
    if (bShowSettings && Canvas)
    {
        UFont* SettingsFont = GEngine->GetLargeFont();

        // ==== CONSTANTE DE LAYOUT ====
        const float BarWidth = 300.f;
        const float BarHeight = 30.f;
        const float BtnWidth = 50.f;
        const float BtnHeight = 30.f;
        const float BtnSpacing = 20.f;
        const float SectionSpacing = 100.f;

        float TextW, TextH;

        // Centru pe orizontal
        float CenterX = Canvas->SizeX / 2.f;

        // Y de start de sus în jos
        float CurrentY = 200.f;

        // ==== LUMINOZITATE ====
        FVector2D BrightnessBarPos(CenterX - BarWidth / 2.f, CurrentY);

        DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f), BrightnessBarPos.X, BrightnessBarPos.Y, BarWidth, BarHeight);
        float BrightnessFillPercent = FMath::Clamp((float)BrightnessLevel / 7, 0.f, 1.f);
        DrawRect(FLinearColor(1.f, 1.f, 0.f, 0.9f), BrightnessBarPos.X, BrightnessBarPos.Y, BarWidth * BrightnessFillPercent, BarHeight);

        FString BrightnessLabel = FString::Printf(TEXT("Brightness: %d / %d"), BrightnessLevel, 7);
        Canvas->StrLen(SettingsFont, BrightnessLabel, TextW, TextH);
        FVector2D BrightnessTextPos(BrightnessBarPos.X + (BarWidth - TextW) / 2.f, BrightnessBarPos.Y - TextH - 5.f);
        FCanvasTextItem BrightnessTextItem(BrightnessTextPos, FText::FromString(BrightnessLabel), SettingsFont, FLinearColor::White);
        BrightnessTextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(BrightnessTextItem);

        // ==== LUMINOZITATE ====

 // Butoane +/- luminozitate
        FVector2D BrightnessDecBtnPos(BrightnessBarPos.X - BtnWidth - BtnSpacing, BrightnessBarPos.Y);
        FVector2D BrightnessIncBtnPos(BrightnessBarPos.X + BarWidth + BtnSpacing, BrightnessBarPos.Y);
        DrawRect(FLinearColor(0.f, 0.5f, 1.f, 0.8f), BrightnessDecBtnPos.X, BrightnessDecBtnPos.Y, BtnWidth, BtnHeight);
        DrawRect(FLinearColor(0.f, 0.5f, 1.f, 0.8f), BrightnessIncBtnPos.X, BrightnessIncBtnPos.Y, BtnWidth, BtnHeight);

        // "-" mai mare
        FCanvasTextItem BrightnessDecText(BrightnessDecBtnPos, FText::FromString(TEXT("-")), SettingsFont, FLinearColor::White);
        BrightnessDecText.Scale = FVector2D(2.f, 2.f);
        BrightnessDecText.EnableShadow(FLinearColor::Black);
        float DecTextW, DecTextH;
        Canvas->StrLen(SettingsFont, TEXT("-"), DecTextW, DecTextH);
        BrightnessDecText.Position = FVector2D(
            BrightnessDecBtnPos.X + (BtnWidth - DecTextW * BrightnessDecText.Scale.X) / 2.f,
            BrightnessDecBtnPos.Y + (BtnHeight - DecTextH * BrightnessDecText.Scale.Y) / 2.f
        );
        Canvas->DrawItem(BrightnessDecText);

        // "+" mai mare
        FCanvasTextItem BrightnessIncText(BrightnessIncBtnPos, FText::FromString(TEXT("+")), SettingsFont, FLinearColor::White);
        BrightnessIncText.Scale = FVector2D(2.f, 2.f);
        BrightnessIncText.EnableShadow(FLinearColor::Black);
        float IncTextW, IncTextH;
        Canvas->StrLen(SettingsFont, TEXT("+"), IncTextW, IncTextH);
        BrightnessIncText.Position = FVector2D(
            BrightnessIncBtnPos.X + (BtnWidth - IncTextW * BrightnessIncText.Scale.X) / 2.f,
            BrightnessIncBtnPos.Y + (BtnHeight - IncTextH * BrightnessIncText.Scale.Y) / 2.f
        );
        Canvas->DrawItem(BrightnessIncText);

        BrightnessButtonPositions.Empty();
        BrightnessButtonSizes.Empty();
        BrightnessButtonPositions.Add(BrightnessDecBtnPos);
        BrightnessButtonPositions.Add(BrightnessIncBtnPos);
        BrightnessButtonSizes.Add(FVector2D(BtnWidth, BtnHeight));
        BrightnessButtonSizes.Add(FVector2D(BtnWidth, BtnHeight));

        CurrentY += SectionSpacing;

        // ==== VOLUM ====
        FVector2D VolumeBarPos(CenterX - BarWidth / 2.f, CurrentY);

        DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f), VolumeBarPos.X, VolumeBarPos.Y, BarWidth, BarHeight);
        float VolumeFillPercent = FMath::Clamp((float)VolumeLevel / 10, 0.f, 1.f);
        DrawRect(FLinearColor(0.f, 0.8f, 1.f, 0.9f), VolumeBarPos.X, VolumeBarPos.Y, BarWidth* VolumeFillPercent, BarHeight);

        FString VolumeLabel = FString::Printf(TEXT("Volume: %d / %d"), VolumeLevel, 10);
        Canvas->StrLen(SettingsFont, VolumeLabel, TextW, TextH);
        FVector2D VolumeTextPos(VolumeBarPos.X + (BarWidth - TextW) / 2.f, VolumeBarPos.Y - TextH - 5.f);
        FCanvasTextItem VolumeTextItem(VolumeTextPos, FText::FromString(VolumeLabel), SettingsFont, FLinearColor::White);
        VolumeTextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(VolumeTextItem);

        FVector2D VolumeDecBtnPos(VolumeBarPos.X - BtnWidth - BtnSpacing, VolumeBarPos.Y);
        FVector2D VolumeIncBtnPos(VolumeBarPos.X + BarWidth + BtnSpacing, VolumeBarPos.Y);
        DrawRect(FLinearColor(0.f, 0.5f, 1.f, 0.8f), VolumeDecBtnPos.X, VolumeDecBtnPos.Y, BtnWidth, BtnHeight);
        DrawRect(FLinearColor(0.f, 0.5f, 1.f, 0.8f), VolumeIncBtnPos.X, VolumeIncBtnPos.Y, BtnWidth, BtnHeight);

        // "-" mai mare
        FCanvasTextItem VolumeDecText(VolumeDecBtnPos, FText::FromString(TEXT("-")), SettingsFont, FLinearColor::White);
        VolumeDecText.Scale = FVector2D(2.f, 2.f);
        VolumeDecText.EnableShadow(FLinearColor::Black);
        float VolDecW, VolDecH;
        Canvas->StrLen(SettingsFont, TEXT("-"), VolDecW, VolDecH);
        VolumeDecText.Position = FVector2D(
            VolumeDecBtnPos.X + (BtnWidth - VolDecW * VolumeDecText.Scale.X) / 2.f,
            VolumeDecBtnPos.Y + (BtnHeight - VolDecH * VolumeDecText.Scale.Y) / 2.f
        );
        Canvas->DrawItem(VolumeDecText);

        // "+" mai mare
        FCanvasTextItem VolumeIncText(VolumeIncBtnPos, FText::FromString(TEXT("+")), SettingsFont, FLinearColor::White);
        VolumeIncText.Scale = FVector2D(2.f, 2.f);
        VolumeIncText.EnableShadow(FLinearColor::Black);
        float VolIncW, VolIncH;
        Canvas->StrLen(SettingsFont, TEXT("+"), VolIncW, VolIncH);
        VolumeIncText.Position = FVector2D(
            VolumeIncBtnPos.X + (BtnWidth - VolIncW * VolumeIncText.Scale.X) / 2.f,
            VolumeIncBtnPos.Y + (BtnHeight - VolIncH * VolumeIncText.Scale.Y) / 2.f
        );
        Canvas->DrawItem(VolumeIncText);

        VolumeButtonPositions.Empty();
        VolumeButtonSizes.Empty();
        VolumeButtonPositions.Add(VolumeDecBtnPos);
        VolumeButtonPositions.Add(VolumeIncBtnPos);
        VolumeButtonSizes.Add(FVector2D(BtnWidth, BtnHeight));
        VolumeButtonSizes.Add(FVector2D(BtnWidth, BtnHeight));
        CurrentY += SectionSpacing;

        // ==== GRAPHICS QUALITY ====
        TArray<FString> GraphicsQualityLevels = { TEXT("Low"), TEXT("Medium"), TEXT("High"), TEXT("Ultra") };
        GraphicsButtonPositions.Empty();
        GraphicsButtonSizes.Empty();

        float GraphicsStartX = CenterX - ((GraphicsQualityLevels.Num() * 100.f + (GraphicsQualityLevels.Num() - 1) * 20.f) / 2.f);

        for (int32 i = 0; i < GraphicsQualityLevels.Num(); i++)
        {
            FVector2D BtnPos(GraphicsStartX + i * (100.f + 20.f), CurrentY);
            FVector2D BtnSize(100.f, 30.f);
            FLinearColor BtnColor = (i == CurrentGraphicsQuality) ? FLinearColor(1.f, 1.f, 0.f, 0.9f) : FLinearColor(0.f, 0.5f, 1.f, 0.8f);

            DrawRect(BtnColor, BtnPos.X, BtnPos.Y, BtnSize.X, BtnSize.Y);

            Canvas->StrLen(SettingsFont, GraphicsQualityLevels[i], TextW, TextH);
            FVector2D TextPos(BtnPos.X + (BtnSize.X - TextW) / 2.f, BtnPos.Y + (BtnSize.Y - TextH) / 2.f);
            FCanvasTextItem TextItem(TextPos, FText::FromString(GraphicsQualityLevels[i]), SettingsFont, FLinearColor::White);
            TextItem.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(TextItem);

            GraphicsButtonPositions.Add(BtnPos);
            GraphicsButtonSizes.Add(BtnSize);
        }

        CurrentY += SectionSpacing;

        // ==== RESET PLAYER DATA BUTTON ====
        FVector2D ResetBtnSize(150.f, 40.f);
        ResetPlayerDataButtonPos = FVector2D(CenterX - ResetBtnSize.X / 2.f, CurrentY);

        DrawRect(FLinearColor(1.f, 0.4f, 0.f, 0.8f), ResetPlayerDataButtonPos.X, ResetPlayerDataButtonPos.Y, ResetBtnSize.X, ResetBtnSize.Y);

        Canvas->StrLen(SettingsFont, TEXT("Reset Player Data"), TextW, TextH);
        FVector2D ResetTextPos(ResetPlayerDataButtonPos.X + (ResetBtnSize.X - TextW) / 2.f, ResetPlayerDataButtonPos.Y + (ResetBtnSize.Y - TextH) / 2.f);
        FCanvasTextItem ResetTextItem(ResetTextPos, FText::FromString(TEXT("Reset Player Data")), SettingsFont, FLinearColor::White);
        ResetTextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(ResetTextItem);

        ResetPlayerDataButtonSize = ResetBtnSize;



        // ==== BACK BUTTON ====
        FVector2D BackBtnSize(150.f, 40.f);

        DrawRect(FLinearColor(0.8f, 0.f, 0.f, 0.8f), BackButtonPos2.X, BackButtonPos2.Y, BackBtnSize.X, BackBtnSize.Y);

        Canvas->StrLen(SettingsFont, TEXT("Back"), TextW, TextH);
        FVector2D BackTextPos(BackButtonPos2.X + (BackBtnSize.X - TextW) / 2.f, BackButtonPos2.Y + (BackBtnSize.Y - TextH) / 2.f);
        FCanvasTextItem BackTextItem(BackTextPos, FText::FromString(TEXT("Back")), SettingsFont, FLinearColor::White);
        BackTextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(BackTextItem);

        BackButtonSize2 = BackBtnSize;
    }










    if (bShowMainMenu && Canvas)
    {
        UFont* Font = GEngine->GetLargeFont();

        // Setări generale
        FVector2D ButtonSize(200.f, 50.f);
        float ButtonSpacing = 20.f;

        // Poziția de start, centrată pe ecran
        FVector2D ScreenCenter(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
        FVector2D MenuStartPos(ScreenCenter.X - ButtonSize.X * 0.5f, ScreenCenter.Y - (ButtonSize.Y * 2 + ButtonSpacing * 1.5f));
        // (4 butoane, așezate vertical)

        // Numele butoanelor
        TArray<FString> ButtonLabels = { TEXT("Play"),TEXT("Store"), TEXT("Achievements"), TEXT("Settings"), TEXT("Exit")};

        // Pozițiile butoanelor pentru click handling (opțional)
        MenuButtonPositions.Empty();

        for (int32 i = 0; i < ButtonLabels.Num(); ++i)
        {
            FVector2D ButtonPos = MenuStartPos + FVector2D(0.f, i * (ButtonSize.Y + ButtonSpacing));

            // Salvăm poziția pentru interacțiune
            MenuButtonPositions.Add(ButtonPos);

            // Desenăm dreptunghiul butonului
            DrawRect(FLinearColor(0.f, 0.5f, 1.f, 0.8f), ButtonPos.X, ButtonPos.Y, ButtonSize.X, ButtonSize.Y);

            // Desenăm textul butonului centrat
            FString Label = ButtonLabels[i];
            float TextW, TextH;
            Canvas->StrLen(Font, Label, TextW, TextH);

            FVector2D TextPos = ButtonPos + FVector2D((ButtonSize.X - TextW) / 2.f, (ButtonSize.Y - TextH) / 2.f);

            FCanvasTextItem TextItem(TextPos, FText::FromString(Label), Font, FLinearColor::White);
            TextItem.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(TextItem);
        }
    }











    if (bIsDrinking == true)
    {
        AnimateEnergyDrink(5.0f, 4.0f);
    }

    EnergyDrainTimer += GetWorld()->GetDeltaSeconds();

    // Dacă au trecut 3 secunde, scădem energia
    if (EnergyDrainTimer >= EnergyDrainInterval)
    {
        CurrentEnergy = FMath::Max(0.f, CurrentEnergy - 1.f);
        EnergyDrainTimer = 0.f;

        UE_LOG(LogTemp, Log, TEXT("Energy drained! CurrentEnergy: %f"), CurrentEnergy);
        if (CurrentHealth < 100 && CurrentEnergy > 0)
            CurrentHealth++;
    }
    if (!Canvas) return;
    //imi iau damage ca player
    //bara de energie
    
    float MidX2 = Canvas->ClipX / 2.0f;

    // Dimensiuni generale
    float HealthBarWidth2 = Canvas->ClipX / 3.0f;
    float HealthBarHeight2 = 40.0f;
    float EnergyBarHeight2 = 10.0f;

    float HealthBarX2 = MidX2 - (HealthBarWidth2 / 2.0f);
    float HealthBarY2 = Canvas->ClipY - 210.0f;

    float EnergyBarX2 = HealthBarX2;
    float EnergyBarY2 = HealthBarY2 + HealthBarHeight2 + 10.0f;

    float EnergyRatio = FMath::Clamp(CurrentEnergy / MaxEnergy, 0.0f, 1.0f);
    float FilledEnergyWidth = HealthBarWidth2 * EnergyRatio;

    // Culori
    FLinearColor StartColor = FLinearColor::Yellow;
    FLinearColor EndColor = FLinearColor(1.f, 0.3f, 0.f);

    // Parametri segmente
    int NumberSegments = 4;
    float SegmentTotalWidth = HealthBarWidth2 / NumberSegments;
    float SegmentPadding = 2.0f; // spațiu între segmente
    float SegmentInnerWidth = SegmentTotalWidth - SegmentPadding;

    // 🔲 1. Desenează contur alb + margine neagră
    for (int i = 0; i < NumberSegments; ++i)
    {
        float SegmentStartX = EnergyBarX2 + i * SegmentTotalWidth;

        // Contur negru subțire
        DrawRect(FLinearColor::Black, SegmentStartX, EnergyBarY2, SegmentTotalWidth, EnergyBarHeight2);

        // Interior alb (gol)
        DrawRect(FLinearColor::White, SegmentStartX + 1.0f, EnergyBarY2 + 1.0f, SegmentInnerWidth - 2.0f, EnergyBarHeight2 - 2.0f);
    }

    // 🌈 2. Umple segmentele în funcție de energie
    for (int i = 0; i < NumberSegments; ++i)
    {
        float SegmentStartX = EnergyBarX2 + i * SegmentTotalWidth;

        float T = (float)i / (NumberSegments - 1);
        FLinearColor SegmentColor = FLinearColor::LerpUsingHSV(StartColor, EndColor, T);

        float SegmentStartOffset = i * SegmentTotalWidth;
        float SegmentEndOffset = FMath::Min((i + 1) * SegmentTotalWidth, FilledEnergyWidth);
        float SegmentFillWidth = SegmentEndOffset - SegmentStartOffset;

        if (SegmentFillWidth > 0.f)
        {
            // Umple segmentul doar în interior, cu padding
            float FillX = SegmentStartX + 2.0f;
            float FillY = EnergyBarY2 + 2.0f;
            float FillW = FMath::Max(0.0f, SegmentFillWidth - 4.0f);
            float FillH = EnergyBarHeight2 - 4.0f;

            DrawRect(SegmentColor, FillX, FillY, FillW, FillH);
        }
    }





    //pun sange pe ecran si imi iau damage
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (BloodTexture && CurrentTime < BloodOverlayEndTime)
    {
        // Desenăm textura pe tot ecranul
        float ScreenX = Canvas->SizeX;
        float ScreenY = Canvas->SizeY;

        float Duration = BloodOverlayEndTime - (CurrentTime - GetWorld()->DeltaTimeSeconds); // adică 2.0 sec
        float Elapsed = BloodOverlayEndTime - CurrentTime;
        float Progress = 1.0f - (Elapsed / Duration);

        // Intensitate pulsatorie folosind sin()
        float Pulse = FMath::Sin(CurrentTime * 10.f) * 0.5f + 0.5f; // variază între 0 și 1

        // Aplicăm și un fade-out liniar spre final
        float FadeAlpha = FMath::Clamp(Elapsed / Duration, 0.0f, 1.0f);

        // Alpha final = fade * puls
        float FinalAlpha = Pulse * FadeAlpha;

        FCanvasTileItem Tile(FVector2D(0, 0), BloodTexture->Resource, FVector2D(ScreenX, ScreenY), FLinearColor(1.f, 1.f, 1.f, FinalAlpha));
        Tile.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(Tile);
    }
    //dau sunet de miscare daca ma misc:

    //reloading cu animatie
    if (bIsReloading)
    {
        ReloadTimer += GetWorld()->GetDeltaSeconds();
        float Progress = FMath::Clamp(ReloadTimer / ReloadDuration, 0.f, 1.f);

        FVector2D Center(Canvas->SizeX / 2, Canvas->SizeY / 2);

        // Text sub cerc
        FString ReloadText = FString::Printf(TEXT("Reloading %s..."), *ReloadingWeaponName);
        DrawText(ReloadText, FLinearColor::White, Center.X - 70.f, Center.Y + 60.f, nullptr, 1.5f);

        // Cerc progres
        float Radius = 50.f;
        int32 Segments = 64;
        float AngleStep = 2 * PI / Segments;

        for (int32 i = 0; i < Segments * Progress; ++i)
        {
            float Angle1 = i * AngleStep;
            float Angle2 = (i + 1) * AngleStep;

            FVector2D Dir1 = FVector2D(FMath::Cos(Angle1), FMath::Sin(Angle1));
            FVector2D Dir2 = FVector2D(FMath::Cos(Angle2), FMath::Sin(Angle2));

            FVector2D Point1 = Center + Dir1 * Radius;
            FVector2D Point2 = Center + Dir2 * Radius;

            // Simulează grosimea: 3 linii paralele (grosime 3px)
            for (int Offset = -7; Offset <= 7; ++Offset)
            {
                FVector2D OffsetDir = FVector2D(-Dir1.Y, Dir1.X) * Offset;
                DrawLine(Point1.X + OffsetDir.X, Point1.Y + OffsetDir.Y,
                    Point2.X + OffsetDir.X, Point2.Y + OffsetDir.Y,
                    FLinearColor::White);
            }
        }

        // Finalizează reîncărcarea
        if (ReloadTimer >= ReloadDuration)
        {
            bIsReloading = false;
            EBulletType BulletType = Match_Weapon_with_bullets(ReloadingWeaponName);
            ReloadWeapon(BulletType, SelectedWeaponSlot);
        }
    }


    APawn* PlayerPawn = GetOwningPawn();
    if (!PlayerPawn) return;

    FVector Velocity = PlayerPawn->GetVelocity();
    float Speed = Velocity.Size();

    bool bNewIsMoving = Speed > 5.0f; // Evităm fluctuațiile minore

    if (bNewIsMoving != bIsMoving2)
    {
        bIsMoving2 = bNewIsMoving;

        if (bIsMoving2)
        {
           // UE_LOG(LogTemp, Log, TEXT("Ma misc"));
            if (WalkingAudioComponent && !WalkingAudioComponent->IsPlaying() && VolumeLevel > 0)
            {
                WalkingAudioComponent->Play();
            }
        }
        else
        {
            if (WalkingAudioComponent && WalkingAudioComponent->IsPlaying())
            {
                WalkingAudioComponent->Stop();
            }
        }
    }



    /*Afisez o cruce cu brate egale pe ecran pentru tinta*/
    //UE_LOG(LogTemp, Log, TEXT("Trece prin HUD"));
    FString InSlot1 = GetWeaponInSlot(1);
    FString InSlot2 = GetWeaponInSlot(2);
    if (InSlot1.Contains("S686") && SelectedWeaponSlot == 1 && !bShowMainMenu && !bShowSettings && !bShowStore && !bShowAchievements)
    {
        IsShotgun = true;
        DrawShotgunTarget();
    }
    else
    {
        IsShotgun = false;
    }
    if (InSlot2.Contains("S686") && SelectedWeaponSlot == 2 && !bShowMainMenu && !bShowSettings && !bShowStore && !bShowAchievements)
    {
        IsShotgun = true;
        DrawShotgunTarget();
    }
    else
    {
        IsShotgun = false;
    }

    if (slot12 == true && bIsScoped == false && IsShotgun == false && PlayerInCar == false && bShowMainMenu == false && bShowStore == false
        &&bShowSettings == false && bShowAchievements == false)
    {
        //UE_LOG(LogTemp, Log, TEXT("Desenez crucea"));
        DrawCrosshair();
    }

    /*Afisez traiectoria grenadei daca tin in mana o grenada*/
    if (slot345 == true)
    {
        DrawGrenadeImpactPoint();
    }
    //desenare minimap
    UpdateMiniMapCamera(); // actualizează poziția SceneCapture
    if (!bShowStore && !bShowSettings && !bShowAchievements) {
        DrawMiniMap();
    }

    //////////////////////////////////////
    //desenare compas in partea de sus  //
    // //////////////////////////////////
    
     // 1. Desenare compas în partea de sus
    const float CompassWidth = Canvas->ClipX * 0.8f; // lățimea compasului (80% din lățimea ecranului)
    const float CompassHeight = 30.f;                // înălțimea compasului
    const float CenterX = Canvas->ClipX / 2.f;
    const float TopY = 10.f; // distanța față de marginea de sus a ecranului

    // Font pentru text (poți seta alt font dacă vrei)
    UFont* Font25 = GEngine->GetMediumFont();

    // Punctele cardinale de afișat
    struct FCompassPoint
    {
        FString Label;
        float Angle; // în grade pe compas (0 = N, 90 = E, etc)
    };

    TArray<FCompassPoint> CompassPoints = {
        { TEXT("N"),   0.f },
        { TEXT("NE"),  45.f },
        { TEXT("E"),   90.f },
        { TEXT("SE"), 135.f },
        { TEXT("S"),  180.f },
        { TEXT("SW"), 225.f },
        { TEXT("W"),  270.f },
        { TEXT("NW"), 315.f },
        { TEXT("N"),  360.f }  // pentru continuitate
    };

    // Simulăm rotirea compasului în funcție de direcția jucătorului (aici rot = 0 ca exemplu)
    float PlayerYaw = 0.f;
    if (APawn* Pawn = GetOwningPawn())
    {
        PlayerYaw = Pawn->GetActorRotation().Yaw;
        if (PlayerYaw < 0) PlayerYaw += 360.f;
    }

    // Desenăm fiecare punct cardinal pe compas
    for (const FCompassPoint& Point : CompassPoints)
    {
        float RelativeAngle = Point.Angle - PlayerYaw;
        if (RelativeAngle < 0) RelativeAngle += 360.f;

        float XPos = CenterX + (RelativeAngle - 180.f) * (CompassWidth / 360.f);

        if (XPos > 0 && XPos < Canvas->ClipX)
        {
            FVector2D TextSize;
            // Desenăm label-ul
            Canvas->StrLen(Font25, Point.Label, TextSize.X, TextSize.Y);

            Canvas->SetDrawColor(FColor::White);
            Canvas->DrawText(Font25, Point.Label, XPos - TextSize.X / 2.f, TopY);

            // Desenăm numărul gradelor sub label (de exemplu, cu o distanță de 15 pixeli)
            FString AngleText = FString::Printf(TEXT("%.0f°"), Point.Angle);
            FVector2D AngleTextSize;
            Canvas->StrLen(Font25, AngleText, AngleTextSize.X, AngleTextSize.Y);

            Canvas->DrawText(Font25, AngleText, XPos - AngleTextSize.X / 2.f, TopY + TextSize.Y + 5.f);
        }
    }
    float ArrowHeight = 20.f;
    float ArrowWidth = 10.f;

    FVector2D Tip(CenterX, TopY + CompassHeight + 5.f); // vârful săgeții
    FVector2D LeftBase(CenterX - ArrowWidth / 2.f, Tip.Y + ArrowHeight);
    FVector2D RightBase(CenterX + ArrowWidth / 2.f, Tip.Y + ArrowHeight);

    Canvas->SetDrawColor(FColor::Red);

    // Linii între cele 3 puncte
    Canvas->K2_DrawLine(Tip, LeftBase, 2.f);
    Canvas->K2_DrawLine(LeftBase, RightBase, 2.f);
    Canvas->K2_DrawLine(RightBase, Tip, 2.f);

    //afisare scope x2
    if (bIsScoped && Scopex3_image)
    {
        float Scale = 2.5f; // Ajustează dacă vrei un scope mai mare sau mai mic
        float ScopeWidth = Scopex3_image->GetSurfaceWidth();
        float ScopeHeight = Scopex3_image->GetSurfaceHeight();

        float ScreenX20 = Canvas->SizeX / 2.f - ScopeWidth*Scale / 2.f;
        float ScreenY20 = Canvas->SizeY / 2.f - ScopeHeight*Scale / 2.f;
        FVector2D DrawPosition(ScreenX20, ScreenY20);
        FVector2D DrawSize(ScopeWidth * Scale, ScopeHeight * Scale);

        FCanvasTileItem TileItem(DrawPosition, Scopex3_image->Resource, DrawSize, FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Translucent; // Pentru texturi cu alpha

        Canvas->DrawItem(TileItem);
    }



    //---------------------------------
    //        Reference from bullets
    //---------------------------------

    if (!BulletMessage.IsEmpty())
    {
        float X = 50.0f;
        float Y = 50.0f;

        FCanvasTextItem TextItem(FVector2D(X, Y), FText::FromString(BulletMessage), GEngine->GetSmallFont(), FLinearColor::White);
        Canvas->DrawItem(TextItem);

        //UE_LOG(LogTemp, Warning, TEXT("Mesaj primit in HUD: %s"), *BulletMessage);

        if (bShowBulletIcon) // 🔥 Aici controlezi afisarea iconului
        {
            UTexture2D* BulletIcon = nullptr;

            if (BulletMessage.Contains(TEXT("5.56mm")))
            {
                BulletIcon = Slot556Icon;
            }
            else if (BulletMessage.Contains(TEXT("7.62mm")))
            {
                BulletIcon = Slot762Icon;
            }
            else if (BulletMessage.Contains(TEXT("9mm")))
            {
                BulletIcon = Slot9Icon;
            }

            if (BulletIcon)
            {
                float IconSize = 64.0f;
                FVector2D IconPosition(50.0f, 100.0f);

                FCanvasTileItem TileItem(IconPosition, BulletIcon->Resource, FVector2D(IconSize, IconSize), FLinearColor::White);
                TileItem.BlendMode = SE_BLEND_Translucent;
                Canvas->DrawItem(TileItem);
            }

            if (BulletMessage.Contains(TEXT("5.56mm")) && PickBullet == true)
            {
                BulletIcon = Slot556Icon;
                UE_LOG(LogTemp, Log, TEXT("Dau pick la 5.56"));
                PickBullet = false;
            }
            else if (BulletMessage.Contains(TEXT("7.62mm")) && PickBullet == true)
            {
                BulletIcon = Slot762Icon;
                UE_LOG(LogTemp, Log, TEXT("Dau pick la 7.62"));
                PickBullet = false;
            }
            else if (BulletMessage.Contains(TEXT("9mm")) && PickBullet == true)
            {
                BulletIcon = Slot9Icon;
                UE_LOG(LogTemp, Log, TEXT("Dau pick la 9"));
                PickBullet = false;
            }
        }
    }
    //-----------------------------------
    //    draw weapon on my shoulder
    //-----------------------------------




    //-----------------------------------
    //           HEALTH BAR
    //-----------------------------------

    float MidX = Canvas->ClipX / 2.0f;
    float HealthBarWidth = Canvas->ClipX / 3.0f;
    float HealthBarHeight = 40.0f;
    float HealthBarX = MidX - (HealthBarWidth / 2.0f);
    float HealthBarY = Canvas->ClipY - 210.0f;
    if (!bShowStore && !bShowSettings && !bShowAchievements) {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), HealthBarX, HealthBarY, HealthBarWidth, HealthBarHeight);
    }

    float HealthRatio = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
    float FilledWidth = HealthBarWidth * HealthRatio;

    FLinearColor HealthColor;
    if (HealthRatio >= 0.5f)
    {
        float T = (HealthRatio - 0.5f) * 2.0f;
        HealthColor = FLinearColor::LerpUsingHSV(FLinearColor::Yellow, FLinearColor::Green, T);
    }
    else
    {
        float T = HealthRatio * 2.0f;
        HealthColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Yellow, T);
    }

    FCanvasTileItem HealthTile(FVector2D(HealthBarX, HealthBarY), FVector2D(FilledWidth, HealthBarHeight), HealthColor);
    HealthTile.BlendMode = SE_BLEND_Translucent;
    if (!bShowStore && !bShowSettings && !bShowAchievements) {
        Canvas->DrawItem(HealthTile);
    }

    UFont* Font = GEngine->GetSmallFont();
    FString HealthText = FString::Printf(TEXT("HP: %d / %d"), FMath::RoundToInt(CurrentHealth), FMath::RoundToInt(MaxHealth));
    float TextW, TextH;
    Canvas->StrLen(Font, HealthText, TextW, TextH);
    FVector2D TextPos(HealthBarX + (HealthBarWidth - TextW) / 2, HealthBarY + (HealthBarHeight - TextH) / 2);

    FCanvasTextItem HealthTextItem(TextPos, FText::FromString(HealthText), Font, FLinearColor::White);
    HealthTextItem.EnableShadow(FLinearColor::Black);
    if (bShowStore == false && bShowSettings == false && bShowAchievements == false) {
        Canvas->DrawItem(HealthTextItem);
    }

    //-----------------------------------
    //        WEAPON SLOTS
    //-----------------------------------



    FString Slot1Name = GetWeaponInSlot(1);
    FString Slot2Name = GetWeaponInSlot(2);

    if (SelectedWeaponSlot == 1 && Slot1Name.Contains(TEXT("Empty")))
    {
        bhasweapon = false;
    }
    if (SelectedWeaponSlot == 1 && !Slot1Name.Contains(TEXT("Empty")))
    {
        bhasweapon = true;
    }
    if (SelectedWeaponSlot == 2 && Slot2Name.Contains(TEXT("Empty")))
    {
        bhasweapon = false;
    }
    if (SelectedWeaponSlot == 2 && !Slot2Name.Contains(TEXT("Empty")))
    {
        bhasweapon = true;
    }

    // Slot 1
    EBulletType TipBullet1 = Match_Weapon_with_bullets(Slot1Name);
    FString BulletTypeString1;

    switch (TipBullet1)
    {
    case EBulletType::BT_762:
        BulletTypeString1 = TEXT("7.62mm");
        break;
    case EBulletType::BT_556:
        BulletTypeString1 = TEXT("5.56mm");
        break;
    case EBulletType::BT_9MM:
        BulletTypeString1 = TEXT("9mm");
        break;
    case EBulletType::BT_None:
    default:
        BulletTypeString1 = TEXT("N/A");
        break;
    }

    //UE_LOG(LogTemp, Warning, TEXT("Arma: %s | Tip Glont: %s"), *Slot1Name, *BulletTypeString1);

    // Slot 2
    EBulletType TipBullet2 = Match_Weapon_with_bullets(Slot2Name);
    FString BulletTypeString2;

    switch (TipBullet2)
    {
    case EBulletType::BT_762:
        BulletTypeString2 = TEXT("7.62mm");
        break;
    case EBulletType::BT_556:
        BulletTypeString2 = TEXT("5.56mm");
        break;
    case EBulletType::BT_9MM:
        BulletTypeString2 = TEXT("9mm");
        break;
    case EBulletType::BT_None:
    default:
        BulletTypeString2 = TEXT("N/A");
        break;
    }

    //UE_LOG(LogTemp, Warning, TEXT("Arma: %s | Tip Glont: %s"), *Slot2Name, *BulletTypeString2);



    float SlotWidth = Canvas->ClipX / 6.0f;
    float SlotHeight = 120.0f;
    FVector2D Size(SlotWidth, SlotHeight);
    float BottomY = Canvas->ClipY - SlotHeight - 20.0f;

    FVector2D Pos1(MidX - SlotWidth - 10.0f, BottomY);
    FVector2D Pos2(MidX + 10.0f, BottomY);

    FLinearColor RectColor(0.f, 0.f, 0.f, 0.6f);
    DrawRect(RectColor, Pos1.X, Pos1.Y, Size.X, Size.Y);
    DrawRect(RectColor, Pos2.X, Pos2.Y, Size.X, Size.Y);

    FLinearColor TextColor = FLinearColor::White;

    //aici scriu numele armelor
    if (WeaponIcon1)
    {
        FCanvasTileItem TileItem(Pos1, WeaponIcon1->Resource, Size, FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(TileItem);
        //FString WeaponName = TEXT("Numele Armei"); // Înlocuiește cu numele real al armei
        FVector2D TextPos15 = Pos1 + FVector2D(0.0f, Size.Y - 20.0f);
        FCanvasTextItem TextItem(TextPos15, FText::FromString(Slot1Name), GEngine->GetSmallFont(), FLinearColor::White);
        TextItem.Scale = FVector2D(1.5f, 1.5f);
        TextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(TextItem);
    }
    else
    {
        float TextW1, TextH1;
        Canvas->StrLen(Font, Slot1Name, TextW1, TextH1);
        FVector2D TextPos1(Pos1.X + (Size.X - TextW1) / 2, Pos1.Y + (Size.Y - TextH1) / 2);

        FCanvasTextItem TextItem(TextPos1, FText::FromString(Slot1Name), Font, TextColor);
        TextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(TextItem);
    }

    if (WeaponIcon2)
    {
        FCanvasTileItem TileItem(Pos2, WeaponIcon2->Resource, Size, FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(TileItem);
        FVector2D TextPos16 = Pos2 + FVector2D(0.0f, Size.Y - 20.0f);
        FCanvasTextItem TextItem2(TextPos16, FText::FromString(Slot2Name), GEngine->GetSmallFont(), FLinearColor::White);
        TextItem2.Scale = FVector2D(1.5f, 1.5f);
        TextItem2.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(TextItem2);
    }
    else
    {
        float TextW2, TextH2;
        Canvas->StrLen(Font, Slot2Name, TextW2, TextH2);
        FVector2D TextPos2(Pos2.X + (Size.X - TextW2) / 2, Pos2.Y + (Size.Y - TextH2) / 2);

        FCanvasTextItem TextItem(TextPos2, FText::FromString(Slot2Name), Font, TextColor);
        TextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(TextItem);
    }

    switch (TipBullet1)
    {
    case EBulletType::BT_762:
        TotalAmmo1 = gloante_762;
        break;
    case EBulletType::BT_556:
        TotalAmmo1 = gloante_556;
        break;
    case EBulletType::BT_9MM:
        TotalAmmo1 = gloante_9;
        break;
    case EBulletType::BT_None:
    default:
        TotalAmmo1 = 0;
        break;
    }

    switch (TipBullet2)
    {
    case EBulletType::BT_762:
        TotalAmmo2 = gloante_762;
        break;
    case EBulletType::BT_556:
        TotalAmmo2 = gloante_556;
        break;
    case EBulletType::BT_9MM:
        TotalAmmo2 = gloante_9;
        break;
    case EBulletType::BT_None:
    default:
        TotalAmmo2 = 0;
        break;
    }

    ////////////////////////////////////////////////////////
    //      Afisare gloante in magazine/ gloante totale   //
    ////////////////////////////////////////////////////////

    
    //slot 1
    // Poziția pentru textul cu muniția (ușor mai în dreapta decât Slot1Name)
    if (!Slot1Name.Contains("Empty")) {
        FVector2D AmmoTextPos1 = Pos1 + FVector2D(Size.X - 100.0f, Size.Y - 20.0f);
        // Creezi stringul cu gloanțe
        FString AmmoText1 = FString::Printf(TEXT("%d / %d"), AmmoInMag1, FMath::Max(TotalAmmo1-AmmoInMag1,0));
        // Creezi textul de desenat
        FCanvasTextItem AmmoTextItem1(AmmoTextPos1, FText::FromString(AmmoText1), GEngine->GetSmallFont(), FLinearColor::White);
        AmmoTextItem1.Scale = FVector2D(1.5f, 1.5f);
        AmmoTextItem1.EnableShadow(FLinearColor::Black);
        // Desenezi textul pe ecran
        Canvas->DrawItem(AmmoTextItem1);
    }


    //slot 2
    if (!Slot2Name.Contains("Empty")) {
        FVector2D AmmoTextPos2 = Pos2 + FVector2D(Size.X - 100.0f, Size.Y - 20.0f);

        FString AmmoText2 = FString::Printf(TEXT("%d / %d"), AmmoInMag2, FMath::Max(TotalAmmo2-AmmoInMag2,0));

        FCanvasTextItem AmmoTextItem2(AmmoTextPos2, FText::FromString(AmmoText2), GEngine->GetSmallFont(), FLinearColor::White);
        AmmoTextItem2.Scale = FVector2D(1.5f, 1.5f);
        AmmoTextItem2.EnableShadow(FLinearColor::Black);

        Canvas->DrawItem(AmmoTextItem2);
    }


    FVector2D OutlineSize = Size + FVector2D(4.f, 4.f);
    FVector2D Offset(-2.f, -2.f);
    FLinearColor OrangeColor = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f);

    auto DrawOutline = [&](const FVector2D& TopLeft, const FVector2D& Size, const FLinearColor& Color, float Thickness = 2.0f)
        {
            FVector2D TopRight = TopLeft + FVector2D(Size.X, 0);
            FVector2D BottomLeft = TopLeft + FVector2D(0, Size.Y);
            FVector2D BottomRight = TopLeft + Size;

            Canvas->K2_DrawLine(TopLeft, TopRight, Thickness, Color);
            Canvas->K2_DrawLine(TopRight, BottomRight, Thickness, Color);
            Canvas->K2_DrawLine(BottomRight, BottomLeft, Thickness, Color);
            Canvas->K2_DrawLine(BottomLeft, TopLeft, Thickness, Color);
        };

    if (SelectedWeaponSlot == 1)
    {
        FVector2D HighlightPos = Pos1 + Offset;
        DrawOutline(HighlightPos, OutlineSize, OrangeColor);
    }
    else if (SelectedWeaponSlot == 2)
    {
        FVector2D HighlightPos = Pos2 + Offset;
        DrawOutline(HighlightPos, OutlineSize, OrangeColor);
    }

    // Dimensiune grenadă ajustată (50% lățime, 70% înălțime)
    FVector2D GrenadeSize(Size.X * 0.5f, Size.Y * 0.7f);
    // Poziția grenzii, ajustată vertical pentru a rămâne aliniată jos cu celelalte sloturi
    FVector2D Pos3(MidX + SlotWidth + 30.0f, BottomY + (Size.Y - GrenadeSize.Y));

    // Fundal grenadă
    DrawRect(RectColor, Pos3.X, Pos3.Y, GrenadeSize.X, GrenadeSize.Y);

    if (GrenadeIcon)
    {
        // Desenăm iconița
        FCanvasTileItem TileItem(Pos3, GrenadeIcon->Resource, GrenadeSize, FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(TileItem);

        // Setăm poziția textului (în colțul din dreapta-jos al iconiței)
        FVector2D TextPos123 = Pos3 + FVector2D(GrenadeSize.X -20.f, GrenadeSize.Y - 20.f);

        // Creăm fontul (dacă ai deja un font setat, îl poți folosi)
        UFont* DefaultFont = GEngine->GetLargeFont(); // sau GetSmallFont()
        if (DefaultFont)
        {
            // Dublăm mărimea fontului (Scale = 2.0f)
            FCanvasTextItem TextItem(TextPos123, FText::FromString(FString::Printf(TEXT("%d"), Grenades)), DefaultFont, FLinearColor::White);
            TextItem.Scale = FVector2D(2.0f, 2.0f); // font de 2 ori mai mare
            TextItem.bCentreX = false;
            TextItem.bCentreY = false;

            Canvas->DrawItem(TextItem);
        }
    }

    else
    {
        float TextW3, TextH3;
        Canvas->StrLen(Font, GrenadeSlot, TextW3, TextH3);
        FVector2D TextPos3(Pos3.X + (GrenadeSize.X - TextW3) / 2, Pos3.Y + (GrenadeSize.Y - TextH3) / 2);

        FCanvasTextItem TextItem(TextPos3, FText::FromString(GrenadeSlot), Font, TextColor);
        TextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(TextItem);
    }

    // Contur portocaliu dacă e selectată grenada
    if (SelectedWeaponSlot == 3)
    {
        FVector2D GrenadeOutlineSize = GrenadeSize + FVector2D(4.f, 4.f);
        FVector2D HighlightPos = Pos3 + Offset;
        DrawOutline(HighlightPos, GrenadeOutlineSize, OrangeColor);
    }
    FVector2D Slot4Size = GrenadeSize; // Poți schimba dimensiunea dacă vrei alt aspect

    // Poziția - plasăm chenarul 4 la dreapta celui de grenadă, cu spațiere de 10 px
    FVector2D Pos4(Pos3.X + GrenadeSize.X + 10.0f, BottomY + (Size.Y - Slot4Size.Y));

    // Fundal slot 4
    DrawRect(RectColor, Pos4.X, Pos4.Y, Slot4Size.X, Slot4Size.Y);

    if (Slot4Icon)
    {
        // Desenează iconița
        FCanvasTileItem TileItem(Pos4, Slot4Icon->Resource, Slot4Size, FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(TileItem);

        // Obține numărul de grenade (înlocuiește cu variabila ta reală)

        // Poziția textului (dreapta-jos față de iconiță)
        FVector2D TextPos125 = Pos4 + FVector2D(Slot4Size.X -20.f, Slot4Size.Y - 20.f);

        // Folosim un font
        UFont* DefaultFont = GEngine->GetLargeFont();
        if (DefaultFont)
        {
            FCanvasTextItem TextItem(TextPos125,
                FText::FromString(FString::Printf(TEXT("%d"), Molotovs)),
                DefaultFont,
                FLinearColor::White);

            // Font de 2x mai mare
            TextItem.Scale = FVector2D(2.0f, 2.0f);
            TextItem.bCentreX = false;
            TextItem.bCentreY = false;

            Canvas->DrawItem(TextItem);
        }
    }

    else
    {
        FString Slot4Text = TEXT("Gadget");
        float TextW4, TextH4;
        Canvas->StrLen(Font, Slot4Text, TextW4, TextH4);
        FVector2D TextPos4(Pos4.X + (Slot4Size.X - TextW4) / 2, Pos4.Y + (Slot4Size.Y - TextH4) / 2);

        FCanvasTextItem TextItem(TextPos4, FText::FromString(Slot4Text), Font, TextColor);
        TextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(TextItem);
    }

    // Contur portocaliu dacă e selectat slotul 4
    if (SelectedWeaponSlot == 4)
    {
        FVector2D Slot4OutlineSize = Slot4Size + FVector2D(4.f, 4.f);
        FVector2D HighlightPos = Pos4 + Offset;
        DrawOutline(HighlightPos, Slot4OutlineSize, OrangeColor);
    }
    FVector2D Slot5Size = Slot4Size;

    // Poziție: la dreapta slotului 4, cu 10 px spațiu
    FVector2D Pos5(Pos4.X + Slot4Size.X + 10.0f, BottomY + (Size.Y - Slot5Size.Y));

    // Fundal slot 5
    DrawRect(RectColor, Pos5.X, Pos5.Y, Slot5Size.X, Slot5Size.Y);

    if (WeaponIcon5)
    {
        // Desenează iconița
        FCanvasTileItem TileItem(Pos5, WeaponIcon5->Resource, Slot5Size, FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(TileItem);

        // Exemplu: numărul de grenade sau muniție pentru slotul 5

        // Poziția textului (dreapta-jos față de iconiță)
        FVector2D TextPos126 = Pos5 + FVector2D(Slot5Size.X - 20.f, Slot5Size.Y - 20.f);

        // Fontul
        UFont* DefaultFont = GEngine->GetLargeFont();
        if (DefaultFont)
        {
            FCanvasTextItem TextItem(
                TextPos126,
                FText::FromString(FString::Printf(TEXT("%d"), SmokeGrenades)),
                DefaultFont,
                FLinearColor::White
            );

            // Mărim fontul de 2x
            TextItem.Scale = FVector2D(2.0f, 2.0f);
            TextItem.bCentreX = false;
            TextItem.bCentreY = false;

            Canvas->DrawItem(TextItem);
        }
    }
    else
    {
        FString Slot5Text = TEXT("Empty");
        float TextW5, TextH5;
        Canvas->StrLen(Font, Slot5Text, TextW5, TextH5);
        FVector2D TextPos5(Pos5.X + (Slot5Size.X - TextW5) / 2, Pos5.Y + (Slot5Size.Y - TextH5) / 2);

        FCanvasTextItem TextItem(TextPos5, FText::FromString(Slot5Text), Font, TextColor);
        TextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(TextItem);
    }

    // Contur portocaliu dacă e selectat slotul 5
    if (SelectedWeaponSlot == 5)
    {
        FVector2D Slot5OutlineSize = Slot5Size + FVector2D(4.f, 4.f);
        FVector2D HighlightPos = Pos5 + Offset;
        DrawOutline(HighlightPos, Slot5OutlineSize, OrangeColor);
    }
    /*
        Bagpack slot
    */
    float CircleRadius = 50.0f;
    FVector2D CircleCenter(120.f, Canvas->ClipY - 120.0f); // Poziție stânga jos, ajustează cum vrei

    // Desenează cercul
    const int32 NumSegments = 64;
    float AngleStep = 2 * PI / NumSegments;

    for (int32 i = 0; i < NumSegments; ++i)
    {
        float Angle1 = i * AngleStep;
        float Angle2 = (i + 1) * AngleStep;

        FVector2D Point1 = CircleCenter + FVector2D(FMath::Cos(Angle1), FMath::Sin(Angle1)) * CircleRadius;
        FVector2D Point2 = CircleCenter + FVector2D(FMath::Cos(Angle2), FMath::Sin(Angle2)) * CircleRadius;

        Canvas->K2_DrawLine(Point1, Point2, 2.0f, FLinearColor::White);
    }

    // Desenează iconul în interiorul cercului (sau text fallback dacă nu ai icon)
    FVector2D IconSize(80.0f, 80.0f); // Ajustează dimensiunea dacă vrei
    FVector2D IconPos = CircleCenter - (IconSize * 0.5f);

    if (BackpackIcon)
    {
        FCanvasTileItem TileItem(IconPos, BackpackIcon->Resource, IconSize, FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(TileItem);
    }
    else
    {
        UFont* LocalFont = GEngine->GetSmallFont();
        FString BackpackText = TEXT("Bag");
        float LocalTextW, LocalTextH;
        Canvas->StrLen(LocalFont, BackpackText, LocalTextW, LocalTextH);
        FVector2D LocalTextPos = CircleCenter - FVector2D(LocalTextW / 2, LocalTextH / 2);

        FCanvasTextItem TextItem(LocalTextPos, FText::FromString(BackpackText), LocalFont, FLinearColor::White);
        TextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(TextItem);
    }
    /*Inventarul meu*/
    if (bIsInventoryOpen)
    {
        FVector2D InventorySize(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
        FVector2D InventoryPos((Canvas->ClipX - InventorySize.X) / 2.0f, (Canvas->ClipY - InventorySize.Y) / 2.0f);

        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.8f), InventoryPos.X, InventoryPos.Y, InventorySize.X, InventorySize.Y);

        UFont* Font2 = GEngine->GetLargeFont();
        FString InventoryText = TEXT("Inventory Opened");
        float TextW2, TextH2;
        Canvas->StrLen(Font2, InventoryText, TextW2, TextH2);
        FVector2D TextPos5(InventoryPos.X + (InventorySize.X - TextW2) / 2, InventoryPos.Y + 20.0f);

        FCanvasTextItem TextItem(TextPos5, FText::FromString(InventoryText), Font2, FLinearColor::White);
        TextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(TextItem);

        // Desenare buton Close
        CloseButtonPos = FVector2D(InventoryPos.X + InventorySize.X - CloseButtonSize.X - 10.f, InventoryPos.Y + 10.f);
        DrawRect(FLinearColor::Red, CloseButtonPos.X, CloseButtonPos.Y, CloseButtonSize.X, CloseButtonSize.Y);

        FString CloseText = TEXT("X");
        float CloseTextW, CloseTextH;
        Canvas->StrLen(Font2, CloseText, CloseTextW, CloseTextH);
        FVector2D CloseTextPos = CloseButtonPos + (CloseButtonSize - FVector2D(CloseTextW, CloseTextH)) * 0.5f;

        FCanvasTextItem CloseTextItem(CloseTextPos, FText::FromString(CloseText), Font2, FLinearColor::White);
        CloseTextItem.EnableShadow(FLinearColor::Black);
        Canvas->DrawItem(CloseTextItem);

        //adaugare iteme:
        float ItemSize = 64.0f; // Dimensiune default
        float Padding = 10.0f;
        FVector2D StartPos(InventoryPos.X + 20.0f, InventoryPos.Y + 60.0f);

        for (int32 i = 0; i < InventoryItems.Num(); ++i)
        {
            const FInventoryItem& Item = InventoryItems[i];

            // Dimensiune variabilă pentru arme
            FVector2D IconSize2;
            if (Item.bIsWeapon)
            {
                IconSize2 = FVector2D(ItemSize * 2.0f, ItemSize); // De 2 ori mai lung dacă e armă
            }
            else
            {
                IconSize2 = FVector2D(ItemSize, ItemSize);
            }

            FVector2D ItemDrawPos = StartPos + FVector2D(0, i * (ItemSize + Padding));

            if (Item.Icon)
            {
                FCanvasTileItem Tile(ItemDrawPos, Item.Icon->Resource, IconSize2, FLinearColor::White);
                Tile.BlendMode = SE_BLEND_Translucent;
                Canvas->DrawItem(Tile);
            }

            // Nume + cantitate
            FString ItemDescription;
            if (Item.bIsWeapon)
            {
                ItemDescription = FString::Printf(TEXT("%s | Ammo: %d"), *Item.Name, Item.AmmoInMagazine);
            }
            else
            {
                ItemDescription = FString::Printf(TEXT("%s x%d"), *Item.Name, Item.Quantity);
            }

            float TextWidth = 0.0f;
            float TextHeight = 0.0f;
            Canvas->StrLen(Font2, ItemDescription, TextWidth, TextHeight);

            FVector2D TextDrawPos = ItemDrawPos + FVector2D(IconSize2.X + 10.0f, (IconSize2.Y - TextHeight) * 0.5f);

            FCanvasTextItem ItemTextItem(TextDrawPos, FText::FromString(ItemDescription), Font2, FLinearColor::White);
            ItemTextItem.EnableShadow(FLinearColor::Black);
            Canvas->DrawItem(ItemTextItem);
        }
    }
    if (bSeeMap && Canvas && MiniMapRenderTarget)
    {
        if (!CachedPC)
        {
            CachedPC = GetOwningPlayerController();
        }

        if (CachedPC)
        {
            CachedPC->bShowMouseCursor = true;
            CachedPC->SetInputMode(FInputModeGameAndUI());

            // Scroll = Zoom
            float ScrollValue = CachedPC->GetInputAnalogKeyState(EKeys::MouseWheelAxis);
            MapZoom = FMath::Clamp(MapZoom + ScrollValue * 0.1f, 0.25f, 4.0f);

            // Drag cu mouse stânga
            bool bIsLeftMouseDown = CachedPC->IsInputKeyDown(EKeys::LeftMouseButton);

            FVector2D CurrentMousePos;
            CachedPC->GetMousePosition(CurrentMousePos.X, CurrentMousePos.Y);

            if (bIsLeftMouseDown)
            {
                if (!bDraggingMap)
                {
                    bDraggingMap = true;
                    LastMousePos = CurrentMousePos;
                }
                else
                {
                    FVector2D Delta = CurrentMousePos - LastMousePos;
                    MapOffset += Delta;
                    LastMousePos = CurrentMousePos;
                }
            }
            else
            {
                bDraggingMap = false;
            }

            FVector2D ViewportSize(Canvas->ClipX, Canvas->ClipY);

            // Dimensiunea reală a hărții (ajustează după zona ta explorable)
            FVector2D VirtualMapSize = FVector2D(2048.0f, 2048.0f) * MapZoom;

            // Poziția pe ecran (centrat + offset)
            FVector2D MapDrawPos = (ViewportSize - VirtualMapSize) / 2.0f + MapOffset;

            // Fundal negru
            FCanvasTileItem Background(FVector2D(0.0f, 0.0f), ViewportSize, FLinearColor::Black);
            Background.BlendMode = SE_BLEND_Opaque;
            Canvas->DrawItem(Background);

            // Harta desenată ca tile
            FCanvasTileItem MapTile(MapDrawPos, MiniMapRenderTarget->Resource, VirtualMapSize, FLinearColor::White);
            MapTile.BlendMode = SE_BLEND_Opaque;
            Canvas->DrawItem(MapTile);

            // --- Desenăm poziția playerului pe hartă ---
            APawn* PlayerPawn2 = CachedPC->GetPawn();
            if (PlayerPawn2)
            {
                FVector PlayerWorldPos = PlayerPawn2->GetActorLocation();

                // ⚙️ Dimensiunea totală a lumii hărții (în unități Unreal) — modifică după caz
                FVector2D MapWorldSize(2048.0f, 2048.0f);

                // 🟡 Centru imaginar al hărții în coordonate world
                FVector2D MapWorldCenter(0.0f, 0.0f);

                // 🔄 Poziția relativă 0..1 în funcție de cât de departe e față de centru
                FVector2D RelativePos;
                // Inversăm axele pentru că pe hartă X=Nord, Y=Est, dar în lume Y e Est
                RelativePos.X = (PlayerWorldPos.Y - MapWorldCenter.Y + MapWorldSize.Y / 2.0f) / (MapWorldSize.Y*230.0f)+0.4925f;
                RelativePos.Y = (PlayerWorldPos.X - MapWorldCenter.X + MapWorldSize.X / 2.0f) / (MapWorldSize.X*230.0f)+0.4925f;


                // 🧭 Coordonatele UI: transformăm Y pentru că în UI Y crește în jos
                FVector2D PlayerMapPos;
                PlayerMapPos.X = RelativePos.X * VirtualMapSize.X;
                PlayerMapPos.Y = (1.0f - RelativePos.Y) * VirtualMapSize.Y;

                // 🖼️ Poziția finală pe ecran
                FVector2D PlayerDrawPos = MapDrawPos + PlayerMapPos;

                // 🔴 Desenăm punctul
                float DotSize = 10.0f;
                FCanvasTileItem PlayerDot(PlayerDrawPos - FVector2D(DotSize / 2, DotSize / 2), FVector2D(DotSize, DotSize), FLinearColor::Red);
                PlayerDot.BlendMode = SE_BLEND_Translucent;
                Canvas->DrawItem(PlayerDot);
            }
        }
    }
    else
    {
        if (CachedPC)
        {
            CachedPC->bShowMouseCursor = false;
            CachedPC->SetInputMode(FInputModeGameOnly());
        }
        bDraggingMap = false;
    }
}
