#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Car.h"
#include "DragRace.generated.h"

UCLASS()
class CPP_SHOOTER2_API ADragRace : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ADragRace();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    void CheckRaceFinished();
    void ResetRace();
    void InitCars();
    //void UpdateHUD();
    // Zona de start
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race")
    UBoxComponent* StartTrigger;

    // Poziția liniei de start
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
    FVector StartLine;

    // Lista mașinilor (jucător + AI)
    UPROPERTY(EditAnywhere, Category = "DragRace")
    TSubclassOf<ACar> Cars;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
    TArray<AActor*> RaceCars;

    // Flag pentru startul cursei
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Race")
    bool bRaceStarted;

    // Flag pentru player în zona de start
    bool bPlayerInZone;

    // Timp de start (countdown)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
    float Countdown;

    // Funcție pentru start cursă
    UFUNCTION(BlueprintCallable, Category = "Race")
    void StartRace();

    UPROPERTY()
    UStaticMesh* CarMeshAsset;

    //pentru masini
    TArray<float> AI_Viteza;    // viteza fiecărei mașini
    TArray<float> AI_Turatie;   // turatia motorului
    TArray<int> AI_Treapta;     // treapta curentă
    TArray<float> AI_Acceleratie;
    TArray<float> AI_putere;
    TArray<float> AI_raport;
    float greutate = 1120; //kg
    //float putere = 120 * 736; //wati
    float velocity;
    double k = 0.713949191;           //0.713949191;
    double u = 0.02;

    float a, b, c;
    float g = 9.81;
    float nrtrepte = 5;
    float max = 8000;
    //float puteremed = 0;
    //float turatie = 0;
    float vant = 0;
    float panta = 0;
    float x = 10;
    //float acceleratie = 0;
    //float viteza = 0;
    //float treapta = 1;

    //cursa efectiva
    FVector StartLineLocation;
    float RaceDistance = 91000.f; // 910 metri în cm
    float RaceStartTime;

    // Timpurile fiecărei mașini
    TArray<float> FinishTimes;
    TArray<bool> bHasFinished;

    //playerul meu
    bool bPlayerFinished;
    float PlayerFinishTime;

private:
    // Funcție pentru mișcarea AI-ului
    void MoveAICars(float DeltaTime);

    // Overlap pentru intrarea jucătorului în start zone
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    // Overlap pentru ieșirea jucătorului din start zone
    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
