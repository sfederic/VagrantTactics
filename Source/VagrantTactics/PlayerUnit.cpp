// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerUnit.h"
#include "Unit.h"
#include "Components/InstancedStaticMeshComponent.h" 
#include "Kismet/GameplayStatics.h"
#include "VagrantTacticsGameModeBase.h"
#include "BattleGrid.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	camera = FindComponentByClass<UCameraComponent>();
	cameraFocusRotation = camera->GetComponentRotation();

	AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	battleGrid = gameMode->activeBattleGrid;
}

void APlayerUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), nextLocation, DeltaTime, moveSpeed));
	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), nextRotation, DeltaTime, rotateSpeed));

	//camera->SetWorldRotation(FMath::RInterpTo(camera->GetComponentRotation(), cameraFocusRotation, DeltaTime, cameraFocusLerpSpeed));
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
}

void APlayerUnit::Move(FVector direction)
{
	if (nextLocation.Equals(GetActorLocation()) && nextRotation.Equals(GetActorRotation()))
	{
		if (bPlayerInBattle)
		{
			if (currentActionPoints > 10)
			{
				nextLocation += (direction * moveDistance);
				currentActionPoints -= 10;
			}
		}
		else
		{
			nextLocation += (direction * moveDistance);
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
					gridActor->currentHealth -= 20;
					currentActionPoints -= 20;
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
				FGridNode node = *battleGrid->nodeMap.Find(hit.Item);
				selectedUnit->MoveTo(node);
			}

			//camera->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(), unit->GetActorLocation()));
		}
		else
		{
			camera->SetWorldRotation(cameraFocusRotation = UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(), GetActorLocation()));
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
