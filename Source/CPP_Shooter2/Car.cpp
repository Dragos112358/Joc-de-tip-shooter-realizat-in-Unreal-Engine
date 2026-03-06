#include "Car.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/CollisionProfile.h"
#include "CPP_Shooter2GameMode.h"

ACar::ACar()
{
    PrimaryActorTick.bCanEverTick = true;

    CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
    SetRootComponent(CarMesh);
    CarMesh->SetSimulatePhysics(true);
    CarMesh->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);

    DriverSeat = CreateDefaultSubobject<USceneComponent>(TEXT("DriverSeat"));
    DriverSeat->SetupAttachment(RootComponent);
    DriverSeat->SetRelativeLocation(FVector(30.f, -30.f, 100.f)); // Ajustează după model

    CarCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CarCamera"));
    CarCamera->SetupAttachment(DriverSeat);
    CarCamera->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
    CarCamera->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    CarCamera->bUsePawnControlRotation = false;
    CarCamera->SetAutoActivate(false);

    OriginalPawn = nullptr;
    PlayerController = nullptr;
    bPlayerInside = false;

    ToggleCooldown = 0.5f;
    TimeSinceLastToggle = ToggleCooldown;

    velocity = 0.001;

    // Exemplu setare mesh (schimbă cu calea ta)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/alfa-romeo.alfa-romeo'"));
    if (MeshAsset.Succeeded())
    {
        CarMesh->SetStaticMesh(MeshAsset.Object);
    }
    CarMesh->SetNotifyRigidBodyCollision(true);
    CarMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CarMesh->BodyInstance.bUseCCD = true; // Activare CCD
    CarMesh->SetLinearDamping(2.0f);
    CarMesh->SetAngularDamping(3.0f);
    CarMesh->SetEnableGravity(true);
    CarMesh->SetPhysicsLinearVelocity(FVector::ZeroVector); // optional reset
    CarMesh->SetMassOverrideInKg(NAME_None, greutate, true); // opțional, dacă vrei să ajustezi masa
    //CarMesh->SetGravityScale(2.0f); // 2x gravitație

    AutoPossessPlayer = EAutoReceiveInput::Disabled;

    // Inițializări pentru calculul puterii, raportului etc.
    putere = 736 * 150;  // Exemplu valori - setează-le tu
    greutate = 1120;
    u = 0.02;
    g = 9.81;
    nrtrepte = 5;
    max = 8000;
    k = 0.713949191;
    int32 EngineLevel = 1;
    int32 TurboLevel = 1;
    int32 WeightLevel = 1;

    ACPP_Shooter2GameMode* GM = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
    if (GM)
    {
        EngineLevel = GM->GetCurrentEngineLevel();
        TurboLevel = GM->GetCurrentTurboLevel();
        WeightLevel = GM->GetCurrentWeightLevel();

        UE_LOG(LogTemp, Log, TEXT("Current upgrades: Engine=%d, Turbo=%d, Weight=%d"),
            EngineLevel, TurboLevel, WeightLevel);
    }
    putere = 736 * (150 + (EngineLevel-1)*20+(TurboLevel-1)*20);
    a = k * sqrt(greutate);
    b = u * greutate * g;
    c = -putere;
    double delta = pow(b, 2) - 4 * a * c;
    double x2 = (-b + sqrt(delta)) / (2 * a);
    raport = (x2 / (nrtrepte * max)) * 3 / 2;

    turatie = 0.f;
    treapta = 1;
    viteza = 0.f;
    panta = 0.f;
    vant = 0.f;
}

void ACar::BeginPlay()
{
    Super::BeginPlay();

    PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (PlayerController)
    {
        OriginalPawn = PlayerController->GetPawn();
    }

    CarMesh->SetCenterOfMass(FVector(0.f, 0.f, -30.f));
}

