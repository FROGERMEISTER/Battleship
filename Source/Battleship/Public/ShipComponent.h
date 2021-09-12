// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <utility>
#include "Components/StaticMeshComponent.h"
#include "ShipComponent.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESHIP_API UShipComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UShipComponent();
	bool IsBoardGridInShip(std::pair<int, int> Point);
	bool IsBoardGridInShipX(int PointX);
	bool IsBoardGridInShipY(int PointY);
	bool IsNewShipIntersectingShip(UShipComponent* Ship);
	void SetShipLocation(std::pair<int, int> ShipStart, std::pair<int, int> ShipEnd);
	std::pair<int, int> GetShipStart();
	std::pair<int, int> GetShipEnd();

private:
	int length;
	std::pair<std::pair<int, int>, std::pair<int, int>> ShipCoordinates;
};
