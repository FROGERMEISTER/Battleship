// Fill out your copyright notice in the Description page of Project Settings.


#include "Board.h"
#include "Engine/EngineTypes.h"
#include "BattleShipPlayerController.h"

// Sets default values
ABoard::ABoard()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Battleship.Battleship'"));
	ShipMeshAsset = MeshAsset.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial>GoodPlacementMaterialAsset(TEXT("Material'/Game/GoodPlacement.GoodPlacement'"));
	GoodPlacementMaterial = GoodPlacementMaterialAsset.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial>BadPlacementMaterialAsset(TEXT("Material'/Game/BadPlacement.BadPlacement'"));
	BadPlacementMaterial = BadPlacementMaterialAsset.Object;
}

std::pair<int, int> ABoard::WorldLocationToBoardGrid(FVector WorldLocation)
{
	FVector BoardOrigin = this->GetActorLocation();
	FVector WorldLocationOnBoard = WorldLocation - (BoardOrigin - 500);
	WorldLocationOnBoard = WorldLocationOnBoard / 100;
	return std::pair<int, int>(WorldLocationOnBoard.X, WorldLocationOnBoard.Y);
}

FVector ABoard::BoardGridToWorldLocation(std::pair<int, int> BoardGrid)
{
	FVector BoardGridLocation = FVector(BoardGrid.first, BoardGrid.second, 0.0f);
	BoardGridLocation = BoardGridLocation * 100 - 450;
	BoardGridLocation.Z = this->GetActorLocation().Z;
	return BoardGridLocation;
}

void ABoard::UpdateBoardSelectorLocationToGrid(std::pair<int, int> BoardGrid)
{
	SelectorMeshComponent->SetWorldLocation(BoardGridToWorldLocation(BoardGrid));
	SelectorMeshComponent->SetStaticMesh(ShipMeshAsset);
	std::pair<int, int> PlacementDirection = std::pair<int, int>(-1, 0);
	int ShipLength = 4;
	std::pair<int, int> PlacementLocationEnd = std::pair<int, int>(BoardGrid.first + (PlacementDirection.first * ShipLength), BoardGrid.second + (PlacementDirection.second * ShipLength));
	CurrentShip->SetShipLocation(BoardGrid, PlacementLocationEnd);
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
	for(UShipComponent* Ship : ShipsLeft)
	{
		if (Ship->IsBoardGridInShip(BoardGrid))
		{
			return Ship;
		}
	}
	return nullptr;
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();
	CurrentShip = NewObject<UShipComponent>(this, UShipComponent::StaticClass(), MakeUniqueObjectName(this, UShipComponent::StaticClass(), TEXT("Ship")));
	UE_LOG(LogTemp, Warning, TEXT("Placing ship %d"), ShipsLeft.Num());
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
}

void ABoard::OnClick()
{
	UE_LOG(LogTemp, Warning, TEXT("Click"))
	ABattleShipPlayerController* PlayerController = Cast<ABattleShipPlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Original Trace %s"), *(PlayerController->MouseTraceLocation.ToString()));
		std::pair<int, int> BoardGrid = this->WorldLocationToBoardGrid(PlayerController->MouseTraceLocation);
		UE_LOG(LogTemp, Warning, TEXT("World Location of Grid %s"), *(BoardGridToWorldLocation(BoardGrid).ToString()));
		if (AttemptShipPlacement(CurrentShip))
		{
			UE_LOG(LogTemp, Warning, TEXT("Placing ship %d"), ShipsLeft.Num());
			CurrentShip = NewObject<UShipComponent>(this, UShipComponent::StaticClass(), MakeUniqueObjectName(this, UShipComponent::StaticClass(), TEXT("Ship")));
			UpdateBoardSelectorLocationToGrid(BoardGrid);
		}
	}
}

void ABoard::OnSecondary()
{
	ABattleShipPlayerController* PlayerController = Cast<ABattleShipPlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		UShipComponent* SelectedShip = ShipAtGridLocation(PlayerController->SelectedBoardGrid);
		if (SelectedShip != nullptr)
		{
			int32 RemovedItems = ShipsLeft.Remove(SelectedShip);
			UE_LOG(LogTemp, Warning, TEXT("RemovedShips %d"), RemovedItems);
			SelectedShip->DestroyComponent();
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Secondary"));
	UE_LOG(LogTemp, Warning, TEXT("Number Of ships %d"), ShipsLeft.Num());
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
	
	for (UShipComponent* ShipIterator : ShipsLeft)
	{
		if (ShipIterator->IsLineIntersectingShip(Ship))
		{
			return false;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Can place ship"));
	return true;
}

UShipComponent* ABoard::AddShip(UShipComponent* Ship)
{
	if (Ship != nullptr)
	{
		Ship->RegisterComponent();
		Ship->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		Ship->SetWorldLocation(BoardGridToWorldLocation(Ship->GetShipStart()));
		ShipsLeft.Add(Ship);
		return Ship;
	}
	return nullptr;
}