void ACar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    int32 EngineLevel2 = 1;
    int32 TurboLevel2 = 1;
    int32 WeightLevel2 = 1;
    if (!PlayerController)
    {
        PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        if (!PlayerController) return;
    }
    ACPP_Shooter2GameMode* GM4 = Cast<ACPP_Shooter2GameMode>(UGameplayStatics::GetGameMode(this));
    if (GM4)
    {
        EngineLevel2 = GM4->GetCurrentEngineLevel();
        TurboLevel2 = GM4->GetCurrentTurboLevel();
        WeightLevel2 = GM4->GetCurrentWeightLevel();
    }
    putere = 736 * (150 + (EngineLevel2 - 1) * 17.5f + (TurboLevel2 - 1) * 17.5f);
    greutate = 1120 - (10 * WeightLevel2);
    a = k * sqrt(greutate);
    b = u * greutate * g;
    c = -putere;
    double delta2 = pow(b, 2) - 4 * a * c;
    double x3 = (-b + sqrt(delta2)) / (2 * a);
    raport = (x3 / (nrtrepte * max)) * 3 / 2;

    TimeSinceLastToggle += DeltaTime;

    if (PlayerController->WasInputKeyJustPressed(EKeys::C) && TimeSinceLastToggle >= ToggleCooldown)
    {
        TimeSinceLastToggle = 0.f;

        if (!bPlayerInside)
        {
            // Intrare în mașină
            // Intrare în mașină
            if (OriginalPawn && OriginalPawn != this)
            {
                OriginalPawn->SetActorLocation(DriverSeat->GetComponentLocation());
                OriginalPawn->SetActorRotation(GetActorRotation());
                OriginalPawn->SetActorHiddenInGame(true);
                OriginalPawn->DisableInput(PlayerController);

                // Oprește fizica imediat ce intri
                CarMesh->SetSimulatePhysics(false);
                CarMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
                CarMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
                CarMesh->SetWorldRotation(FRotator(0.f, GetActorRotation().Yaw, 0.f));

                PlayerController->Possess(this);
                PlayerController->SetControlRotation(GetActorRotation());

                bPlayerInside = true;

                if (auto Cam = OriginalPawn->FindComponentByClass<UCameraComponent>())
                    Cam->Deactivate();

                CarCamera->Activate();

                PlayerController->SetIgnoreLookInput(true);

                UE_LOG(LogTemp, Warning, TEXT("Ai intrat în mașină."));
                CarMesh->SetSimulatePhysics(true);
            }
        }
        else
        {
            // Ieșire din mașină
            PlayerController->SetIgnoreLookInput(false);

            if (OriginalPawn)
            {
                // Dezactivează temporar fizica pe mașină
                CarMesh->SetSimulatePhysics(false);
                CarMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
                CarMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

                FVector ExitLocation = GetActorLocation()
                    + GetActorRightVector() * -150.f
                    + GetActorForwardVector() * -100.f
                    + FVector(0.f, 0.f, 50.f);

                PlayerController->UnPossess();

                // Mică întârziere pentru a lăsa sistemul să proceseze unpossess
                GetWorld()->GetTimerManager().SetTimer(
                    ExitTimerHandle,
                    [this, ExitLocation]()
                    {
                        if (OriginalPawn && PlayerController)
                        {
                            // Repoziționează jucătorul cu teleport (nu va aplica fizica)
                            OriginalPawn->SetActorLocation(ExitLocation, false, nullptr, ETeleportType::TeleportPhysics);
                            OriginalPawn->SetActorHiddenInGame(false);

                            // Repossess și input
                            PlayerController->Possess(OriginalPawn);
                            OriginalPawn->EnableInput(PlayerController);

                            if (auto Cam = OriginalPawn->FindComponentByClass<UCameraComponent>())
                                Cam->Activate();

                            PlayerController->SetViewTargetWithBlend(OriginalPawn, 0.2f);
                            PlayerController->SetIgnoreLookInput(false);
                            PlayerController->SetIgnoreMoveInput(false);

                            // Reactivează fizica pe mașină după ce jucătorul nu mai interferează
                            CarMesh->SetSimulatePhysics(true);
                        }
                    },
                    0.1f, // întârziere scurtă
                    false
                );

                bPlayerInside = false;
                CarCamera->Deactivate();

                UE_LOG(LogTemp, Warning, TEXT("Ai ieșit din mașină."));
            }
        }
    }

    if (bPlayerInside)
    {

        // Aplicăm forța în funcție de masă
        if (turatie <= 3 * max / 10 && treapta <= nrtrepte && treapta >= 2)
        {
            viteza = viteza - acceleratie * 0.15f;
            turatie = turatie * treapta / (treapta - 1);
            treapta--;
        }



        FVector CarLocation2 = GetActorLocation();
        FVector GroundCheckStart2 = CarLocation2;
        FVector GroundCheckEnd2 = CarLocation2 - FVector(0, 0, MaxGroundDistance);

        FHitResult GroundHit2;
        FCollisionQueryParams QueryParams2;
        QueryParams2.AddIgnoredActor(this);
        QueryParams2.bTraceComplex = false;

        bool bHitGround2 = GetWorld()->LineTraceSingleByChannel(
            GroundHit2,
            GroundCheckStart2,
            GroundCheckEnd2,
            ECC_WorldStatic,
            QueryParams2
        );
        if (bHitGround2 == false) {
            float GravityScale = 7.0f;
            FVector ExtraGravity = FVector(0.f, 0.f, GetWorld()->GetGravityZ() * (GravityScale - 1));
            CarMesh->AddForce(ExtraGravity * CarMesh->GetMass());
        }
        if (bHitGround2)
        {
            FVector GroundPosition2 = GroundHit2.Location;
            FVector GroundNormal2 = GroundHit2.Normal;

            // Unghi între normalul solului și axa Z (verticală)
            float SlopeAngleRad2 = FMath::Acos(FVector::DotProduct(GroundNormal2, FVector::UpVector));

            // Pantă în procente
            float SlopePercent2 = FMath::Tan(SlopeAngleRad2) * 100.0f;

            // Semn pentru pantă (pozitiv = urci, negativ = cobori)
            FVector ForwardDir23 = GetActorForwardVector();
            float Sign2 = FMath::Sign(FVector::DotProduct(ForwardDir23, GroundNormal2));

            // Setează panta finală în procente
            panta = SlopePercent2 * Sign2;
        }
        // Controlul mașinii când jucătorul este înăuntru
        ForwardInput = 0.f;
        TurnInput = 0.f;

        if (PlayerController->IsInputKeyDown(EKeys::W))
            ForwardInput = 1.f;
        else if (PlayerController->IsInputKeyDown(EKeys::S))
            ForwardInput = -1.f;

        if (PlayerController->IsInputKeyDown(EKeys::A))
            TurnInput = -1.f;
        else if (PlayerController->IsInputKeyDown(EKeys::D))
            TurnInput = 1.f;

        FVector ForwardDir = GetActorRightVector();
        ForwardDir.Z = 0.f;
        ForwardDir.Normalize();

        // Modelul de accelerație/turație din codul tău
        puteremed = 0.f;
        acceleratie = 0.f;

        if (ForwardInput > 0.f)
        {
            if (turatie < max / 8)
            {
                puteremed = (putere / 8) * (3.f / 2.f);
            }
            else if (turatie >= 2 * max / 3)
            {
                puteremed = putere;
            }
            else if (turatie > max / 8 && turatie <= 2 * max / 3)
            {
                puteremed = 3.f * (putere * turatie) / (2.f * max);
            }

            acceleratie = (puteremed - k * sqrt(greutate) * viteza * (viteza - vant / 3.6f) - u * greutate * g * viteza) / (greutate * viteza) - panta * g / 100.f;
            if (acceleratie > x)
                acceleratie = x;

            viteza = viteza + acceleratie * DeltaTime;
            turatie = viteza / (raport * treapta);

            if (turatie >= 3 * max / 4 && treapta <= nrtrepte)
            {
                viteza = viteza - ((k * sqrt(greutate) * viteza * (viteza - vant / 3.6f) + u * greutate * g * viteza) / (greutate * viteza) - panta * g / 100.f) * 0.15f;
                turatie = turatie * treapta / (treapta + 1);
                treapta++;
            }
        }
        else if (ForwardInput < 0.f && viteza > 0)
        {
            viteza = viteza - 9.81f * DeltaTime;
            if (viteza < 0)
                viteza = 0;
        }

        if (turatie <= 3 * max / 10 && treapta < nrtrepte && treapta >= 2)
        {
            viteza = viteza - acceleratie * 0.15f;
            turatie = turatie * treapta / (treapta - 1);
            treapta--;
        }

        if (viteza > 0.01f && ForwardInput == 0.f) // evit divizare cu 0
        {
            acceleratie = 15.f * (-k * sqrt(greutate) * viteza * (viteza - vant / 3.6f)
                - u * greutate * g * viteza) / (greutate * viteza)
                - panta * g / 100.f;

            if (acceleratie > x - panta * g / 100.f)
                acceleratie = x - panta * g / 100.f;

            viteza = FMath::Max(0.f, viteza + acceleratie * DeltaTime);
        }
        // Obține viteza curentă din fizică
        FVector CurrentVelocity = CarMesh->GetPhysicsLinearVelocity();

        // Vectorul înainte corect
        FVector ForwardDir15 = GetActorRightVector();
        ForwardDir15.Z = 0.f;
        ForwardDir15.Normalize();

        // Proiecția vitezei pe direcția înainte
        float ForwardSpeed = FVector::DotProduct(CurrentVelocity, ForwardDir15); // cm/s

        // Logica de frânare naturală
        if (ForwardInput == 0.f)
        {
            // Frânare graduală
            float BrakeRate = 0.98f; // 0.98 = 2% scădere pe frame
            ForwardSpeed *= BrakeRate;

            // Dacă e aproape de zero, oprește complet
            if (FMath::Abs(ForwardSpeed) < 1.f)
                ForwardSpeed = 0.f;
        }
        else
        {
            // Dacă vrei schimbare de direcție naturală:
            if ((ForwardInput > 0.f && ForwardSpeed < 0.f) || (ForwardInput < 0.f && ForwardSpeed > 0.f))
            {
                // Apasă invers → frânare mai tare
                ForwardSpeed *= 0.95f;
                if (FMath::Abs(ForwardSpeed) < 5.f) // prag în cm/s
                    ForwardSpeed = 0.f;
            }
            else
            {
                // Aplică viteza calculată din "viteza" ta (în m/s → cm/s)
                ForwardSpeed = FMath::Sign(ForwardInput) * FMath::Abs(viteza) * 100.f * 2;
            }
        }

        // Creează noul vector de viteză
        FVector NewVelocity = ForwardDir15 * ForwardSpeed;

        // Păstrează componenta Z pentru gravitație
        NewVelocity.Z = CurrentVelocity.Z;

        // Setează viteza
        CarMesh->SetPhysicsLinearVelocity(NewVelocity);


        // Actualizează turația și treapta
        if (viteza > 0.f)
            turatie = viteza / (raport * treapta);
        else
            turatie = 0.f;

        FVector Velocity22 = CarMesh->GetComponentVelocity();
        float Speed22 = Velocity22.Size();

        if (FMath::Abs(TurnInput) > 0.f && Speed22 > KINDA_SMALL_NUMBER)
        {
            FRotator RotationDelta(0.f, TurnInput * 50.f * DeltaTime, 0.f);
            FQuat QuatRotation(RotationDelta);
            CarMesh->AddWorldRotation(QuatRotation);
        }


        // Corectare orientare pe X (pitch) și Y (roll)
        FVector CurrentUp = CarMesh->GetUpVector();
        FVector DesiredUp = FVector::UpVector;

        FVector RotationAxis = FVector::CrossProduct(CurrentUp, DesiredUp);
        float RotationAngle = FMath::Acos(FMath::Clamp(FVector::DotProduct(CurrentUp, DesiredUp), -1.f, 1.f));

        float Mass = CarMesh->GetMass();
       float TorqueStrength = 10000000.f;
        float DampingFactor = 0.1f;

        FVector CorrectiveTorque = RotationAxis * (RotationAngle * TorqueStrength * Mass);
        CorrectiveTorque.Z = 0.f;

        FVector AngularVelocity = CarMesh->GetPhysicsAngularVelocityInDegrees();
        FVector DampingTorque = -AngularVelocity * DampingFactor;
        DampingTorque.Z = 0.f;

        CarMesh->AddTorqueInDegrees(CorrectiveTorque + DampingTorque);
        // ADAUGĂ ACEST COD DUPĂ CONTROLUL MAȘINII:

   // Detectare sol și stabilizare
        FVector CarLocation = GetActorLocation();
        FVector GroundCheckStart = CarLocation;
        FVector GroundCheckEnd = CarLocation - FVector(0, 0, MaxGroundDistance);

        FHitResult GroundHit;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);
        QueryParams.bTraceComplex = false;

        bool bHitGround = GetWorld()->LineTraceSingleByChannel(
            GroundHit,
            GroundCheckStart,
            GroundCheckEnd,
            ECC_WorldStatic,
            QueryParams
        );

        if (bHitGround)
        {
            FVector GroundPosition = GroundHit.Location;
            FVector GroundNormal = GroundHit.Normal;

            // Unghi între normalul solului și axa Z (verticală)
            float SlopeAngleRad = FMath::Acos(FVector::DotProduct(GroundNormal, FVector::UpVector));

            // Pantă în procente
            float SlopePercent = FMath::Tan(SlopeAngleRad) * 100.0f;

            // Semn pentru pantă (pozitiv = urci, negativ = cobori)
            FVector ForwardDir23 = GetActorForwardVector();
            float Sign = FMath::Sign(FVector::DotProduct(ForwardDir23, GroundNormal));

            // Setează panta finală în procente
            panta = SlopePercent * Sign;

            if (bHasValidGroundPosition)
            {
                // Calculează diferența pe Z față de ultima poziție cunoscută
                float ZDifference = FMath::Abs(GroundPosition.Z - LastGroundPosition.Z);

                // Doar dacă diferența este mai mare decât pragul, permite schimbarea bruscă
                if (ZDifference > MinBumpThreshold)
                {
                    // Bump mare detectat - lasă fizica să funcționeze normal
                    LastGroundPosition = GroundPosition;
                }
                else
                {
                    // Bump mic - aplică stabilizare
                    float DesiredHeight = LastGroundPosition.Z + 100.0f; // 100cm deasupra solului
                    float CurrentHeight = CarLocation.Z;
                    float HeightDifference = DesiredHeight - CurrentHeight;

                    // Aplică forță de stabilizare doar pe Z
                    if (FMath::Abs(HeightDifference) > 5.0f) // Toleranță de 5cm
                    {
                        FVector StabilizationForce = FVector(0, 0, HeightDifference * GroundStabilizationForce);
                        CarMesh->AddForce(StabilizationForce);
                    }

                    // Limitează viteza pe Z pentru bumpurile mici
                    FVector CurrentVelocity22 = CarMesh->GetPhysicsLinearVelocity();
                    if (FMath::Abs(CurrentVelocity22.Z) > 3000.0f && ZDifference < MinBumpThreshold)
                    {
                        CurrentVelocity22.Z = FMath::Clamp(CurrentVelocity22.Z, -3000.0f, 3000.0f);
                        CarMesh->SetPhysicsLinearVelocity(CurrentVelocity22);
                    }
                }
            }
            else
            {
                // Prima detectare a solului
                LastGroundPosition = GroundPosition;
                bHasValidGroundPosition = true;
            }
        }

        // Stabilizare suplimentară pentru orientare
        /*FRotator CurrentRotation = GetActorRotation();

        // Limitează pitch și roll pentru bumpurile mici
        float MaxPitchRoll = 50.0f; // grade maxime de înclinare

        FRotator StableRotation = CurrentRotation;
        StableRotation.Pitch = FMath::Clamp(StableRotation.Pitch, -MaxPitchRoll, MaxPitchRoll);
        StableRotation.Roll = FMath::Clamp(StableRotation.Roll, -MaxPitchRoll, MaxPitchRoll);

        // Aplică rotația stabilizată gradual
        FRotator RotationDiff = StableRotation - CurrentRotation;
        if (RotationDiff.GetManhattanDistance(FRotator::ZeroRotator) > 1.0f)
        {
            FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, StableRotation, DeltaTime, 5.0f);
            SetActorRotation(SmoothRotation);
        }

        // Ajustare suplimentară a centrului de masă pentru stabilitate
        CarMesh->SetCenterOfMass(FVector(0.f, 0.f, -50.f)); // Centru de masă mai jos

        // Reducere bounce pentru contactul cu solul
        if (CarMesh->GetPhysicsLinearVelocity().Z < -500.0f) // Cădere rapidă
        {
            FVector Velocity = CarMesh->GetPhysicsLinearVelocity();
            Velocity.Z = FMath::Max(Velocity.Z, -500.0f); // Limitează viteza de cădere
            CarMesh->SetPhysicsLinearVelocity(Velocity);
        }*/
    }
}