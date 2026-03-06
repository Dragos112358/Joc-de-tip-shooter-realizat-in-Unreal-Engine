// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "CPP_Shooter2HUD.h"
#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>


// Weapon.cpp

// Definirea staticelor
UStaticMesh* AWeapon::AKM_Mesh = nullptr;
UStaticMesh* AWeapon::Scar_Mesh = nullptr;
UStaticMesh* AWeapon::VSS_Mesh = nullptr;
UStaticMesh* AWeapon::M416_Mesh = nullptr;
UStaticMesh* AWeapon::M762_Mesh = nullptr;
UStaticMesh* AWeapon::M249_Mesh = nullptr;
UStaticMesh* AWeapon::AWM_Mesh = nullptr;
UStaticMesh* AWeapon::SKS_Mesh = nullptr;
UStaticMesh* AWeapon::Groza_Mesh = nullptr;
UStaticMesh* AWeapon::AUGA3_Mesh = nullptr;
UStaticMesh* AWeapon::KAR98K_Mesh = nullptr;
UStaticMesh* AWeapon::S686_Mesh = nullptr;
UStaticMesh* AWeapon::DP28_Mesh = nullptr;


AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;

    PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
    PickupSphere->SetupAttachment(RootComponent);
    PickupSphere->SetSphereRadius(50.f);
    PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    bCanBePickedUp = false;

    // Încărcăm mesh-urile static doar o singură dată
    if (!AKM_Mesh)
    {
        AKM_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/StarterContent/source/AKM_47_Skechtfab.AKM_47_Skechtfab")));
        if (!AKM_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load AKM mesh!"));
    }
    if (!Scar_Mesh)
    {
        Scar_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/SCAR_L_SCAR_L_.SCAR_L_SCAR_L_'")));
        if (!Scar_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load Scar mesh!"));
    }
    if (!VSS_Mesh)
    {
        VSS_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/VSS.VSS'")));
        if (!VSS_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load VSS mesh!"));
    }
    if(!M416_Mesh)
    {
        M416_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/M4.M4'")));
        if (!M416_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load M416 mesh!"));
    }
    if (!M762_Mesh)
    {
        M762_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/m762_assault_rifle.m762_assault_rifle'")));
        if (!M762_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load M762 mesh!"));
    }
    if (!M249_Mesh)
    {
        M249_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/M249.M249'")));
        if (!M249_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load M249 mesh!"));
    }
    if (!AWM_Mesh)
    {
        AWM_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/awm.awm'")));
        if (!AWM_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load AWM mesh!"));
    }
    if (!SKS_Mesh)
    {
        SKS_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/SKS.SKS'")));
        if (!SKS_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load SKS mesh!"));
    }
    if (!Groza_Mesh)
    {
        Groza_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/GROZA.GROZA'")));
        if (!Groza_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load Groza mesh!"));
    }
    if (!KAR98K_Mesh)
    {
        KAR98K_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/Kar98K.Kar98K'")));
        if (!KAR98K_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load KAR98K mesh!"));
    }
    if (!S686_Mesh)
    {
        S686_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/shotgun.shotgun'")));
        if (!S686_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load S686 mesh!"));
    }
    if (!DP28_Mesh)
    {
        DP28_Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/dp28.dp28'")));
        if (!DP28_Mesh) UE_LOG(LogTemp, Error, TEXT("Failed to load DP28 mesh!"));
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> PickupObj(TEXT("/Script/Engine.SoundWave'/Game/StarterContent/Audio/put_item-83043.put_item-83043'"));

    if (PickupObj.Succeeded())
    {
        PickupWeaponSound2 = PickupObj.Object;
        UE_LOG(LogTemp, Warning, TEXT("Pickup sound loaded successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load pickup sound!"));
    }
}


void AWeapon::InitializeWeapon(EWeaponType WeaponType)
{
    switch (WeaponType)
    {
    case EWeaponType::AKM:
        WeaponMesh->SetStaticMesh(AKM_Mesh);
        WeaponName = "AKM";
        BulletType = EBulletType::BT_762;
        AmmoInMagazine = 30;
        AmmoInInventory = 90;
        FireRate = 0.12f;
        Damage = 35.0f;
        break;

    case EWeaponType::Scar:
        WeaponMesh->SetStaticMesh(Scar_Mesh);
        WeaponName = "Scar";
        BulletType = EBulletType::BT_556;
        AmmoInMagazine = 30;
        AmmoInInventory = 120;
        FireRate = 0.1f;
        WeaponMesh->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
        PickupSphere->SetSphereRadius(550.f);
        Damage = 33.0f;
        //WeaponMesh->SetRelativeRotation(FRotator(0.f, 0.f, 180.f));
        break;

    case EWeaponType::VSS:
        WeaponMesh->SetStaticMesh(VSS_Mesh);
        WeaponName = "VSS";
        BulletType = EBulletType::BT_9MM;
        AmmoInMagazine = 10;
        AmmoInInventory = 150;
        FireRate = 0.09f;
        Damage = 32.0f;
        WeaponMesh->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
        PickupSphere->SetSphereRadius(500.f);
        break;

    case EWeaponType::M416:
        WeaponMesh->SetStaticMesh(M416_Mesh);
        WeaponName = "M416";
        BulletType = EBulletType::BT_556;
        AmmoInMagazine = 30;
        AmmoInInventory = 150;
        FireRate = 0.09f;
        Damage = 28.0f;
        WeaponMesh->SetWorldScale3D(FVector(0.03f, 0.03f, 0.03f));
        PickupSphere->SetSphereRadius(2000.f);
        break;

    case EWeaponType::M762:
        WeaponMesh->SetStaticMesh(M416_Mesh);
        WeaponName = "M762";
        BulletType = EBulletType::BT_762;
        AmmoInMagazine = 30;
        AmmoInInventory = 150;
        FireRate = 0.09f;
        Damage = 35.0f;
        WeaponMesh->SetWorldScale3D(FVector(0.03f, 0.03f, 0.03f));
        PickupSphere->SetSphereRadius(2000.f);
        break;

    case EWeaponType::M249:
        WeaponMesh->SetStaticMesh(M416_Mesh);
        WeaponName = "M249";
        BulletType = EBulletType::BT_556;
        AmmoInMagazine = 30;
        AmmoInInventory = 150;
        FireRate = 0.09f;
        Damage = 35.0f;
        WeaponMesh->SetWorldScale3D(FVector(0.03f, 0.03f, 0.03f));
        PickupSphere->SetSphereRadius(2000.f);
        break;

    case EWeaponType::AWM:
        WeaponMesh->SetStaticMesh(AWM_Mesh);
        WeaponName = "AWM";
        BulletType = EBulletType::BT_9MM;
        AmmoInMagazine = 5;
        AmmoInInventory = 30;
        FireRate = 0.7;
        Damage = 90.f;
        WeaponMesh->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
        PickupSphere->SetSphereRadius(120.f);
        break;

    case EWeaponType::SKS:
        WeaponMesh->SetStaticMesh(SKS_Mesh);
        WeaponName = "SKS";
        BulletType = EBulletType::BT_762;
        AmmoInMagazine = 10;
        AmmoInInventory = 30;
        FireRate = 0.4;
        Damage = 70.f;
        WeaponMesh->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
        PickupSphere->SetSphereRadius(600.f);
        break;
    
    case EWeaponType::Groza:
        WeaponMesh->SetStaticMesh(Groza_Mesh);
        WeaponName = "Groza";
        BulletType = EBulletType::BT_762;
        AmmoInMagazine = 30;
        AmmoInInventory = 150;
        FireRate = 0.08;
        Damage = 31.f;
        WeaponMesh->SetWorldScale3D(FVector(0.03f, 0.03f, 0.03f));
        PickupSphere->SetSphereRadius(2000.f);
        break;

    case EWeaponType::KAR98K:
        WeaponMesh->SetStaticMesh(KAR98K_Mesh);
        WeaponName = "KAR98K";
        BulletType = EBulletType::BT_762;
        AmmoInMagazine = 5;
        AmmoInInventory = 30;
        FireRate = 0.8;
        Damage = 96.f;
        WeaponMesh->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));
        PickupSphere->SetSphereRadius(200.f);
        break;

    case EWeaponType::S686:
        WeaponMesh->SetStaticMesh(S686_Mesh);
        WeaponName = "S686";
        BulletType = EBulletType::BT_762;
        AmmoInMagazine = 2;
        AmmoInInventory = 30;
        FireRate = 0.2;
        Damage = 96.f;
        WeaponMesh->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));
        PickupSphere->SetSphereRadius(200.f);
        break;

    case EWeaponType::DP28:
        WeaponMesh->SetStaticMesh(DP28_Mesh);
        WeaponName = "DP28";
        BulletType = EBulletType::BT_762;
        AmmoInMagazine = 47;
        AmmoInInventory = 150;
        FireRate = 0.1;
        Damage = 27.f;
        //WeaponMesh->SetRelativeRotation(FRotator(90.f, 0.f, 0.f)); // Culcat pe podea

        WeaponMesh->SetWorldScale3D(FVector(0.02f, 0.02f, 0.02f));
        PickupSphere->SetSphereRadius(3000.f);
        WeaponMesh->SetRelativeRotation(FRotator::ZeroRotator);
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("WeaponType necunoscut!"));
        break;
    }

    UE_LOG(LogTemp, Warning, TEXT("Weapon %s initializat cu succes!"), *WeaponName);
}

void AWeapon::OnPickupSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(ACharacter::StaticClass()))
    {
        if (bIsPickupEnabled) // Verifici dacă arma e gândită să poată fi ridicată
        {
            bCanBePickedUp = true;
            UE_LOG(LogTemp, Warning, TEXT("Esti langa arma: %s"), *WeaponName);
        }
    }
}


void AWeapon::OnPickupSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor->IsA(ACharacter::StaticClass()) && bIsPickupEnabled)
    {
        bCanBePickedUp = false;
        UE_LOG(LogTemp, Warning, TEXT("Ai iesit din zona armei: %s"), *WeaponName);
    }
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
    PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnPickupSphereOverlap);
    PickupSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnPickupSphereEndOverlap);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bCanBePickedUp)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC && PC->IsInputKeyDown(EKeys::P))
        {
            UE_LOG(LogTemp, Warning, TEXT("Ai luat arma: %s"), *WeaponName);
            UGameplayStatics::PlaySound2D(this, PickupWeaponSound2);
            ACPP_Shooter2HUD* MyHUD = Cast<ACPP_Shooter2HUD>(PC->GetHUD());
            if (MyHUD)
            {
                MyHUD->AddWeaponToInventory(this);
            }

            Destroy();
        }
    }
}


