// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleshipPlayerState.h"
#include "BattleshipGameState.h"
#include "ShipSunkWidget.h"

void ABattleshipPlayerState::MulticastAddShip_Implementation(const FString& Key)
{
	Ships.Add(Key, true);
}

void ABattleshipPlayerState::MulticastDestroyShip_Implementation(const FString& Key)
{
	UE_LOG(LogTemp, Warning, TEXT("Ship Sunk %s"), *Key);
	Ships.Emplace(Key, false);
}