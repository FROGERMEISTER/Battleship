// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleShipPlayerController.h"
#include "Board.h"

ABattleShipPlayerController::ABattleShipPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	
}

void ABattleShipPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	ABoard* PlayerBoard = Cast<ABoard>(GetPawn());
	if (PlayerBoard != nullptr)
	{
		FVector WorldLocation;
		FVector WorldDirection;
		DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
		FHitResult Hit;

		FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true);
		RV_TraceParams.bTraceComplex = true;
		RV_TraceParams.bReturnPhysicalMaterial = false;

		PlayerBoard->ActorLineTraceSingle(Hit, WorldLocation, WorldLocation + (WorldDirection * 10000), ECC_Visibility, RV_TraceParams);
		MouseTraceLocation = Hit.ImpactPoint;
		PlayerBoard->UpdateBoardSelectorLocationToGrid(PlayerBoard->WorldLocationToBoardGrid(MouseTraceLocation));
	}
}