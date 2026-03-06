#include "Energy.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "CPP_Shooter2HUD.h"

// Sets default values
AEnergy::AEnergy()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(RootComponent);
	PickupSphere->SetSphereRadius(100.f);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);

	CapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CapMesh"));
	CapMesh->SetupAttachment(BodyMesh);

	// Încarcă mesh-urile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DrinkAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/Energy_drink.Energy_drink'"));
	if (DrinkAsset.Succeeded())
	{
		Mesh_Drink = DrinkAsset.Object;
		UE_LOG(LogTemp, Log, TEXT("Energy Drink mesh loaded successfully."));
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
	bCanBePickedUp = false;
}

// Called when the game starts or when spawned
void AEnergy::BeginPlay()
{
	Super::BeginPlay();

	switch (EnergyType)
	{
	case EEnergyType::E_EnergyDrink:
		BodyMesh->SetStaticMesh(Mesh_Drink);
		BodyMesh->SetWorldScale3D(FVector(2.0f));
		break;

	case EEnergyType::E_Painkiller:
		BodyMesh->SetStaticMesh(Mesh_Painkiller);
		BodyMesh->SetWorldScale3D(FVector(0.08f));
		break;

	case EEnergyType::E_AdrenalineSyringe:
		BodyMesh->SetStaticMesh(Mesh_Adrenaline);
		BodyMesh->SetWorldScale3D(FVector(0.8f));
		break;
	}

	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnergy::OnPickupSphereOverlap);
	PickupSphere->OnComponentEndOverlap.AddDynamic(this, &AEnergy::OnPickupSphereEndOverlap);
}
void AEnergy::OnPickupSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		bCanBePickedUp = true;

		FString EnergyName;
		switch (EnergyType)
		{
		case EEnergyType::E_EnergyDrink:
			EnergyName = TEXT("Energy Drink");
			break;
		case EEnergyType::E_Painkiller:
			EnergyName = TEXT("Painkiller");
			break;
		case EEnergyType::E_AdrenalineSyringe:
			EnergyName = TEXT("Adrenaline Syringe");
			break;
		default:
			EnergyName = TEXT("Unknown Energy Item");
			break;
		}

		UE_LOG(LogTemp, Warning, TEXT("Esti langa un obiect de tip: %s"), *EnergyName);
	}
}

void AEnergy::OnPickupSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		bCanBePickedUp = false;

		FString EnergyName;
		switch (EnergyType)
		{
		case EEnergyType::E_EnergyDrink:
			EnergyName = TEXT("Energy Drink");
			break;
		case EEnergyType::E_Painkiller:
			EnergyName = TEXT("Painkiller");
			break;
		case EEnergyType::E_AdrenalineSyringe:
			EnergyName = TEXT("Adrenaline Syringe");
			break;
		default:
			EnergyName = TEXT("Unknown Energy Item");
			break;
		}

		UE_LOG(LogTemp, Warning, TEXT("Ai iesit din zona obiectului de tip: %s"), *EnergyName);
	}
}
void AEnergy::InitEnergy(EEnergyType NewType)
{
	EnergyType = NewType;

	// Setezi mesh-ul direct aici
	switch (EnergyType)
	{
	case EEnergyType::E_EnergyDrink:
		BodyMesh->SetStaticMesh(Mesh_Drink);
		BodyMesh->SetWorldScale3D(FVector(2.0f));
		break;
	case EEnergyType::E_Painkiller:
		BodyMesh->SetStaticMesh(Mesh_Painkiller);
		BodyMesh->SetWorldScale3D(FVector(0.08f));
		break;
	case EEnergyType::E_AdrenalineSyringe:
		BodyMesh->SetStaticMesh(Mesh_Adrenaline);
		BodyMesh->SetWorldScale3D(FVector(0.8f));
		break;
	}
}


// Called every frame
void AEnergy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanBePickedUp)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC && PC->IsInputKeyDown(EKeys::P))
		{
			// Aici pui ce vrei să se întâmple când jucătorul ia obiectul, ex:
			UE_LOG(LogTemp, Warning, TEXT("Ai luat obiectul Energy de tip: %d"), (int32)EnergyType);
			ACPP_Shooter2HUD* MyHUD = Cast<ACPP_Shooter2HUD>(PC->GetHUD());
			if (MyHUD)
			{
				if ((int32)EnergyType == 0)
					MyHUD->EnergyDrinks++;
				if ((int32)EnergyType == 1)
					MyHUD->Painkillers++;
				if ((int32)EnergyType == 2)
					MyHUD->Adrenaline_Syringe++;

			}


			// De exemplu distrugi obiectul după "pick-up"
			Destroy();
		}
	}
}
