// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LiftActor.generated.h"

UCLASS()
class CPP_SHOOTER2_API ALiftActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALiftActor();
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* LiftMesh;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Lift")
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, Category = "Lift")
	float LiftSpeed = 1000.f;
	FTimerHandle LiftActivationTimerHandle;
	bool bPendingActivation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lift")
	float TargetHeightOffset = 2000.0f; // Poți seta din blueprint sau cod extern

	UFUNCTION()
	void StartLift(); // va fi apelată după o secundă
	FVector StartLocation;
	bool bIsMoving = false;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void InitLift(float InHeightOffset);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
