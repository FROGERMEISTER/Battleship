// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <utility>
#include "CoreMinimal.h"
#include "ShipComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Pawn.h"
#include "Board.generated.h"

UCLASS()
class BATTLESHIP_API ABoard : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABoard();

	std::pair<int, int> WorldLocationToBoardGrid(FVector WorldLocation);
	FVector BoardGridToWorldLocation(std::pair<int, int> BoardGrid);
	void UpdateBoardSelectorLocationToGrid(std::pair<int, int> BoardGrid);
	UShipComponent* ShipAtGridLocation(std::pair<int, int> BoardGrid);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	class UCameraComponent* CameraComponent;

	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BoardMesh;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SelectorMeshComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	std::pair<int, int> BoardSize = std::pair<int, int>(10, 10);
	TArray<UShipComponent*> ShipsLeft;
	class UShipComponent* CurrentShip;

	UStaticMesh* ShipMeshAsset;
	UMaterial* GoodPlacementMaterial;
	UMaterial* BadPlacementMaterial;

	void OnClick();
	void OnSecondary();
	void OnShootAtShip();
	bool IsGridOnBoard(std::pair<int, int> BoardGrid);
	bool AttemptShipPlacement(UShipComponent* Ship);
	bool CanPlaceShip(UShipComponent* Ship);
	UShipComponent* AddShip(UShipComponent* Ship);
};
