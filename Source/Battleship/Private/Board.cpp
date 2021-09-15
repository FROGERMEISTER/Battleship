// Fill out your copyright notice in the Description page of Project Settings.


#include "Board.h"
#include "BattleshipGameMode.h"
#include "BattleshipPlayerState.h"
#include "Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "BattleShipPlayerController.h"

void AddShipsToPlacementMap(TMap<FString, UShipComponent*> PlacedMap, TMap<FString, int> DefaultMap)
{
	for (auto &ship : DefaultMap)
	{
		PlacedMap.Add(ship.Key, nullptr);
	}
}

// Sets default values
ABoard::ABoard()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bOnlyRelevantToOwner = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
	SelectorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectorMesh"));
	SelectorMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	BoardMesh->SetupAttachment(RootComponent);
	SelectorMeshComponent->SetupAttachment(RootComponent);
	SpringArm->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArm);
	
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(-70.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 1500.0f;
	SpringArm->bDoCollisionTest = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>BoardMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	BoardMesh->SetStaticMesh(BoardMeshAsset.Object);
	BoardMesh->SetRelativeScale3D(FVector(10.0f, 10.0f, 1));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh2Asset(TEXT("StaticMesh'/Game/Battleship2long.Battleship2long'"));
	ShipMeshAssets.Add(2, Mesh2Asset.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh3Asset(TEXT("StaticMesh'/Game/Battleship3long.Battleship3long'"));
	ShipMeshAssets.Add(3, Mesh3Asset.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh4Asset(TEXT("StaticMesh'/Game/Battleship4long.Battleship4long'"));
	ShipMeshAssets.Add(4, Mesh4Asset.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh5Asset(TEXT("StaticMesh'/Game/Battleship5long.Battleship5long'"));
	ShipMeshAssets.Add(5, Mesh5Asset.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>SelectorMeshAsset(TEXT("StaticMesh'/Game/SelectorMesh.SelectorMesh'"));
	SelectorMesh = SelectorMeshAsset.Object;



	static ConstructorHelpers::FObjectFinder<UMaterial>GoodPlacementMaterialAsset(TEXT("Material'/Game/GoodPlacement.GoodPlacement'"));
	GoodPlacementMaterial = GoodPlacementMaterialAsset.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial>BadPlacementMaterialAsset(TEXT("Material'/Game/BadPlacement.BadPlacement'"));
	BadPlacementMaterial = BadPlacementMaterialAsset.Object;

	SelectorMeshComponent->SetMaterial(0, GoodPlacementMaterial);

	DefaultShipsToPlace.Add("Carrier", 5);
	DefaultShipsToPlace.Add("Battleship", 4);
	DefaultShipsToPlace.Add("Cruiser", 3);
	DefaultShipsToPlace.Add("Submarine", 3);
	DefaultShipsToPlace.Add("Destroyer", 2);
	AddShipsToPlacementMap(ShipsPlaced, DefaultShipsToPlace);
}

void ABoard::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current ship.
	DOREPLIFETIME(ABoard, CurrentShip);
}

void ABoard::OnRep_CurrentShip()
{
	FString ShipMessage = FString::Printf(TEXT("%s now has %d ships."), *GetFName().ToString(), ShipsOnBoard.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, ShipMessage);
	if (GetLocalRole() == ROLE_Authority)
	{
		ShipMessage = FString::Printf(TEXT("%s now has %d ships."), *GetFName().ToString(), ShipsOnBoard.Num());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, ShipMessage);
	}
	ABattleShipPlayerController* PlayerController = Cast<ABattleShipPlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		UpdateBoardSelectorLocationToGrid(PlayerController->SelectedBoardGrid);
	}
}

std::pair<int, int> ABoard::WorldLocationToBoardGrid(FVector WorldLocation)
{
	FVector BoardOrigin = this->GetActorLocation();
	FVector WorldLocationOnBoard = WorldLocation - (BoardOrigin - 500);
	WorldLocationOnBoard = WorldLocationOnBoard / 100;
	//UE_LOG(LogTemp, Warning, TEXT("Placing ship grid %d %d"), (int)WorldLocationOnBoard.X, (int)WorldLocationOnBoard.Y);
	return std::pair<int, int>(WorldLocationOnBoard.X, WorldLocationOnBoard.Y);
}

FVector ABoard::BoardGridToWorldLocation(std::pair<int, int> BoardGrid)
{
	FVector BoardGridLocation = FVector(BoardGrid.first, BoardGrid.second, 0.0f);
	BoardGridLocation = BoardGridLocation * 100 - 450;
	BoardGridLocation += this->GetActorLocation();
	BoardGridLocation.Z = this->GetActorLocation().Z + 10.0f;
	return BoardGridLocation;
}

void ABoard::UpdateBoardSelectorLocationToGrid(std::pair<int, int> BoardGrid)
{
	if (CurrentShip == nullptr)
		return;
	std::pair<int, int> OldShipStart;
	std::pair<int, int> OldShipEnd;
	int EndPlacementX = BoardGrid.first + (ShipPlacementDirection.first * (ShipLength - 1));
	int EndPlacementY = BoardGrid.second + (ShipPlacementDirection.second * (ShipLength - 1));
	if (EndPlacementX - 10 > 0)
	{
		BoardGrid.first -= 10 - EndPlacementX;
	}
	if (EndPlacementY - 10 > 0)
	{
		BoardGrid.second -= 10 - EndPlacementY;
	}
	if (EndPlacementY < 0)
	{
		BoardGrid.second -= EndPlacementY;
	}
	if (EndPlacementX < 0)
	{
		BoardGrid.first -= EndPlacementX;
	}
	std::pair<int, int> PlacementLocationEnd = std::pair<int, int>(BoardGrid.first + (ShipPlacementDirection.first * (ShipLength - 1)), BoardGrid.second + (ShipPlacementDirection.second * (ShipLength - 1)));
	CurrentShip->SetShipLocation(BoardGrid, PlacementLocationEnd);
	SelectorMeshComponent->SetWorldLocation(BoardGridToWorldLocation(BoardGrid));
	SelectorMeshComponent->AddRelativeLocation(FVector(0.0f, 0.0f, 5.0f));
	RefreshSelectorValidPlacement();
}

void ABoard::RefreshSelectorValidPlacement()
{
	if (CanPlaceShip(CurrentShip))
	{
		SelectorMeshComponent->SetMaterial(0, GoodPlacementMaterial);
	}
	else
	{
		SelectorMeshComponent->SetMaterial(0, BadPlacementMaterial);
	}
}

UShipComponent* ABoard::ShipAtGridLocation(std::pair<int, int> BoardGrid)
{
	for(UShipComponent* Ship : ShipsOnBoard)
	{
		if (Ship->IsBoardGridInShip(BoardGrid))
		{
			return Ship;
		}
	}
	return nullptr;
}

bool ABoard::AreAllShipsPlaced()
{
	UE_LOG(LogTemp, Warning, TEXT("ShipsOnBoard %d, Ships To Place %d"), ShipsOnBoard.Num(), DefaultShipsToPlace.Num());
	if (ShipsOnBoard.Num() != DefaultShipsToPlace.Num())
		return false;
	if (ShipsPlaced.FindKey(nullptr) == nullptr)
	{
		return true;
	}
	return false;
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();

	CreateNewShip();
	SelectorMeshComponent->SetStaticMesh(*(ShipMeshAssets.Find(ShipLength)));
	UE_LOG(LogTemp, Warning, TEXT("Placing ship %d"), ShipsOnBoard.Num());
	UE_LOG(LogTemp, Warning, TEXT("OwningController %s"), *GetDebugName(GetController()));
}

// Called every frame
void ABoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABoard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Action1", IE_Pressed, this, &ABoard::OnClick);
	InputComponent->BindAction("Action2", IE_Pressed, this, &ABoard::OnSecondary);
	InputComponent->BindAction("RotateShip", IE_Pressed, this, &ABoard::OnRotateShip);
	InputComponent->BindAction("SelectNextShip", IE_Pressed, this, &ABoard::SelectNextShip);
	InputComponent->BindAction("NextPhase", IE_Pressed, this, &ABoard::NextPhase);
}

