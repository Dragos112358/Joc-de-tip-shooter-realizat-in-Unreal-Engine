// Copyright Epic Games, Inc. All Rights Reserved.


#include "CPP_Shooter2PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CPP_Shooter2HUD.h"

void ACPP_Shooter2PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (LeftClickAction)
		{
			EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Triggered, this, &ACPP_Shooter2PlayerController::OnLeftClick);
		}
	}
}
bool ACPP_Shooter2PlayerController::IsPointInRect(const FVector2D& Point, const FVector2D& RectTopLeft, const FVector2D& RectSize)
{
	return Point.X >= RectTopLeft.X && Point.X <= RectTopLeft.X + RectSize.X &&
		Point.Y >= RectTopLeft.Y && Point.Y <= RectTopLeft.Y + RectSize.Y;
}
void ACPP_Shooter2PlayerController::OnLeftClick()
{
	UE_LOG(LogTemp, Log, TEXT("Am dat click stanga"));
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);

	if (ACPP_Shooter2HUD* MyHUD2 = Cast<ACPP_Shooter2HUD>(GetHUD()))
	{
		if (IsPointInRect(FVector2D(MouseX, MouseY), MyHUD2->CloseButtonPos, MyHUD2->CloseButtonSize))
		{
			MyHUD2->ToggleInventory();
		}
	}
}


void ACPP_Shooter2PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		if (InputMappingContext)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("InputMappingContext este nullptr in PlayerController!"));
		}

		UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));
	}
}