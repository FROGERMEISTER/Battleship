// Fill out your copyright notice in the Description page of Project Settings.


#include "Board.h"
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
	SpringArm->TargetArmLength = 1000.0f;
}

std::pair<int, int> ABoard::WorldLocationToBoardGrid(FVector WorldLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("World Location X = %f Y = %f"), WorldLocation.X, WorldLocation.Y)
	FVector BoardOrigin = this->GetActorLocation();
	FVector WorldLocationOnBoard = WorldLocation - BoardOrigin;
	WorldLocationOnBoard = WorldLocationOnBoard / 100 - 5;
	UE_LOG(LogTemp, Warning, TEXT("Board Grid X = %f Y = %f"), WorldLocationOnBoard.X, WorldLocationOnBoard.Y)
	return std::pair<int, int>(WorldLocationOnBoard.X, WorldLocationOnBoard.Y);
}

FVector ABoard::BoardGridToWorldLocation(std::pair<int, int> BoardGrid)
{
	FVector BoardGridLocation = FVector(BoardGrid.first, BoardGrid.second, 0.0f);
	BoardGridLocation = (BoardGridLocation + 5) * 100 - 50;
	BoardGridLocation.Z = this->GetActorLocation().Z;
	return BoardGridLocation;
}

void ABoard::UpdateBoardSelectorLocationToGrid(std::pair<int, int> BoardGrid)
{
	SelectorMeshComponent->SetWorldLocation(BoardGridToWorldLocation(BoardGrid));
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();
	
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

	InputComponent->BindAction("TestInput", IE_Pressed, this, &ABoard::OnClick);
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
	}
}