void ABoard::NextPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("NextPhase"));
	if (AreAllShipsPlaced())
	{
		SelectorMeshComponent->SetStaticMesh(SelectorMesh);
		PlacementMode = false;
		PlayerReady = true;
		HandlePlayerReady();
	}
}

void ABoard::HandlePlayerReady_Implementation()
{
	if (AreAllShipsPlaced())
	{
		SelectorMeshComponent->SetStaticMesh(SelectorMesh);
		PlacementMode = false;
		PlayerReady = true;
		ABattleshipGameMode* GameMode = GetWorld() != NULL ? GetWorld()->GetAuthGameMode<ABattleshipGameMode>() : NULL;
		if (GameMode != nullptr)
			GameMode->TryToStartGame();
	}
}

void ABoard::OnClick()
{
	ABattleShipPlayerController* PlayerController = Cast<ABattleShipPlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		std::pair<int, int> BoardGrid = this->WorldLocationToBoardGrid(PlayerController->MouseTraceLocation);
		if (PlacementMode && AttemptShipPlacement(CurrentShip))
		{
			UE_LOG(LogTemp, Warning, TEXT("Placing ship %d"), ShipsOnBoard.Num());
			CreateNewShip();
			UpdateBoardSelectorLocationToGrid(BoardGrid);
		}
	}
}

