// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattleShipPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESHIP_API ABattleShipPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABattleShipPlayerController();

public:
	// Called every frame
	virtual void PlayerTick(float DeltaTime) override;

	FVector MouseTraceLocation;
};
