// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerUnit.generated.h"

class UCameraComponent;

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

	UCameraComponent* camera;
	FRotator cameraFocusRotation;
	UPROPERTY(EditAnywhere) float cameraFocusLerpSpeed;

	FVector nextLocation;
	FRotator nextRotation;

	UPROPERTY(EditAnywhere) float moveSpeed;
	UPROPERTY(EditAnywhere) float rotateSpeed;

	const float moveDistance = 100.f;

	UPROPERTY(EditAnywhere) int maxActionPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int currentActionPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool bPlayerInBattle;
};
