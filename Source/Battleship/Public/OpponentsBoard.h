// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Board.h"
#include "OpponentsBoard.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESHIP_API AOpponentsBoard : public ABoard
{
	GENERATED_BODY()

public:
	AOpponentsBoard();
	bool PlacementMode = true;
	bool CanShoot = false;
	void UpdateBoardSelectorLocationToGrid(std::pair<int, int> BoardGrid) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void HandleShoot(FVector BoardLocation);
	void HandleShoot_Implementation(FVector BoardLocation);

private:
	void OnClick() override;
};
