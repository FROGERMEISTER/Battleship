// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleshipGameMode.h"
#include "BattleshipGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Board.h"
#include "OpponentsBoard.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

ABattleshipGameMode::ABattleshipGameMode()
{
	
}

void ABattleshipGameMode::RestartPlayer(AController* NewPlayer)
{
	RestartPlayerAtTransform(NewPlayer, FTransform(FVector(100.0f, 0.0f, 0.0f)));
}

void ABattleshipGameMode::RestartPlayerAtTransform(AController* NewPlayer, const FTransform& SpawnTransform)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	UE_LOG(LogGameMode, Verbose, TEXT("RestartPlayerAtTransform %s"), (NewPlayer && NewPlayer->PlayerState) ? *NewPlayer->PlayerState->GetPlayerName() : TEXT("Unknown"));

	if (MustSpectate(Cast<APlayerController>(NewPlayer)))
	{
		UE_LOG(LogGameMode, Verbose, TEXT("RestartPlayerAtTransform: Tried to restart a spectator-only player!"));
		return;
	}

	FRotator SpawnRotation = SpawnTransform.GetRotation().Rotator();

	if (NewPlayer->GetPawn() != nullptr)
	{
		// If we have an existing pawn, just use it's rotation
		SpawnRotation = NewPlayer->GetPawn()->GetActorRotation();
	}
	else if (GetDefaultPawnClassForController(NewPlayer) != nullptr)
	{
		// Try to create a pawn to use of the default class for this player
		NewPlayer->SetPawn(SpawnDefaultPawnAtTransform(NewPlayer, SpawnTransform));
		Cast<ABattleShipPlayerController>(NewPlayer)->OpponentsBoard = SpawnOpponentBoardPawnAtTransform(NewPlayer, FTransform(FVector(0.0f, 1500.0f, 0.0f)));
	}

	if (NewPlayer->GetPawn() == nullptr)
	{
		NewPlayer->FailedToSpawnPawn();
	}
	else
	{
		FinishRestartPlayer(NewPlayer, SpawnRotation);
	}
}

APawn* ABattleshipGameMode::SpawnDefaultPawnAtTransform(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo);
	if (!ResultPawn)
	{
		UE_LOG(LogGameMode, Warning, TEXT("SpawnDefaultPawnAtTransform: Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
	}
	Cast<ABattleShipPlayerController>(NewPlayer)->OwnBoard = ResultPawn;
	return ResultPawn;
}

APawn* ABattleshipGameMode::SpawnOpponentBoardPawnAtTransform(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(AOpponentsBoard::StaticClass(), SpawnTransform, SpawnInfo);
	if (!ResultPawn)
	{
		UE_LOG(LogGameMode, Warning, TEXT("SpawnDefaultPawnAtTransform: Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(AOpponentsBoard::StaticClass()), *SpawnTransform.ToHumanReadableString());
	}
	return ResultPawn;
}

void ABattleshipGameMode::TryToStartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("TryToStartGame"));
	if (((ABattleshipGameState*)GameState)->ArePlayersReady())
		((ABattleshipGameState*)GameState)->SetMatchState(MatchState::InProgress);
}