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
	void DestroyShip();
	bool ShootAtShip(std::pair<int, int> BoardGrid);
	std::pair<int, int> GetShipStart();
	std::pair<int, int> GetShipEnd();

private:
	int length;
	TArray<bool> DestroyedParts;
	bool IsDestroyed;
	UMaterial* DestroyedMaterial;
	std::pair<std::pair<int, int>, std::pair<int, int>> ShipCoordinates;
};
