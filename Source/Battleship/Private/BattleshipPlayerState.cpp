// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleshipPlayerState.h"

void ABattleshipPlayerState::MulticastAddShip_Implementation(const FString& Key)
{
	Ships.Add(Key, true);
}

void ABattleshipPlayerState::MulticastDestroyShip_Implementation(const FString& Key)
{
	Ships.Emplace(Key, false);
}