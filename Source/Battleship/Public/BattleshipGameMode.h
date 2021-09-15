// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Board.h"
#include "BattleShipPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "BattleshipGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESHIP_API ABattleshipGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleshipGameMode();
	
	void RestartPlayer(AController* NewPlayer) override;
	void RestartPlayerAtTransform(AController* NewPlayer, const FTransform& SpawnTransform);
	APawn* SpawnDefaultPawnAtTransform(AController* NewPlayer, const FTransform& SpawnTransform);
	APawn* SpawnOpponentBoardPawnAtTransform(AController* NewPlayer, const FTransform& SpawnTransform);
	void TryToStartGame();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> ShipSunkWidget;

protected:
	void StartPlacementPhase();
	void StartShootingPhase();

	int PlayerTurn;
};
