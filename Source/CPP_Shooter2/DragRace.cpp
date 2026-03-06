#include "DragRace.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "CPP_Shooter2HUD.h"
#include "CPP_Shooter2GameMode.h"

ADragRace::ADragRace()
{
    PrimaryActorTick.bCanEverTick = true;

    bRaceStarted = false;
    bPlayerInZone = false;

    // Creează BoxComponent pentru zona de start
    StartTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StartTrigger"));
    StartTrigger->SetBoxExtent(FVector(2500.f, 500.f, 500.f));
    RootComponent = StartTrigger;

    StartTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    StartTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    StartTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    StartTrigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // mașini
    StartTrigger->SetGenerateOverlapEvents(true);

    StartTrigger->OnComponentBeginOverlap.AddDynamic(this, &ADragRace::OnOverlapBegin);
    StartTrigger->OnComponentEndOverlap.AddDynamic(this, &ADragRace::OnOverlapEnd);

    // Încarcă un mesh default (poți schimba în editor)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CarMeshObj(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/alfa-romeo.alfa-romeo'"));
    if (CarMeshObj.Succeeded())
    {
        CarMeshAsset = CarMeshObj.Object;
    }
}

void ADragRace::InitCars()
{
    const int NumCars = 5;

    // Reinițializează array-urile
    AI_Viteza.Init(0.f, NumCars);
    AI_Turatie.Init(0.f, NumCars);
    AI_Treapta.Init(1, NumCars);
    AI_Acceleratie.Init(0.f, NumCars);
    AI_putere.Init(0.f, NumCars);
    AI_raport.Init(0.f, NumCars);
    bHasFinished.Init(false, NumCars);

    // Setează puteri diferite pentru fiecare mașină (în Wați)
    AI_putere[0] = 120 * 736.0f;
    AI_putere[1] = 150 * 736.0f;
    AI_putere[2] = 180 * 736.0f;
    AI_putere[3] = 100 * 736.0f;
    AI_putere[4] = 80 * 736.0f;

    // Recalculează rapoartele pentru fiecare mașină
    for (int i = 0; i < NumCars; i++)
    {
        float c_local = -AI_putere[i];
        double delta = pow(b, 2) - 4 * a * c_local;
        if (delta >= 0)
        {
            double x2 = (-b + sqrt(delta)) / (2 * a);
            AI_raport[i] = (x2 / (nrtrepte * max)) * 3 / 2;
        }
        else
        {
            AI_raport[i] = 0.01f;
        }
    }

    // Dacă nu am spawn-at deja AI, îi spawnăm
    if (RaceCars.Num() == 0)
    {
        UWorld* World = GetWorld();
        if (!World) return;

        for (int i = 0; i < NumCars; i++)
        {
            FVector SpawnLocation = StartLineLocation + FVector(i * 1000.f, 0.f, 165.f);
            FRotator SpawnRotation = FRotator::ZeroRotator;

            AActor* CarActor = World->SpawnActor<AActor>(AActor::StaticClass(), SpawnLocation, SpawnRotation);
            if (!CarActor || !CarMeshAsset) continue;

            // Root gol
            USceneComponent* SceneRoot = NewObject<USceneComponent>(CarActor);
            CarActor->SetRootComponent(SceneRoot);
            SceneRoot->RegisterComponent();
            SceneRoot->SetMobility(EComponentMobility::Movable);

            // Mesh
            UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(CarActor);
            MeshComp->SetStaticMesh(CarMeshAsset);
            MeshComp->SetupAttachment(SceneRoot);
            MeshComp->RegisterComponent();
            MeshComp->SetMobility(EComponentMobility::Movable);

            CarActor->SetActorLocation(SpawnLocation);
            CarActor->SetActorRotation(SpawnRotation);

            RaceCars.Add(CarActor);
        }
    }
    else
    {
        // Dacă mașinile există deja, doar le repoziționăm
        for (int i = 0; i < RaceCars.Num(); i++)
        {
            if (RaceCars[i])
            {
                FVector SpawnLocation = StartLineLocation + FVector(i * 1000.f, 0.f, 200.f);
                RaceCars[i]->SetActorLocation(SpawnLocation);
                RaceCars[i]->SetActorRotation(FRotator::ZeroRotator);
            }
        }
    }
}


