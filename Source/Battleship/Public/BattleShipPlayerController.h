// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <utility>
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void PlayerTick(float DeltaTime) override;

	FVector MouseTraceLocation;
	std::pair<int, int> SelectedBoardGrid;
	APawn* OwnBoard;
	APawn* OpponentsBoard;
};
