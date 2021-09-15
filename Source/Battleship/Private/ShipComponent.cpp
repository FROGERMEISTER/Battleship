// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipComponent.h"
#include "Board.h"

UShipComponent::UShipComponent()
{
	static ConstructorHelpers::FObjectFinder<UMaterial>DestroyedMaterialAsset(TEXT("Material'/Game/DestroyedMaterial.DestroyedMaterial'"));
	DestroyedMaterial = DestroyedMaterialAsset.Object;
	SetIsReplicatedByDefault(true);
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

bool UShipComponent::IsNewShipIntersectingShip(UShipComponent* Ship)
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

void UShipComponent::HandleSetShipLocation_Implementation(int ShipStartX, int ShipStartY, int ShipEndX, int ShipEndY)
{
	SetShipLocation(std::pair<int, int>(ShipStartX, ShipStartY), std::pair<int, int>(ShipEndX, ShipEndY));
	DestroyedParts.Init(false, GetShipLength());
}

void UShipComponent::DestroyShip()
{
	if (GetOwner<ABoard>() == nullptr)
		return;
	GetOwner<ABoard>()->DestroyShipWithKey(ShipKey);
	SetMaterial(0, DestroyedMaterial);
}

void UShipComponent::ClientDestroyShip_Implementation()
{
	DestroyShip();
}

bool UShipComponent::ShootAtShip(std::pair<int, int> BoardGrid)
{
	if (IsBoardGridInShip(BoardGrid))
	{
		int HitPart = FMath::Max(abs(BoardGrid.first - ShipCoordinates.first.first), abs(BoardGrid.second - ShipCoordinates.first.second));
		UE_LOG(LogTemp, Warning, TEXT("Hit part: %d of %d"), HitPart, DestroyedParts.Num());
		DestroyedParts[HitPart] = true;
		if (!DestroyedParts.Contains(false))
		{
			DestroyShip();
			ClientDestroyShip();
		}
		return true;
	}
	return false;
}

std::pair<int, int> UShipComponent::GetShipStart()
{
	return ShipCoordinates.first;
}

std::pair<int, int> UShipComponent::GetShipEnd()
{
	return ShipCoordinates.second;
}

int UShipComponent::GetShipLength()
{
	return FMath::Max(abs(ShipCoordinates.first.first - ShipCoordinates.second.first), abs(ShipCoordinates.first.second - ShipCoordinates.second.second)) + 1;
}