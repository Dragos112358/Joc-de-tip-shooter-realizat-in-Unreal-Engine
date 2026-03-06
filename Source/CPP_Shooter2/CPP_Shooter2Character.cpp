// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPP_Shooter2Character.h"
#include "CPP_Shooter2Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "UObject/ConstructorHelpers.h"
#include "CPP_Shooter2HUD.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACPP_Shooter2Character

ACPP_Shooter2Character::ACPP_Shooter2Character()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextObj(TEXT("InputMappingContext'/Game/StarterContent/HDRI/Input/MPC_Default.MPC_Default'"));
	if (MappingContextObj.Succeeded())
	{
		DefaultMappingContext = MappingContextObj.Object.Get();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load DefaultMappingContext asset!"));
	}


}

void ACPP_Shooter2Character::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
				UE_LOG(LogTemp, Warning, TEXT("DefaultMappingContext added successfully"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is nullptr!"));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////// Input
void ACPP_Shooter2Character::OnFire()
{
	UE_LOG(LogTemp, Warning, TEXT("OnFire() called!"));

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetWorld() is nullptr!"));
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("No PlayerController found!"));
		return;
	}

	ACPP_Shooter2HUD* HUD = Cast<ACPP_Shooter2HUD>(PC->GetHUD());
	if (!HUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD is nullptr!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Inventory is open: %s"), HUD->IsInventoryOpen() ? TEXT("true") : TEXT("false"));

	if (HUD->IsInventoryOpen())
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is open. Cannot fire."));
		return;
	}
	if (CurrentAmmo > 0)
	{
		CurrentAmmo--;

		// Calculează procentul de muniție rămasă
		float AmmoRatio = static_cast<float>(CurrentAmmo) / 30.0f; // presupunem max ammo = 30

		// Interpolează culoarea de la roșu la verde
		// Verde (0,255,0), Roșu (255,0,0)
		FColor AmmoColor = FColor(
			static_cast<uint8>((1.0f - AmmoRatio) * 255), // Roșu crește pe măsură ce scade muniția
			static_cast<uint8>(AmmoRatio * 255),          // Verde scade pe măsură ce scade muniția
			0);

		//DrawDebugSphere(GetWorld(), GetActorLocation(), 30.f, 12, AmmoColor, false, 2.0f);

		UE_LOG(LogTemplateCharacter, Log, TEXT("Ammo left: %d"), CurrentAmmo);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("No ammo left!"));
		//DrawDebugSphere(GetWorld(), GetActorLocation(), 30.f, 12, FColor::Red, false, 2.0f);
	}
}


void ACPP_Shooter2Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Fire bullets
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACPP_Shooter2Character::OnFire);
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACPP_Shooter2Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACPP_Shooter2Character::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ACPP_Shooter2Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACPP_Shooter2Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACPP_Shooter2Character::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ACPP_Shooter2Character::GetHasRifle()
{
	return bHasRifle;
}