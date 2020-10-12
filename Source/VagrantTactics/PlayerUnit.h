// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerUnit.generated.h"

class UCameraComponent;
class ABattleGrid;
class AUnit;
class UCameraShake;

//Player class.
UCLASS()
class VAGRANTTACTICS_API APlayerUnit : public APawn
{
	GENERATED_BODY()

public:
	APlayerUnit();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(FVector direction);
	void MoveForward();
	void MoveBack();
	void MoveLeft();
	void MoveRight();
	void RotateLeft();
	void RotateRight();
	void Attack();
	void Guard();
	void EndTurn();
	void Click();
	void StartCombat();
	void ResetActionPointsToMax();
	void Cancel();

	UCameraComponent* camera;
	FRotator cameraFocusRotation;
	UPROPERTY(EditAnywhere, Category="Camera") float cameraFocusLerpSpeed;
	UPROPERTY(VisibleAnywhere, Category="Camera") float currentCameraFOV;
	UPROPERTY(EditAnywhere, Category="Camera") float cameraFOVLerpSpeed;

	UPROPERTY(EditAnywhere, Category="Camera") TSubclassOf<UCameraShake> cameraShakeAttack;

	const float maxCameraFOV = 90.f;
	const float cameraFOVAttack = 45.f;

	UPROPERTY(EditAnywhere, Category="AP Costs") int costToAttack;
	UPROPERTY(EditAnywhere, Category="AP Costs") int costToMove;

	ABattleGrid* battleGrid;
	AUnit* selectedUnit;

	FVector nextLocation;
	FRotator nextRotation;

	UPROPERTY(EditAnywhere) float moveSpeed;
	UPROPERTY(EditAnywhere) float rotateSpeed;

	const float moveDistance = 100.f;

	UPROPERTY(EditAnywhere) int maxActionPoints;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int currentActionPoints;

	UPROPERTY(VisibleAnywhere) int xIndex;
	UPROPERTY(VisibleAnywhere) int yIndex;
};