void ADragRace::BeginPlay()
{
    Super::BeginPlay();
    StartLineLocation = GetActorLocation();
    //aici incarc upgradeurile pe masina

    ACPP_Shooter2GameMode* GM = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
    if (GM)
    {
        int32 EngineLevel = GM->GetCurrentEngineLevel();
        int32 TurboLevel = GM->GetCurrentTurboLevel();
        int32 WeightLevel = GM->GetCurrentWeightLevel();

        UE_LOG(LogTemp, Log, TEXT("Current upgrades: Engine=%d, Turbo=%d, Weight=%d"),
            EngineLevel, TurboLevel, WeightLevel);
    }


    // Inițializează array-urile pentru mașini
    const int NumCars = 5;
    AI_Viteza.Init(0.f, NumCars);
    AI_Turatie.Init(0.f, NumCars);
    AI_Treapta.Init(1, NumCars);
    AI_Acceleratie.Init(0.f, NumCars);
    AI_putere.Init(0.f, NumCars);
    AI_raport.Init(0.f, NumCars); // Nou: fiecare mașină să aibă propriul raport

    // Setează puteri diferite pentru fiecare mașină (în Wați)
    AI_putere[0] = 120 * 736.0f; // 120 CP
    AI_putere[1] = 150 * 736.0f; // 150 CP
    AI_putere[2] = 180 * 736.0f; // 180 CP
    AI_putere[3] = 100 * 736.0f; // 100 CP
    AI_putere[4] = 80 * 736.0f;  // 80 CP

    // Calculează constante fizice comune
    a = k * sqrt(greutate);
    b = u * greutate * g;

    // Calculează raportul pentru fiecare mașină bazat pe puterea ei
    for (int i = 0; i < NumCars; i++)
    {
        float c_local = -AI_putere[i];
        double delta = pow(b, 2) - 4 * a * c_local;
        if (delta >= 0)
        {
            double x2 = (-b + sqrt(delta)) / (2 * a);
            AI_raport[i] = (x2 / (nrtrepte * max)) * 3 / 2;
        }
        else
        {
            AI_raport[i] = 0.01f; // Valoare default în caz de eroare
        }

        UE_LOG(LogTemp, Warning, TEXT("AI[%d] - Putere: %.0f CP, Raport: %.4f"),
            i, AI_putere[i] / 736.0f, AI_raport[i]);
    }

    // Spawn mașini AI
    UWorld* World = GetWorld();
    if (!World) return;

    for (int i = 0; i < NumCars; i++)
    {
        FVector SpawnLocation = GetActorLocation() + FVector(i * 1000.f, 0.f, 200.f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        AActor* CarActor = World->SpawnActor<AActor>(AActor::StaticClass(), SpawnLocation, SpawnRotation);
        if (!CarActor || !CarMeshAsset) continue;

        UE_LOG(LogTemp, Warning, TEXT("Spawnat AI car %d la locatie %s cu putere %.0f CP"),
            i + 1, *SpawnLocation.ToString(), AI_putere[i] / 736.0f);

        // Root gol
        USceneComponent* SceneRoot = NewObject<USceneComponent>(CarActor);
        CarActor->SetRootComponent(SceneRoot);
        SceneRoot->RegisterComponent();
        SceneRoot->SetMobility(EComponentMobility::Movable);

        // Creează mesh
        UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(CarActor);
        MeshComp->SetStaticMesh(CarMeshAsset);
        MeshComp->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
        MeshComp->SetupAttachment(SceneRoot);
        MeshComp->RegisterComponent();
        MeshComp->SetMobility(EComponentMobility::Movable);
        MeshComp->SetVisibility(true);
        MeshComp->SetHiddenInGame(false);

        // Actualizează transform-ul
        MeshComp->SetWorldLocation(SpawnLocation);
        MeshComp->SetWorldRotation(SpawnRotation);

        RaceCars.Add(CarActor);
    }
}

void ADragRace::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bRaceStarted && bPlayerInZone)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            if (PC->WasInputKeyJustPressed(EKeys::B))
            {
                StartRace();
            }
        }
    }

    // Desenează zona de start
    FBoxSphereBounds Bounds = StartTrigger->Bounds;
    DrawDebugBox(
        GetWorld(),
        Bounds.Origin,
        Bounds.BoxExtent,
        GetActorQuat(),
        FColor::Green,
        false,
        -1.f,
        0,
        5.f
    );

    float Distance = MAX_FLT;
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (APawn* PlayerPawn = PC->GetPawn())
        {
            Distance = FVector::Dist(PlayerPawn->GetActorLocation(), Bounds.Origin);
            /*if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(
                    1, // ID fix
                    0.f,
                    FColor::Yellow,
                    FString::Printf(TEXT("Distanta pana la cutie: %.2f cm"), Distance)
                );
            }*/
        }
    }

    // ---- PORNIRE CURSA ----
    if (!bRaceStarted && Distance < 1500.f)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            if (PC->WasInputKeyJustPressed(EKeys::B))
            {
                ResetRace();
                StartRace();
            }
        }
    }

    if (bRaceStarted)
    {
        MoveAICars(DeltaTime);
    }
    if (bRaceStarted && !bPlayerFinished)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            if (APawn* PlayerPawn = PC->GetPawn())
            {
                float DistanceTravelled = FVector::Dist(StartLineLocation, PlayerPawn->GetActorLocation());

                if (DistanceTravelled >= 91000.f)
                {
                    bPlayerFinished = true;
                    PlayerFinishTime = GetWorld()->GetTimeSeconds() - RaceStartTime;

                    UE_LOG(LogTemp, Warning, TEXT("PLAYER a terminat cursa in %.2f secunde"), PlayerFinishTime);

                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(
                            500,  // id unic
                            10.f,
                            FColor::Cyan,
                            FString::Printf(TEXT("PLAYER FINISH: %.2f secunde"), PlayerFinishTime)
                        );
                    }

                    // verificăm dacă toată lumea a terminat
                    CheckRaceFinished();
                }
            }
        }
    }

}

