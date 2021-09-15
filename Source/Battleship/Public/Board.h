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
	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	std::pair<int, int> WorldLocationToBoardGrid(FVector WorldLocation);
	FVector BoardGridToWorldLocation(std::pair<int, int> BoardGrid);
	virtual void UpdateBoardSelectorLocationToGrid(std::pair<int, int> BoardGrid);
	TArray<FString> GetPlacedShipKeys();
	void DestroyShipWithKey(FString Key);
	UShipComponent* ShipAtGridLocation(std::pair<int, int> BoardGrid);
	bool AreAllShipsPlaced();
	bool PlacementMode = true;
	bool PlayerReady = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	class UCameraComponent* CameraComponent;

	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BoardMesh;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SelectorMeshComponent;

	TMap<int, UStaticMesh*> ShipMeshAssets;
	UStaticMesh* SelectorMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	std::pair<int, int> BoardSize = std::pair<int, int>(10, 10);

	TArray<UShipComponent*> ShipsOnBoard;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentShip)
	class UShipComponent* CurrentShip;
	/** RepNotify for changes made to current ships.*/
	UFUNCTION()
	void OnRep_CurrentShip();

	TMap<FString, int> DefaultShipsToPlace;
	TMap<FString, UShipComponent*> ShipsPlaced;

	std::pair<int, int> ShipPlacementDirection = std::pair<int, int>(-1, 0);
	int ShipLength = 5;
	int SelectedShip = 0;
	
	UMaterial* GoodPlacementMaterial;
	UMaterial* BadPlacementMaterial;

	virtual void OnClick();
	void OnSecondary();
	void OnShootAtShip();
	void OnRotateShip();
	void SelectNextShip();
	void RefreshSelectorValidPlacement();
	void NextPhase();
	bool IsGridOnBoard(std::pair<int, int> BoardGrid);
	bool AttemptShipPlacement(UShipComponent* Ship);
	bool CanPlaceShip(UShipComponent* Ship);
	UShipComponent* AddShip(UShipComponent* Ship);
	FString GetEmptyShipKeyFromDefaultShips(int length);

	UFUNCTION(Server, Reliable)
	void HandleAddShip();
	void HandleAddShip_Implementation();

	UFUNCTION(Server, Reliable)
	void HandleRemoveShip(UShipComponent* Ship);
	void HandleRemoveShip_Implementation(UShipComponent* Ship);

	UFUNCTION(Server, Reliable)
	void CreateNewShip();
	void CreateNewShip_Implementation();

	UFUNCTION(Server, Reliable)
	void HandlePlayerReady();
	void HandlePlayerReady_Implementation();

	void RemoveShip(UShipComponent* Ship);
};

