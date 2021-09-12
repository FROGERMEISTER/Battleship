// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipComponent.h"

UShipComponent::UShipComponent()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Battleship.Battleship'"));
	UStaticMesh* Asset = MeshAsset.Object;

	
	this->SetStaticMesh(Asset);
}

bool UShipComponent::IsBoardGridInShip(std::pair<int, int> point)
{
	if (IsBoardGridInShipX(point.first) && IsBoardGridInShipY(point.second))
	{
		return true;
	}
	return false;
}

bool UShipComponent::IsBoardGridInShipX(int pointX)
{
	if ((pointX <= ShipCoordinates.first.first && pointX >= ShipCoordinates.second.first) || (pointX >= ShipCoordinates.first.first && pointX <= ShipCoordinates.second.first))
	{
		return true;
	}
	return false;
}

bool UShipComponent::IsBoardGridInShipY(int pointY)
{
	if ((pointY <= ShipCoordinates.first.second && pointY >= ShipCoordinates.second.second) || (pointY >= ShipCoordinates.first.second && pointY <= ShipCoordinates.second.second))
	{
		return true;
	}
	return false;
}

bool UShipComponent::IsLineIntersectingShip(UShipComponent* Ship)
{
	UE_LOG(LogTemp, Warning, TEXT("test place X1=%d Y1=%d to X2=%d Y2=%d"), Ship->GetShipStart().first, Ship->GetShipStart().second, Ship->GetShipEnd().first, Ship->GetShipEnd().second);
	UE_LOG(LogTemp, Warning, TEXT("test ship X1=%d Y1=%d to X2=%d Y2=%d"), ShipCoordinates.first.first, ShipCoordinates.first.second, ShipCoordinates.second.first, ShipCoordinates.second.second);
	if (this->IsBoardGridInShipX(Ship->GetShipStart().first) && Ship->IsBoardGridInShipY(ShipCoordinates.first.second))
	{
		UE_LOG(LogTemp, Warning, TEXT("first second"));
		return true;
	}
	if (IsBoardGridInShipY(Ship->GetShipStart().second) && Ship->IsBoardGridInShipX(ShipCoordinates.first.first))
	{
		UE_LOG(LogTemp, Warning, TEXT("second"));
		return true;
	}
	return false;
}

void UShipComponent::SetShipLocation(std::pair<int, int> ShipStart, std::pair<int, int> ShipEnd)
{
	ShipCoordinates.first = ShipStart;
	ShipCoordinates.second = ShipEnd;
}

std::pair<int, int> UShipComponent::GetShipStart()
{
	return ShipCoordinates.first;
}

std::pair<int, int> UShipComponent::GetShipEnd()
{
	return ShipCoordinates.second;
}