void ADragRace::StartRace()
{
    bRaceStarted = true;
    RaceStartTime = GetWorld()->GetTimeSeconds();

    FinishTimes.Init(-1.f, RaceCars.Num());
    bHasFinished.Init(false, RaceCars.Num());
    bPlayerFinished = false;
    PlayerFinishTime = -1.f;

    UE_LOG(LogTemp, Warning, TEXT("Start Drag Race!"));
}

void ADragRace::MoveAICars(float DeltaTime)
{
    for (int i = 0; i < RaceCars.Num(); i++)
    {
        if (!RaceCars[i]) continue;

        float ForwardInput = 1.f; // AI accelerează automat

        // Referințe pentru valorile specifice fiecărei mașini
        float& viteza = AI_Viteza[i];
        float& turatie = AI_Turatie[i];
        int& treapta = AI_Treapta[i];
        float& acceleratie = AI_Acceleratie[i];
        float& putere = AI_putere[i];           // Puterea specifică acestei mașini
        float& raport = AI_raport[i];           // Raportul specific acestei mașini

        if (ForwardInput > 0.f && bHasFinished[i] == false)
        {
            // Calculează puterea medie bazată pe turația acestei mașini
            float puteremed = 0.f;
            if (turatie < max / 8)
                puteremed = (putere / 8.f) * (3.f / 2.f);
            else if (turatie >= 2 * max / 3.f)
                puteremed = putere;
            else
                puteremed = 3.f * (putere * turatie) / (2.f * max);

            // Calculează accelerația pentru această mașină specifică
            acceleratie = (puteremed - k * sqrt(greutate) * viteza * (viteza - vant / 3.6f) - u * greutate * g * viteza) / (greutate * viteza)-panta * g / 100.f;
            acceleratie = FMath::Clamp(acceleratie, 0.f, 15.f); // limita acceleratie

            // Aplică accelerația (fără diviziunea cu 5 - era prea lentă)
            viteza = FMath::Max(0.f, viteza + acceleratie * DeltaTime);

            // Calculează turația folosind raportul specific acestei mașini
            turatie = viteza / (raport * treapta);

            // Verifică schimbarea treptei
            if (turatie >= 3 * max / 4.f && treapta < nrtrepte)
            {
                treapta++;
                turatie = turatie * (treapta - 1) / treapta; // ajustare turatie
            }
        }
        else
        {
            viteza = 0;
            turatie = 0;
            treapta = 1;
        }

        // Deplasare
        FVector Forward = RaceCars[i]->GetActorForwardVector();
        FVector RotatedForward = FVector(-Forward.Y, Forward.X, Forward.Z);
        FVector NewLocation = RaceCars[i]->GetActorLocation() + RotatedForward * viteza * DeltaTime * 27.7778*2; //conversie m/s la cm/s
        RaceCars[i]->SetActorLocation(NewLocation);

        // Debug vizual pentru fiecare mașină
        FColor DebugColor = FColor::Red;
        switch (i) {
        case 0: DebugColor = FColor::Red; break;
        case 1: DebugColor = FColor::Blue; break;
        case 2: DebugColor = FColor::Green; break;
        case 3: DebugColor = FColor::Yellow; break;
        case 4: DebugColor = FColor::Purple; break;
        }

       /* DrawDebugBox(
            GetWorld(),
            RaceCars[i]->GetActorLocation(),
            FVector(200.f, 200.f, 200.f),
            DebugColor,
            false,
            -1.f,
            0,
            2.f
        );*/

        // Afișează statistici pentru toate mașinile pe ecran
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                10 + i, // ID-uri diferite pentru fiecare mașină
                0.f,
                DebugColor,
                FString::Printf(TEXT("AI[%d]: %.1f km/h (%.0f CP, Tr:%d)"),
                    i, viteza * 3.6f, putere / 736.0f, treapta)
            );
        }
        float DistanceTravelled = FVector::Dist(StartLineLocation, NewLocation);

        if (!bHasFinished[i] && DistanceTravelled >= 91000.f)
        {
            bHasFinished[i] = true;
            viteza = 0.f;
            acceleratie = 0.f;

            float FinishTime = GetWorld()->GetTimeSeconds() - RaceStartTime;
            FinishTimes[i] = FinishTime;

            UE_LOG(LogTemp, Warning, TEXT("AI[%d] a terminat cursa in %.2f secunde"), i, FinishTime);

            // Afișează pe ecran
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(
                    100 + i,
                    10.f,
                    FColor::Cyan,
                    FString::Printf(TEXT("AI[%d] FINISH: %.2f secunde"), i, FinishTime)
                );
            }
        }
        else if (!bHasFinished[i])
        {
            // Mașina continuă să se miște doar dacă nu a terminat
            RaceCars[i]->SetActorLocation(NewLocation);
        }
    }
    CheckRaceFinished();
}
/*void ADragRace::UpdateHUD()
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
}*/
void ADragRace::CheckRaceFinished()
{
    if (bRaceStarted == false)
    {
        return;
    }
    // Verifică dacă toate AI au terminat
    bool bAllAIFinished = true;
    for (bool Finished : bHasFinished)
    {
        if (!Finished)
        {
            bAllAIFinished = false;
            break;
        }
    }

    // Toți au terminat doar dacă și player-ul a terminat
    if (bAllAIFinished && bPlayerFinished)
    {
        UE_LOG(LogTemp, Warning, TEXT("TOATA LUMEA a terminat cursa!"));

        // Creează un array de perechi (nume, timp)
        TArray<TPair<FString, float>> Results;

        // Adaugă AI
        for (int i = 0; i < FinishTimes.Num(); i++)
        {
            Results.Add(TPair<FString, float>(
                FString::Printf(TEXT("AI[%d]"), i),
                FinishTimes[i]
            ));
        }

        // Adaugă player
        Results.Add(TPair<FString, float>(TEXT("PLAYER"), PlayerFinishTime));

        // Sortează după timp
        Results.Sort([](const TPair<FString, float>& A, const TPair<FString, float>& B) {
            return A.Value < B.Value;
            });
        int PlayerRank = -1;
        // Afișează clasamentul
        for (int Rank = 0; Rank < Results.Num(); Rank++)
        {
            FString Msg = FString::Printf(TEXT("Locul %d: %s - %.2f secunde"),
                Rank + 1, *Results[Rank].Key, Results[Rank].Value);

            UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(
                    600 + Rank,
                    15.f,
                    FColor::Green,
                    Msg
                );
            }

            if (Results[Rank].Key == "PLAYER")
            {
                PlayerRank = Rank + 1; // locurile sunt 1-based
            }
        }
        int Reward = 0;
        if (PlayerRank > 0)
        {
            switch (PlayerRank)
            {
            case 1: Reward = 200; break; // locul 1
            case 2: Reward = 135; break;  // locul 2
            case 3: Reward = 100; break;  // locul 3
            case 4: Reward = 70; break;  // locul 4
            case 5: Reward = 50; break;  // locul 5
            case 6: Reward = 35; break;  // ultimul
            default: Reward = 20; break;  // fallback
            }

            // aici faci efectiv update la banii playerului
            // de ex. dacă ai o variabilă globală sau în PlayerState:
            // PlayerState->AddMoney(Reward);

            UE_LOG(LogTemp, Warning, TEXT("PLAYER a primit %d banuti pentru locul %d"), Reward, PlayerRank);

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(
                    999,
                    10.f,
                    FColor::Yellow,
                    FString::Printf(TEXT("Ai castigat %d banuti!"), Reward)
                );
            }
        }
        bRaceStarted = false;

        //Aici adaug rewardul
        ACPP_Shooter2GameMode* GM = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
        if (GM)
        {
            int32 NewCoins = GM->GetCurrentCoins() + Reward;
            GM->SetCurrentCoins(NewCoins);

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
    }
}


