// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ship.h"
#include <utility>
#include "GameFramework/PlayerState.h"
#include "BattleshipPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESHIP_API ABattleshipPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	TMap<FString, bool> Ships;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddShip(const FString& Key);
	void MulticastAddShip_Implementation(const FString& Key);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroyShip(const FString& Key);
	void MulticastDestroyShip_Implementation(const FString& Key);

	TArray<std::pair<int, int>> Moves;
};
