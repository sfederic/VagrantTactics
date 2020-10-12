// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerUnit.h"
#include "Unit.h"
#include "Components/InstancedStaticMeshComponent.h" 
#include "Kismet/GameplayStatics.h"
#include "VagrantTacticsGameModeBase.h"
#include "BattleGrid.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "LevelGridValues.h"

APlayerUnit::APlayerUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerUnit::BeginPlay()
{
	Super::BeginPlay();

	nextLocation = GetActorLocation();
	nextRotation = GetActorRotation();

	currentActionPoints = maxActionPoints;

	APlayerController* controller = Cast<APlayerController>(GetController());
	controller->bShowMouseCursor = true;

	//Main camera setup
	camera = FindComponentByClass<UCameraComponent>();
	cameraFocusRotation = camera->GetComponentRotation();
	currentCameraFOV = camera->FieldOfView;

	AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	battleGrid = gameMode->activeBattleGrid;
}

void APlayerUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Set grid indices
	xIndex = FMath::RoundToInt(nextLocation.X / LevelGridValues::gridUnitDistance);
	yIndex = FMath::RoundToInt(nextLocation.Y / LevelGridValues::gridUnitDistance);

	//Lerp movement and rotation
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), nextLocation, DeltaTime, moveSpeed));
	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), nextRotation, DeltaTime, rotateSpeed));

	if (selectedUnit)
	{
		cameraFocusRotation = UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(), selectedUnit->GetActorLocation());
	}
	else
	{
		cameraFocusRotation = UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(), GetActorLocation());
	}

	camera->SetWorldRotation(FMath::RInterpTo(camera->GetComponentRotation(), cameraFocusRotation, DeltaTime, cameraFocusLerpSpeed));
	camera->SetFieldOfView(FMath::FInterpTo(camera->FieldOfView, currentCameraFOV, DeltaTime, cameraFOVLerpSpeed));
}

void APlayerUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction(TEXT("Forward"), EInputEvent::IE_Pressed, this, &APlayerUnit::MoveForward);
	InputComponent->BindAction(TEXT("Back"), EInputEvent::IE_Pressed, this, &APlayerUnit::MoveBack);
	InputComponent->BindAction(TEXT("Left"), EInputEvent::IE_Pressed, this, &APlayerUnit::MoveLeft);
	InputComponent->BindAction(TEXT("Right"), EInputEvent::IE_Pressed, this, &APlayerUnit::MoveRight);

	InputComponent->BindAction(TEXT("RotateLeft"), EInputEvent::IE_Pressed, this, &APlayerUnit::RotateLeft);
	InputComponent->BindAction(TEXT("RotateRight"), EInputEvent::IE_Pressed, this, &APlayerUnit::RotateRight);

	InputComponent->BindAction(TEXT("PrimaryAction"), EInputEvent::IE_Pressed, this, &APlayerUnit::Attack);
	InputComponent->BindAction(TEXT("SecondaryAction"), EInputEvent::IE_Pressed, this, &APlayerUnit::Guard);

	InputComponent->BindAction(TEXT("EndTurn"), EInputEvent::IE_Pressed, this, &APlayerUnit::EndTurn);
	InputComponent->BindAction(TEXT("Click"), EInputEvent::IE_Pressed, this, &APlayerUnit::Click);
	InputComponent->BindAction(TEXT("StartCombat"), EInputEvent::IE_Pressed, this, &APlayerUnit::StartCombat);
	InputComponent->BindAction(TEXT("Cancel"), EInputEvent::IE_Pressed, this, &APlayerUnit::Cancel);
}

void APlayerUnit::Move(FVector direction)
{
	if (nextLocation.Equals(GetActorLocation()) && nextRotation.Equals(GetActorRotation()))
	{
		currentCameraFOV = maxCameraFOV;
		selectedUnit = nullptr;
		nextLocation += (direction * moveDistance);

		//Set grid indices
		xIndex = FMath::RoundToInt(nextLocation.X / LevelGridValues::gridUnitDistance);
		yIndex = FMath::RoundToInt(nextLocation.Y / LevelGridValues::gridUnitDistance);

		//return out of function if player is on grid boundary
		if (direction.Equals(-FVector::ForwardVector))
		{
			if (xIndex < 0)
			{
				nextLocation = GetActorLocation(); //The GetActorLocation()'s are just setting nextLocation to the previous location
				UE_LOG(LogTemp, Warning, TEXT("Player move hit X- edge"));
				return;
			}
		}
		else if (direction.Equals(FVector::ForwardVector))
		{
			if (xIndex >= battleGrid->sizeX)
			{
				nextLocation = GetActorLocation();
				UE_LOG(LogTemp, Warning, TEXT("Player move hit X+ edge"));
				return;
			}
		}
		else if (direction.Equals(-FVector::RightVector))
		{
			if (yIndex < 0)
			{
				nextLocation = GetActorLocation();
				UE_LOG(LogTemp, Warning, TEXT("Player move hit Y- edge"));
				return;
			}
		}
		else if (direction.Equals(FVector::RightVector))
		{
			if (yIndex >= battleGrid->sizeY)
			{
				nextLocation = GetActorLocation();
				UE_LOG(LogTemp, Warning, TEXT("Player move hit Y+ edge"));
				return;
			}
		}

		FGridNode* nextNodeToMoveTo = battleGrid->GetNode(xIndex, yIndex);
		if (!nextNodeToMoveTo->bActive)
		{
			nextLocation = GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Player can't move into inactive node"));
			return;
		}
		else
		{
			nextLocation = nextNodeToMoveTo->location;
		}
	}
}

