#include "LiftActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h" // pentru GEngine->AddOnScreenDebugMessage
#include "DrawDebugHelpers.h" // pentru a vizualiza TriggerBox

// Sets default values
ALiftActor::ALiftActor()
{
    PrimaryActorTick.bCanEverTick = true;
    LiftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LiftMesh"));
    RootComponent = LiftMesh;
    //TargetLocation = StartLocation + FVector(0, 0, 500);
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
    //TriggerBox->SetBoxExtent(FVector(200.f, 200.f, 2*TargetHeightOffset));
    //TriggerBox->SetRelativeLocation(FVector(0.f, 0.f, TargetHeightOffset));
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/source/white_house_elevator.white_house_elevator'"));
    if (MeshAsset.Succeeded())
    {
        LiftMesh->SetStaticMesh(MeshAsset.Object);
        UE_LOG(LogTemp, Error, TEXT("Lift mesh loaded!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load lift mesh!"));
    }
}

// Called when the game starts or when spawned
void ALiftActor::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetActorLocation();

    UE_LOG(LogTemp, Warning, TEXT("Lift spawned at location: %s"), *StartLocation.ToString());
    TargetLocation = FVector(StartLocation.X, StartLocation.Y, StartLocation.Z + 2*TargetHeightOffset);
    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALiftActor::OnOverlapBegin);
        TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ALiftActor::OnOverlapEnd);
        DrawDebugBox(GetWorld(), TriggerBox->GetComponentLocation(), TriggerBox->GetScaledBoxExtent(), FColor::Red, true, 10.f);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("TriggerBox is null!"));
    }

    if (TargetLocation.IsZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("TargetLocation is zero! Set it in Editor."));
    }
}

// Called every frame
void ALiftActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsMoving)
    {
        FVector CurrentLocation = GetActorLocation();

        // Verifică dacă am ajuns la sau am trecut de TargetLocation
        if (CurrentLocation.Z >= TargetLocation.Z)
        {
            bIsMoving = false;
            SetActorLocation(TargetLocation); // asigurare exactă
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Lift reached target"));
            return;
        }

        FVector NewLocation = CurrentLocation + FVector(0.f, 0.f, LiftSpeed * DeltaTime);
        SetActorLocation(NewLocation);

        UE_LOG(LogTemp, Log, TEXT("Lift moving up: %s"), *NewLocation.ToString());
    }
}
void ALiftActor::StartLift()
{
    bIsMoving = true;
    bPendingActivation = false;

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Lift activated after delay"));
}

void ALiftActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        if (!bIsMoving && !bPendingActivation)
        {
            bPendingActivation = true;
            GetWorld()->GetTimerManager().SetTimer(LiftActivationTimerHandle, this, &ALiftActor::StartLift, 1.0f, false);
            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("Lift will activate in 1 second..."));
        }
    }
}
void ALiftActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this)
    {
        if (bPendingActivation)
        {
            GetWorld()->GetTimerManager().ClearTimer(LiftActivationTimerHandle);
            bPendingActivation = false;
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("Lift activation cancelled"));
        }

        bIsMoving = false;
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Lift stopped"));
    }
}
void ALiftActor::InitLift(float InHeightOffset)
{
    TargetHeightOffset = InHeightOffset;
    StartLocation = GetActorLocation();
    TargetLocation = FVector(StartLocation.X, StartLocation.Y, StartLocation.Z + TargetHeightOffset);

    if (TriggerBox)
    {
        TriggerBox->SetBoxExtent(FVector(200.f, 200.f, TargetHeightOffset));
        TriggerBox->SetRelativeLocation(FVector(0.f, 0.f, TargetHeightOffset));
        TriggerBox->SetUsingAbsoluteScale(true);
    }

    UE_LOG(LogTemp, Warning, TEXT("Lift initialized: TargetHeightOffset = %f, TargetLocation = %s"), TargetHeightOffset, *TargetLocation.ToString());
}


