// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_PickUpComponent.h"
#include "CPP_Shooter2Character.h"
#include "CPP_Shooter2HUD.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

UTP_PickUpComponent::UTP_PickUpComponent()
{
	SphereRadius = 32.f;

	// Încarcă textura O SINGURĂ DATĂ la construcție
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconObj(TEXT("Texture2D'/Game/StarterContent/Textures/weapon.weapon'"));
	if (IconObj.Succeeded())
	{
		WeaponIcon = IconObj.Object;
	}
	else
	{
		WeaponIcon = nullptr;
	}
}

void UTP_PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UTP_PickUpComponent::OnSphereBeginOverlap);
}
void UTP_PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACPP_Shooter2Character* Character = Cast<ACPP_Shooter2Character>(OtherActor);
	if (Character)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		if (PlayerController)
		{
			ACPP_Shooter2HUD* HUD = Cast<ACPP_Shooter2HUD>(PlayerController->GetHUD());
			if (HUD)
			{
				bool bPickedUp = false; // declar flagul

				for (int32 SlotIndex = 1; SlotIndex <= 2; ++SlotIndex)
				{
					FString CurrentWeapon = HUD->GetWeaponInSlot(SlotIndex);
					if (CurrentWeapon.IsEmpty() || CurrentWeapon == TEXT("Empty"))
					{
						HUD->SetWeaponInSlot(SlotIndex, TEXT("Rifle"), WeaponIcon);
						//bPickedUp = true;
						break; // am gasit slot liber, ies din loop
					}
				}

				// Daca am luat arma, distrug pickup-ul
				if (bPickedUp)
				{
					GetOwner()->Destroy();
				}
			}
		}

		OnPickUp.Broadcast(Character);
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
