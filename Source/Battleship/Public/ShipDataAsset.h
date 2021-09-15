// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ShipDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESHIP_API UShipDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	int ShipSize;
	FString ShipName;
};
