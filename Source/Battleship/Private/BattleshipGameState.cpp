// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleshipGameState.h"
#include "Board.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BattleShipPlayerController.h"
#include "BattleShipGameMode.h"
#include "ShipSunkWidget.h"



ABattleshipGameState::ABattleshipGameState()
{
	static ConstructorHelpers::FObjectFinder<UMaterial>HitMaterialAsset(TEXT("Material'/Game/HitMaterial.HitMaterial'"));
	HitMaterial = HitMaterialAsset.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>PinMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	PinMesh = PinMeshAsset.Object;
}

void ABattleshipGameState::HandleMatchHasStarted()
{
	for (auto& PlayerState : PlayerArray)
	{
		ABoard* PlayerBoard = PlayerState->GetOwner<ABattleShipPlayerController>()->GetPawn<ABoard>();
		for (auto& ShipKey : PlayerBoard->GetPlacedShipKeys())
		{
			((ABattleshipPlayerState*)PlayerState)->MulticastAddShip(ShipKey);
		}
		PlayerBoard->PlacementMode = false;
	}
	StartTurn(PlayerArray[0]->GetOwner<ABattleShipPlayerController>());
}

void ABattleshipGameState::HandleMatchWaitingForPlacement()
{
	
	for (auto& PlayerState : PlayerArray)
	{
		ABoard* PlayerBoard = PlayerState->GetOwner<ABattleShipPlayerController>()->GetPawn<ABoard>();
		PlayerBoard->PlacementMode = true;
	}
}

void ABattleshipGameState::HandleMatchEnd()
{
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("On Server"));
	
	for (auto& PlayerState : PlayerArray)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unpossess"));
		UE_LOG(LogTemp, Warning, TEXT("Name %s"), *PlayerState->GetName());
		UE_LOG(LogTemp, Warning, TEXT("PCName %s"), *PlayerState->GetOwner<AController>()->GetName());
		PlayerState->GetOwner<AController>()->UnPossess();
	}
	}
}

bool ABattleshipGameState::ArePlayersReady()
{
	for (auto& PlayerState : PlayerArray)
	{
		ABoard* PlayerBoard = PlayerState->GetOwner<ABattleShipPlayerController>()->GetPawn<ABoard>();
		UE_LOG(LogTemp, Warning, TEXT("Player Ships %s Player Ready"), (PlayerBoard->AreAllShipsPlaced() ? TEXT("true") : TEXT("false")), (PlayerBoard->PlayerReady ? TEXT("true") : TEXT("false")));
		if (!PlayerBoard->AreAllShipsPlaced() || !PlayerBoard->PlayerReady)
			return false;
	}
	return true;
}

void ABattleshipGameState::ShootAtOpponent(AController* ShootingPlayer, std::pair<int, int> GridToShootAt)
{
	for (auto& PlayerState : PlayerArray)
	{
		if (PlayerState->GetOwner<AController>() != ShootingPlayer && bPlayerHasShot == false)
		{
			for (auto& Move : ShootingPlayer->GetPlayerState<ABattleshipPlayerState>()->Moves)
			{
				if (GridToShootAt == Move)
					return;
			}
			ShootingPlayer->GetPlayerState<ABattleshipPlayerState>()->Moves.Add(GridToShootAt);
			bPlayerHasShot = true;
			UE_LOG(LogTemp, Warning, TEXT("PlayerController %s"), (PlayerState->GetOwner() != nullptr ? TEXT("true") : TEXT("false")));
			if (PlayerState->GetOwner<ABattleShipPlayerController>() == nullptr || PlayerState->GetOwner<ABattleShipPlayerController>()->GetPawn<ABoard>() == nullptr)
				return;
			ABoard* PlayerBoard = PlayerState->GetOwner<ABattleShipPlayerController>()->GetPawn<ABoard>();
			UE_LOG(LogTemp, Warning, TEXT("shoot ship grid %d %d"), GridToShootAt.first, GridToShootAt.first);
			UShipComponent* ShipAtGrid = PlayerBoard->ShipAtGridLocation(GridToShootAt);
			bool IsHit = false;
			if (ShipAtGrid != nullptr)
				IsHit = ShipAtGrid->ShootAtShip(GridToShootAt);
			MulticastPlacePins(GridToShootAt.first, GridToShootAt.second, IsHit);
			if (HasPlayerLost((ABattleshipPlayerState*)PlayerState))
			{
				SetMatchState(MatchState::WaitingPostMatch);
				return;
			}
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ABattleshipGameState::EndTurn, ShootingPlayer);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 3, false);
			
		}
	}
}

