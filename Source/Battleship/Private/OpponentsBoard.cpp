// Fill out your copyright notice in the Description page of Project Settings.


#include "OpponentsBoard.h"
#include "BattleshipGameState.h"
#include "BattleShipPlayerController.h"


AOpponentsBoard::AOpponentsBoard()
{
	
}

void AOpponentsBoard::BeginPlay()
{
	SelectorMeshComponent->SetStaticMesh(SelectorMesh);
}

void AOpponentsBoard::UpdateBoardSelectorLocationToGrid(std::pair<int, int> BoardGrid)
{
	SelectorMeshComponent->SetWorldLocation(BoardGridToWorldLocation(BoardGrid));
	SelectorMeshComponent->AddRelativeLocation(FVector(0.0f, 0.0f, 5.0f));
}

void AOpponentsBoard::OnClick()
{
	HandleShoot(GetController<ABattleShipPlayerController>()->MouseTraceLocation);
}

void AOpponentsBoard::HandleShoot_Implementation(FVector BoardLocation)
{
	ABattleshipGameState* GameState = GetWorld() != NULL ? GetWorld()->GetGameState<ABattleshipGameState>() : NULL;
	if (GameState != nullptr)
		GameState->ShootAtOpponent(GetController(), WorldLocationToBoardGrid(BoardLocation));
	
}