void ABoard::OnSecondary()
{
	ABattleShipPlayerController* PlayerController = Cast<ABattleShipPlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		if (PlacementMode)
		{
			UShipComponent* ShipAtGrid = ShipAtGridLocation(PlayerController->SelectedBoardGrid);
			if (ShipAtGrid != nullptr)
			{
				RemoveShip(ShipAtGrid);
			}
		}
	}
}

void ABoard::OnRotateShip()
{
	ShipPlacementDirection = std::pair<int, int>(ShipPlacementDirection.second, ShipPlacementDirection.first);
	SelectorMeshComponent->SetWorldRotation(FRotator(0.0f, -90.0f * ShipPlacementDirection.second, 0.0f));
}

void ABoard::OnShootAtShip()
{
	ABattleShipPlayerController* PlayerController = Cast<ABattleShipPlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		for (UShipComponent* Ship : ShipsOnBoard)
		{
			if (Ship->ShootAtShip(PlayerController->SelectedBoardGrid))
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit"));
			}
		}
	}
}

bool ABoard::AttemptShipPlacement(UShipComponent* Ship)
{
	
	if (CanPlaceShip(Ship))
	{
		AddShip(Ship);
		return true;
	}
	return false;
}

bool ABoard::CanPlaceShip(UShipComponent* Ship)
{
	if (Ship == nullptr)
		return false;
	TArray<FString> SelectableShips;
	DefaultShipsToPlace.GenerateKeyArray(SelectableShips);
	UE_LOG(LogTemp, Warning, TEXT("ShipPlacementTest"));
	if (ShipsPlaced.Find(SelectableShips[SelectedShip]) != nullptr && *(ShipsPlaced.Find(SelectableShips[SelectedShip])) != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PointerShip %p"), (void*)*(ShipsPlaced.Find(SelectableShips[SelectedShip])));
		UE_LOG(LogTemp, Warning, TEXT("Ship Already placed %s"), *(SelectableShips[SelectedShip]));
		return false;
	}
	if (IsGridOnBoard(Ship->GetShipStart()) && IsGridOnBoard(Ship->GetShipEnd()))
	{
		for (UShipComponent* ShipIterator : ShipsOnBoard)
		{
			if (ShipIterator->IsNewShipIntersectingShip(Ship))
			{
				return false;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Can place ship"));
		return true;
	}
	return false;
}

FString ABoard::GetEmptyShipKeyFromDefaultShips(int length)
{
	for (auto& Ship : DefaultShipsToPlace)
	{
		if (Ship.Value == length && (ShipsPlaced.Find(Ship.Key) == nullptr || *ShipsPlaced.Find(Ship.Key) == nullptr))
			return Ship.Key;
	}
	return FString();
}

UShipComponent* ABoard::AddShip(UShipComponent* Ship)
{
	if (Ship != nullptr)
	{
		if (GetEmptyShipKeyFromDefaultShips(CurrentShip->GetShipLength()) == "")
			return nullptr;
		CurrentShip->HandleSetShipLocation(CurrentShip->GetShipStart().first, CurrentShip->GetShipStart().second, CurrentShip->GetShipEnd().first, CurrentShip->GetShipEnd().second);
		HandleAddShip();
		ShipsPlaced.Emplace(GetEmptyShipKeyFromDefaultShips(CurrentShip->GetShipLength()), CurrentShip);
		ShipsOnBoard.Add(CurrentShip);
		CurrentShip->SetWorldRotation(FRotator(0.0f, -90.0f * ShipPlacementDirection.second, 0.0f));
		CurrentShip->SetStaticMesh(*(ShipMeshAssets.Find(ShipLength)));
		CurrentShip->SetWorldLocation(BoardGridToWorldLocation(CurrentShip->GetShipStart()));
		return Ship;
	}
	return nullptr;
}

void ABoard::HandleAddShip_Implementation()
{
	FString ShipKey = GetEmptyShipKeyFromDefaultShips(CurrentShip->GetShipLength());
	CurrentShip->ShipKey = ShipKey;
	CurrentShip->RegisterComponent();
	CurrentShip->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	CurrentShip->SetWorldLocation(BoardGridToWorldLocation(CurrentShip->GetShipStart()));
	ShipsPlaced.Emplace(ShipKey, CurrentShip);
	GetPlayerState<ABattleshipPlayerState>()->MulticastAddShip(ShipKey);
	ShipsOnBoard.Add(CurrentShip);
}

void ABoard::RemoveShip(UShipComponent* Ship)
{
	FString Key = (*(ShipsPlaced.FindKey(Ship)));
	UE_LOG(LogTemp, Warning, TEXT("Remove Ship %s"), *Key);
	ShipsPlaced.Emplace(Key, nullptr);
	int32 RemovedItems = ShipsOnBoard.Remove(Ship);
	RefreshSelectorValidPlacement();
	HandleRemoveShip(Ship);
}

void ABoard::HandleRemoveShip_Implementation(UShipComponent* Ship)
{
	if (ShipsPlaced.FindKey(Ship) == nullptr)
		return;
	FString Key = (*(ShipsPlaced.FindKey(Ship)));
	ShipsPlaced.Emplace(Key, nullptr);
	ShipsOnBoard.Remove(Ship);
	Ship->DestroyComponent();
}

void ABoard::CreateNewShip_Implementation()
{
	CurrentShip = NewObject<UShipComponent>(this, UShipComponent::StaticClass(), MakeUniqueObjectName(this, UShipComponent::StaticClass(), TEXT("Ship")));
}

TArray<FString> ABoard::GetPlacedShipKeys()
{
	TArray<FString> Keys;
	ShipsPlaced.GenerateKeyArray(Keys);
	return Keys;
}

void ABoard::DestroyShipWithKey(FString Key)
{
	GetPlayerState<ABattleshipPlayerState>()->MulticastDestroyShip(Key);
}

bool ABoard::IsGridOnBoard(std::pair<int, int> BoardGrid)
{
	if (BoardGrid.first >= 0 && BoardGrid.second < BoardSize.first && BoardGrid.second >= 0 && BoardGrid.second < BoardSize.second)
	{
		return true;
	}
	return false;
}

void ABoard::SelectNextShip()
{
	if (CurrentShip == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("NextShip"));
	SelectedShip++;
	SelectedShip = SelectedShip % DefaultShipsToPlace.Num();
	TArray<FString> SelectableShips;
	DefaultShipsToPlace.GenerateKeyArray(SelectableShips);
	//ShipPlacementDirection = std::pair<int, int>(-1, 0);
	ShipLength = *(DefaultShipsToPlace.Find(SelectableShips[SelectedShip]));
	SelectorMeshComponent->SetStaticMesh(*(ShipMeshAssets.Find(ShipLength)));
	std::pair<int, int> PlacementLocationEnd = std::pair<int, int>(CurrentShip->GetShipStart().first + (ShipPlacementDirection.first * (ShipLength - 1)), CurrentShip->GetShipStart().second + (ShipPlacementDirection.second * (ShipLength - 1)));
	CurrentShip->SetShipLocation(CurrentShip->GetShipStart(), PlacementLocationEnd);
	RefreshSelectorValidPlacement();
}