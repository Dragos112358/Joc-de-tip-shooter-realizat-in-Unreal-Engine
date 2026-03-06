#include "Bullet.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "CPP_Shooter2HUD.h"



// Sets default values
// Sets default values
ABullet::ABullet()
{
    PrimaryActorTick.bCanEverTick = true;

    // Creează coliziunea principală ca RootComponent
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Creează PickupSphere și o atașează la RootComponent (acum este definit corect!)
    PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
    PickupSphere->SetupAttachment(RootComponent);
    PickupSphere->SetSphereRadius(100.f);
    PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Creează corpul cutiei
    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(RootComponent);

    // Creează capacul
    LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
    LidMesh->SetupAttachment(BodyMesh);

    // Încarcă mesh-urile

    // Mesh 5.56
    static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/ammobox_low.ammobox_low'"));
    if (BodyMeshAsset.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("Body mesh 5.56 încărcat cu succes"));
        Mesh_556 = BodyMeshAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Body mesh 5.56 neîncărcat"));
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> LidMeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/ammoboxup_low.ammoboxup_low'"));
    if (LidMeshAsset.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("Lid mesh 5.56 încărcat cu succes"));
        LidMesh_556 = LidMeshAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Lid mesh 5.56 neîncărcat"));
    }

    // Mesh 7.62
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Body762(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/ammo_box_012.ammo_box_012'"));
    if (Body762.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("Body mesh 7.62 încărcat cu succes"));
        Mesh_762 = Body762.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Body mesh 7.62 neîncărcat"));
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Capac762(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/ammo_box_0123.ammo_box_0123'"));
    if (Capac762.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("Lid mesh 7.62 încărcat cu succes"));
        Capac_762 = Capac762.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Lid mesh 7.62 neîncărcat"));
    }

    // Mesh 9MM
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Body9MM(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/9mm.9mm'"));
    if (Body9MM.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("Body mesh 9MM încărcat cu succes"));
        Mesh_9MM = Body9MM.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Body mesh 9MM neîncărcat"));
    }

    // Variabilă inițială
    bCanBePickedUp = false;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
    Super::BeginPlay();
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        ACPP_Shooter2HUD* MyHUD = Cast<ACPP_Shooter2HUD>(PC->GetHUD());
        if (MyHUD)
        {
            OnBulletNearby.AddDynamic(MyHUD, &ACPP_Shooter2HUD::HandleBulletNearbyMessage);
        }
    }
    switch (BulletType)
    {
    case EBulletType::BT_556:
        BodyMesh->SetStaticMesh(Mesh_556);
        BodyMesh->SetWorldScale3D(FVector(5.0f));
        LidMesh->SetStaticMesh(LidMesh_556);
        LidMesh->SetWorldScale3D(FVector(4.75f));
        LidMesh->SetVisibility(true);

        // Coliziune normală pe cutie 5.56
        CollisionBox->SetBoxExtent(FVector(10.f, 10.f, 5.f)); // Ajustează dacă vrei
        CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
        CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);
        break;

    case EBulletType::BT_762:
        BodyMesh->SetStaticMesh(Mesh_762);
        BodyMesh->SetWorldScale3D(FVector(0.15f)); // folosim aceeași scalare ca 5.56, ajustabilă
        LidMesh->SetStaticMesh(Capac_762);
        LidMesh->SetWorldScale3D(FVector(0.15f)); // capacul 7.62 la fel scalat
        LidMesh->SetVisibility(true);
        LidMesh->SetupAttachment(BodyMesh); // atașăm capacul la corp
        break;

    case EBulletType::BT_9MM:
        BodyMesh->SetStaticMesh(Mesh_9MM);
        BodyMesh->SetWorldScale3D(FVector(5.0f));

        // Dezactivezi complet capacul
        LidMesh->SetVisibility(false);
        LidMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        LidMesh->SetGenerateOverlapEvents(false);

        // Activezi coliziunea pe cutie
        CollisionBox->SetBoxExtent(FVector(10.f, 10.f, 5.f)); // Ajustează la dimensiunea 9mm dacă e nevoie
        CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
        CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);

        break;
    }
    PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnPickupSphereOverlap);
    PickupSphere->OnComponentEndOverlap.AddDynamic(this, &ABullet::OnPickupSphereEndOverlap);

    bCanBePickedUp = false;

}
void ABullet::OnPickupSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(ACharacter::StaticClass()))
    {
        bCanBePickedUp = true;

        FString BulletTypeName;

        switch (BulletType)
        {
        case EBulletType::BT_556:
            BulletTypeName = TEXT("5.56mm");
            break;
        case EBulletType::BT_762:
            BulletTypeName = TEXT("7.62mm");
            break;
        case EBulletType::BT_9MM:
            BulletTypeName = TEXT("9mm");
            break;
        default:
            BulletTypeName = TEXT("Necunoscut");
            break;
        }

        UE_LOG(LogTemp, Warning, TEXT("Esti langa o cutie cu gloante de tip: %s"), *BulletTypeName);
        FString Message = FString::Printf(TEXT("Esti langa o cutie cu gloante de tip: %s"), *BulletTypeName);
        // Trimite mesajul delegate-ului
        OnBulletNearby.Broadcast(Message);
    }
}


void ABullet::OnPickupSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor->IsA(ACharacter::StaticClass()))
    {
        bCanBePickedUp = false;

        FString BulletTypeName;

        switch (BulletType)
        {
        case EBulletType::BT_556:
            BulletTypeName = TEXT("5.56mm");
            break;
        case EBulletType::BT_762:
            BulletTypeName = TEXT("7.62mm");
            break;
        case EBulletType::BT_9MM:
            BulletTypeName = TEXT("9mm");
            break;
        default:
            BulletTypeName = TEXT("Necunoscut");
            break;
        }

        UE_LOG(LogTemp, Warning, TEXT("Ai iesit din zona cutiei cu gloante de tip: %s"), *BulletTypeName);

        FString ExitMessage = FString::Printf(TEXT("Ai iesit din zona cutiei cu gloante de tip: %s"), *BulletTypeName);
        OnBulletNearby.Broadcast(ExitMessage);
    }
}


// Called every frame
void ABullet::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bCanBePickedUp)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC && PC->IsInputKeyDown(EKeys::P))
        {
            ACPP_Shooter2HUD* MyHUD = Cast<ACPP_Shooter2HUD>(PC->GetHUD());
            if (MyHUD)
            {
                MyHUD->AddBulletToInventory(BulletType, 30); // Funcție pe care o faci în HUD
            }

            UE_LOG(LogTemp, Warning, TEXT("Pick la glont. Il adaug in inventar si il distrug."));
            Destroy();
        }
    }
}

