// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    AKM,
    Scar,
    VSS,
    M416,
    M762,
    M249,
    AWM,
    SKS,
    Groza,
    AUGA3,
    KAR98K,
    DP28,
    S686,
    None
};

UCLASS()
class CPP_SHOOTER2_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // Proprietăți armă
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Info")
    FString WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Info")
    UStaticMeshComponent* WeaponMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    int32 AmmoInMagazine;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    int32 AmmoInInventory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
    EBulletType BulletType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
    float FireRate; // secunde între focuri

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
    float Damage;
    UPROPERTY(VisibleAnywhere)
    class USphereComponent* PickupSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|UI")
    UTexture2D* WeaponIcon;

    bool bCanBePickedUp;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsPickupEnabled = true; // true = arma poate fi luată; false = doar vizual

    UFUNCTION()
    void OnPickupSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPickupSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundBase* PickupWeaponSound2;

public:
    void InitializeWeapon(EWeaponType WeaponType);
private:
    static UStaticMesh* AKM_Mesh;
    static UStaticMesh* Scar_Mesh;
    static UStaticMesh* VSS_Mesh; 
    static UStaticMesh* M416_Mesh;
    static UStaticMesh* M762_Mesh;
    static UStaticMesh* M249_Mesh;
    static UStaticMesh* AWM_Mesh;
    static UStaticMesh* SKS_Mesh;
    static UStaticMesh* Groza_Mesh;
    static UStaticMesh* AUGA3_Mesh;
    static UStaticMesh* KAR98K_Mesh;
    static UStaticMesh* S686_Mesh;
    static UStaticMesh* DP28_Mesh;
};
