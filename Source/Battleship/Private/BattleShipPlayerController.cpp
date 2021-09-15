// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleShipPlayerController.h"
#include "Board.h"
#include "BattleshipGameMode.h"
#include "ShipComponent.h"
#include "Kismet/GameplayStatics.h"
#define ISDEDICATED (GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer)
#define ISLISTEN (GEngine->GetNetMode(GetWorld()) == NM_ListenServer)
#define ISSTANDALONE (GEngine->GetNetMode(GetWorld()) == NM_Standalone)
#define ISCLIENT (GEngine->GetNetMode(GetWorld()) == NM_Client)

ABattleShipPlayerController::ABattleShipPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	
}

void ABattleShipPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ABattleShipPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (aPawn == OwnBoard)
	{
		OwnBoard->ReregisterAllComponents();
		for (auto& Component : ((ABoard*)aPawn)->GetComponentsByClass(UShipComponent::StaticClass()))
		{
			OwnBoard->FinishAndRegisterComponent(Component);
			UE_LOG(LogTemp, Warning, TEXT("ShipComponent %s"), *Component->GetName());
			
		}
		
	}
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
		if (Hit.Time < 1)
		{
			MouseTraceLocation = Hit.ImpactPoint;
			std::pair<int, int> NewBoardGrid = PlayerBoard->WorldLocationToBoardGrid(MouseTraceLocation);
			if (NewBoardGrid != SelectedBoardGrid)
			{
				SelectedBoardGrid = NewBoardGrid;
				PlayerBoard->UpdateBoardSelectorLocationToGrid(NewBoardGrid);
			}
		}
	}
}