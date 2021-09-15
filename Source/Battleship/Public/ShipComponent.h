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
	bool ShootAtShip(std::pair<int, int> BoardGrid);

	void SetShipLocation(std::pair<int, int> ShipStart, std::pair<int, int> ShipEnd);
	void DestroyShip();
	
	int GetShipLength();
	std::pair<int, int> GetShipStart();
	std::pair<int, int> GetShipEnd();

	UFUNCTION(Server, Reliable)
	void HandleSetShipLocation(int ShipStartX, int ShipStartY, int ShipEndX, int ShipEndY);
	void HandleSetShipLocation_Implementation(int ShipStartX, int ShipStartY, int ShipEndX, int ShipEndY);

	FString ShipKey;

private:
	int length;
	TArray<bool> DestroyedParts;
	bool IsDestroyed;
	UMaterial* DestroyedMaterial;
	std::pair<std::pair<int, int>, std::pair<int, int>> ShipCoordinates;
	bool bReplicates = true;

	UFUNCTION(Client, Reliable)
	void ClientDestroyShip();
	void ClientDestroyShip_Implementation();
};
