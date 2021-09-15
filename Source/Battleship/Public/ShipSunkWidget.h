// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ShipSunkWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESHIP_API UShipSunkWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	UTextBlock* ShipSunkText;
};
