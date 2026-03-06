// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "InputAction.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPP_Shooter2PlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class CPP_SHOOTER2_API ACPP_Shooter2PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	// Begin Actor interface
	bool IsPointInRect(const FVector2D& Point, const FVector2D& RectTopLeft, const FVector2D& RectSize);
	void OnLeftClick();
protected:
	virtual void BeginPlay() override;
	void SetupInputComponent();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LeftClickAction;


	// End Actor interface
};