void APlayerUnit::MoveForward()
{
	Move(GetActorForwardVector());
}

void APlayerUnit::MoveBack()
{
	Move(-GetActorForwardVector());
}

void APlayerUnit::MoveLeft()
{
	Move(-GetActorRightVector());
}

void APlayerUnit::MoveRight()
{
	Move(GetActorRightVector());
}

void APlayerUnit::RotateLeft()
{
	if (nextLocation.Equals(GetActorLocation()) && nextRotation.Equals(GetActorRotation()))
	{
		nextRotation.Yaw -= 90.f;
	}
}

void APlayerUnit::RotateRight()
{
	if (nextLocation.Equals(GetActorLocation()) && nextRotation.Equals(GetActorRotation()))
	{
		nextRotation.Yaw += 90.f;
	}
}

void APlayerUnit::Attack()
{
	if (nextLocation.Equals(GetActorLocation()) && nextRotation.Equals(GetActorRotation()))
	{
		if (currentActionPoints > 20)
		{
			FHitResult hit;
			FCollisionQueryParams hitParams;
			hitParams.AddIgnoredActor(this);
			FVector endHit = GetActorLocation() + GetActorForwardVector() * moveDistance;
			if (GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), endHit, ECC_WorldStatic, hitParams))
			{
				AGridActor* gridActor = Cast<AGridActor>(hit.GetActor());
				if (gridActor)
				{
					//gridActor->currentHealth -= 20;
					//currentActionPoints -= 20;

					AUnit* unit = Cast<AUnit>(gridActor);
					if (unit)
					{
						selectedUnit = unit;

						currentCameraFOV = cameraFOVAttack;

						//Dealing with unit position on attack
						if (unit->GetActorForwardVector().Equals(-GetActorForwardVector())) //Front attack
						{
							if (unit->bFrontVulnerable)
							{
								UE_LOG(LogTemp, Warning, TEXT("front attack"));
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("Unit front invulnerable"));
							}
						}
						else if (unit->GetActorForwardVector().Equals(GetActorForwardVector())) //Back attack
						{
							if (unit->bBackVulnerable)
							{
								UE_LOG(LogTemp, Warning, TEXT("back attack"));
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("Unit back invulnerable"));
							}
						}
						else if (unit->GetActorRightVector().Equals(GetActorForwardVector())) //Left side attack
						{
							if (unit->bLeftVulnerable)
							{
								UE_LOG(LogTemp, Warning, TEXT("left side attack"));
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("Unit left invulnerable"));
							}
						}
						else if (unit->GetActorRightVector().Equals(-GetActorForwardVector())) //Right side attack
						{
							if (unit->bRightVulnerable)
							{
								UE_LOG(LogTemp, Warning, TEXT("right side attack"));
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("Unit right invulnerable"));
							}
						}
					}
				}
			}
		}
	}
}

void APlayerUnit::Guard()
{
	if (nextLocation.Equals(GetActorLocation()) && nextRotation.Equals(GetActorRotation()))
	{
		if (currentActionPoints > 20)
		{
			currentActionPoints -= 20;
		}
	}
}

void APlayerUnit::EndTurn()
{
	if (nextLocation.Equals(GetActorLocation()) && nextRotation.Equals(GetActorRotation()))
	{
		currentActionPoints = maxActionPoints;
	}
}

void APlayerUnit::Click()
{
	APlayerController* controller = Cast<APlayerController>(GetController());
	FHitResult hit;
	if (controller->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldStatic), true, hit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Clicked Actor: %s | Index: %d"), *hit.GetActor()->GetName(), hit.Item);

		AUnit* unit = Cast<AUnit>(hit.GetActor());
		if (unit)
		{
			selectedUnit = unit;
		}
		
		if (selectedUnit)
		{
			selectedUnit->ShowMovementPath(selectedUnit->currentMovementPoints);

			if (hit.Item > -1)
			{
				FGridNode* node = battleGrid->nodeMap.Find(hit.Item);
				selectedUnit->MoveTo(node);
			}
		}
		else
		{
			selectedUnit = nullptr;
		}
	}
}

void APlayerUnit::StartCombat()
{
	AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	bPlayerInBattle = !bPlayerInBattle;
	gameMode->activeBattleGrid->ActivateBattle();
}

void APlayerUnit::ResetActionPointsToMax()
{
	currentActionPoints = maxActionPoints;
}

//For now just resets camera focus
void APlayerUnit::Cancel()
{
	cameraFocusRotation = UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(), GetActorLocation());
	selectedUnit = nullptr;
	currentCameraFOV = maxCameraFOV;
}
