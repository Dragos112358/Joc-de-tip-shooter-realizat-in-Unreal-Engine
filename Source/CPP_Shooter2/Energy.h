// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Energy.generated.h"

UENUM(BlueprintType)
enum class EEnergyType : uint8
{
	E_EnergyDrink		UMETA(DisplayName = "Energy Drink"),
	E_Painkiller		UMETA(DisplayName = "Painkiller"),
	E_AdrenalineSyringe UMETA(DisplayName = "Adrenaline Syringe")
};

UCLASS()
class CPP_SHOOTER2_API AEnergy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnergy();
	bool bCanBePickedUp = false;
	void InitEnergy(EEnergyType NewType);

	UFUNCTION()
	void OnPickupSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPickupSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// 🔁 Tipul de energie
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy Type")
	EEnergyType EnergyType;

protected:
	// 🔳 Coliziune principală
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* CollisionBox;

	// 🟡 Sferă pentru detectare pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* PickupSphere;

	// 🧊 Mesh principal (bottle, syringe etc)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* BodyMesh;

	// 🔹 Capul obiectului (opțional)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* CapMesh;

	// 🔧 Mesh-uri disponibile
	UPROPERTY()
	class UStaticMesh* Mesh_Drink;

	UPROPERTY()
	class UStaticMesh* Mesh_Painkiller;

	UPROPERTY()
	class UStaticMesh* Mesh_Adrenaline;

};
