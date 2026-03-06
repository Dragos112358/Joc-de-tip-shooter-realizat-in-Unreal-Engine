#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "Camera/CameraComponent.h"
#include "Car.generated.h"


class USphereComponent;

UCLASS()
class CPP_SHOOTER2_API ACar : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	ACar();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* DriverSeat;
	float greutate = 1120; //kg
	float putere = 120 * 736; //wati
	float velocity;
	double k = 0.713949191;           //0.713949191;
	double u = 0.02;

	float a, b, c, raport;
	float g = 9.81;
	float nrtrepte = 5;
	float max = 8000;
	float puteremed = 0;
	float turatie = 0;
	float vant = 0;
	float panta = 0;
	float x = 10;
	float acceleratie = 0;
	float viteza = 0;
	float treapta = 1;
	float ToggleCooldown = 0.5;
	float TimeSinceLastToggle = 0.f;
	FTimerHandle ExitTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Physics")
	float MinBumpThreshold = 20.0f; // Pragul minim pentru bumpuri (în unități Unreal)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Physics")
	float GroundStabilizationForce = 50000.0f; // Forța de stabilizare

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Physics")
	float MaxGroundDistance = 200.0f; // Distanța maximă de detectare a solului

	FVector LastGroundPosition; // Ultima poziție cunoscută a solului
	bool bHasValidGroundPosition = false;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractionZone;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CarCamera;

	bool bPlayerInside;
	APawn* OriginalPlayer;
	APlayerController* PlayerController;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CarMesh;
	float ForwardInput;
	float TurnInput;
	class APawn* OriginalPawn;

};