void ABattleshipGameState::SetMatchState(FName NewState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogGameState, Log, TEXT("Match State Changed from %s to %s"), *MatchState.ToString(), *NewState.ToString());

		MatchState = NewState;

		// Call the onrep to make sure the callbacks happen
		ServerSetMatchState(MatchState);
	}
}

void ABattleshipGameState::ServerSetMatchState_Implementation(const FName& NewMatchState)
{
	if (NewMatchState == MatchState::WaitingToStart || PreviousMatchState == MatchState::EnteringMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaitingForReady"));
		// Call MatchIsWaiting to start even if you join in progress at a later state
		HandleMatchWaitingForPlacement();
	}

	if (NewMatchState == MatchState::InProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartingGame"));
		HandleMatchHasStarted();
	}

	if (NewMatchState == MatchState::WaitingPostMatch)
	{
		UE_LOG(LogTemp, Warning, TEXT("EndingGame"));
		HandleMatchEnd();
	}

	PreviousMatchState = NewMatchState;
}

void ABattleshipGameState::MulticastPlacePins_Implementation(int GridX, int GridY, bool IsHit)
{
	if (GetNetMode() == ENetMode::NM_Client)
	{
		ABattleShipPlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController<ABattleShipPlayerController>();
		if (LocalPlayerController != nullptr)
		{
			ABoard* PlayerBoard = LocalPlayerController->GetPawn<ABoard>();
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AStaticMeshActor* PlacedPin = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FTransform(FRotator(), FVector(PlayerBoard->BoardGridToWorldLocation(std::pair<int, int>(GridX, GridY))), FVector(0.5f, 0.5f, 2.0f)), SpawnInfo);
			PlacedPin->SetMobility(EComponentMobility::Stationary);
			PlacedPin->GetStaticMeshComponent()->SetStaticMesh(PinMesh);
			if (IsHit && PlacedPin != nullptr)
				PlacedPin->GetStaticMeshComponent()->SetMaterial(0, HitMaterial);
		}
	}
}

void ABattleshipGameState::DisplayShipSunkMessageWithShipKey(FString ShipKey)
{
	TSubclassOf<class UUserWidget> WidgetClass = GetWorld()->GetAuthGameMode<ABattleshipGameMode>()->ShipSunkWidget;
	for (auto& PlayerState : PlayerArray)
	{
		UShipSunkWidget* ShipSunkWidget = CreateWidget<UShipSunkWidget>(PlayerState->GetGameInstance(), WidgetClass);
		ShipSunkWidget->AddToViewport();
	}
}

void ABattleshipGameState::StartTurn(AController* Player)
{
	Player->Possess(((ABattleShipPlayerController*)Player)->OpponentsBoard);
	CurrentPlayer = Player;
	bPlayerHasShot = false;
}

void ABattleshipGameState::EndTurn(AController* Player)
{
	for (auto& PlayerState : PlayerArray)
	{
		if (PlayerState->GetOwner<ABattleShipPlayerController>() != Player)
		{
			Player->Possess(((ABattleShipPlayerController*)Player)->OwnBoard);
			StartTurn(PlayerState->GetOwner<ABattleShipPlayerController>());
		}
	}
	
}

bool ABattleshipGameState::HasPlayerLost(ABattleshipPlayerState* Player)
{
	if (Player->Ships.FindKey(true) == nullptr)
	{
		return true;
	}
	return false;
}