void ADragRace::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor) return;

    // Playerul pe jos
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC && PC->GetPawn() == OtherActor)
    {
        bPlayerInZone = true;
        UE_LOG(LogTemp, Warning, TEXT("Player in start zone"));
        return;
    }

    // Mașina player
    ACar* Car = Cast<ACar>(OtherActor);
    if (Car && Car->IsPlayerControlled())
    {
        bPlayerInZone = true;
        UE_LOG(LogTemp, Warning, TEXT("Player's car in start zone"));
    }
}

void ADragRace::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor) return;

    // Playerul pe jos
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC && PC->GetPawn() == OtherActor)
    {
        bPlayerInZone = false;
        UE_LOG(LogTemp, Warning, TEXT("Player left start zone"));
        return;
    }

    // Mașina player
    ACar* Car = Cast<ACar>(OtherActor);
    if (Car && Car->IsPlayerControlled())
    {
        bPlayerInZone = false;
        UE_LOG(LogTemp, Warning, TEXT("Player's car left start zone"));
    }
}
void ADragRace::ResetRace()
{
    for (AActor* Car : RaceCars)
    {
        if (Car) Car->Destroy();
    }
    RaceCars.Empty();
    bRaceStarted = false;
    bPlayerFinished = false;
    PlayerFinishTime = -1.f;

    InitCars(); // refolosim logica din BeginPlay pentru resetare

    UE_LOG(LogTemp, Warning, TEXT("Race reset complet!"));
}
