// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <utility>
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleShipPlayerState.h"
#include "BattleshipGameState.generated.h"


namespace MatchState
{
	const FName EnteringMap = FName(TEXT("EnteringMap"));
	const FName WaitingToStart = FName(TEXT("WaitingToStart"));
	const FName WaitingForPlacement = FName(TEXT("WaitingForPlacement"));
	const FName InProgress = FName(TEXT("InProgress"));
	const FName WaitingPostMatch = FName(TEXT("WaitingPostMatch"));
	const FName LeavingMap = FName(TEXT("LeavingMap"));
	const FName Aborted = FName(TEXT("Aborted"));
}

/**
 * 
 */
UCLASS()
class BATTLESHIP_API ABattleshipGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ABattleshipGameState();

	bool ArePlayersReady();
	void ShootAtOpponent(AController* ShootingPlayer, std::pair<int, int> GridToShootAt);
	void SetMatchState(FName MatchState);
	bool HasPlayerLost(ABattleshipPlayerState* Player);
	void DisplayShipSunkMessageWithShipKey(FString ShipKey);

protected:
	FName MatchState;

	FName PreviousMatchState;
	void HandleMatchHasStarted();
	void HandleMatchWaitingForPlacement();
	void HandleMatchEnd();
	void StartTurn(AController* Player);
	void EndTurn(AController* Player);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlacePins(int GridX, int GridY, bool IsHit);
	void MulticastPlacePins_Implementation(int GridX, int GridY, bool IsHit);

	UFUNCTION(Server, Reliable)
	void ServerSetMatchState(const FName& NewMatchState);
	void ServerSetMatchState_Implementation(const FName& NewMatchState);

	bool bPlayerHasShot = false;
	AController* CurrentPlayer;
	UMaterial* HitMaterial;
	UStaticMesh* PinMesh;
};
