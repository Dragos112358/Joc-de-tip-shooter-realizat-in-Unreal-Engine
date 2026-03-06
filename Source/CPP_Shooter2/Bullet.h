#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Bullet.generated.h"

UENUM(BlueprintType)
enum class EBulletType : uint8
{
    BT_556 UMETA(DisplayName = "5.56mm"),
    BT_762 UMETA(DisplayName = "7.62mm"),
    BT_9MM UMETA(DisplayName = "9mm"),
    BT_None UMETA(DisplayName = "None")
};

// Delegate declarat corect în afara clasei
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletNearby, FString, Message);

UCLASS()
class CPP_SHOOTER2_API ABullet : public AActor
{
    GENERATED_BODY()
    
public:
    ABullet();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* BodyMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* LidMesh;

    // BulletType acum este property pentru editare în editor și blueprint
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
    EBulletType BulletType;

    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CollisionBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    class USphereComponent* PickupSphere;

    bool bCanBePickedUp;

    FString GetBulletTypeName() const;

    // Delegate pe care alte clase îl pot asculta
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnBulletNearby OnBulletNearby;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnPickupSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPickupSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // Mesh-uri (poți să le faci UPROPERTY dacă vrei să le setezi din editor)
    UStaticMesh* Mesh_556;
    UStaticMesh* Mesh_762;
    UStaticMesh* Capac_762;
    UStaticMesh* Mesh_9MM;
    UStaticMesh* LidMesh_556;
};
