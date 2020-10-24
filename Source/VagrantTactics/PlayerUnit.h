// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerUnit.generated.h"

class UCameraComponent;
class ABattleGrid;
class AUnit;
class UCameraShake;
class UParticleSystem;
class UUserWidget;
class AEntranceTrigger;
class UInteractWidget;
class UInteractDetailsWidget;
class AInteractTrigger;
class USpellBase;
class AGridActor;
class UIntuition;

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
	void MoveForwardHold(float val);
	void MoveBackHold(float val);
	void MoveLeftHold(float val);
	void MoveRightHold(float val);
	void RotateLeft();
	void RotateRight();
	void Attack();
	void Guard();
	void EndTurn();
	void Click();
	void PreviewBattleGrid();
	void ResetActionPointsToMax();
	void Cancel();
	void ShowIntuitions();
	void ResetCameraFocusAndFOV();
	void ChangeSpellToIce();
	void ChangeSpellToFire();

	//PARTICLES
	UPROPERTY(EditAnywhere, Category="Particles") UParticleSystem* particleSystemFocus;

	//CAMERA
	UCameraComponent* camera;
	FRotator cameraFocusRotation;
	UPROPERTY(EditAnywhere, Category="Camera") float cameraFocusLerpSpeed;
	UPROPERTY(VisibleAnywhere, Category="Camera") float currentCameraFOV;
	UPROPERTY(EditAnywhere, Category="Camera") float cameraFOVLerpSpeed;
	UPROPERTY(EditAnywhere, Category="Camera") TSubclassOf<UCameraShake> cameraShakeAttack;
	const float maxCameraFOV = 90.f;
	const float cameraFOVAttack = 45.f;

	//WIDGETS
	UPROPERTY(EditAnywhere, Category="Widgets") TSubclassOf<UUserWidget> classWidgetActionPoints;
	UPROPERTY() UUserWidget* widgetActionPoints;

	UPROPERTY(EditAnywhere, Category="Widgets") TSubclassOf<UInteractWidget> classWidgetInteract;
	UPROPERTY() UInteractWidget* widgetInteract;

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UInteractDetailsWidget> classWidgetInteractDetails;
	UPROPERTY() UInteractDetailsWidget* widgetInteractDetails;

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> classIntuitionsWidget;
	UPROPERTY() UUserWidget* intuitionsWidget;

	//SPELLS
	UPROPERTY(EditAnywhere, Category="Spells") TArray<TSubclassOf<USpellBase>> spells;

	UPROPERTY(VisibleAnywhere, Category = "Spells") TSubclassOf<USpellBase> activeSpell;

	UPROPERTY(VisibleAnywhere) AEntranceTrigger* overlappedEntrace;
	UPROPERTY(VisibleAnywhere) AInteractTrigger* overlappedInteractTrigger;

	UPROPERTY(EditAnywhere, Category="AP Costs") int costToAttack;
	UPROPERTY(EditAnywhere, Category="AP Costs") int costToMove;

	UPROPERTY(EditAnywhere) int attackPoints;

	//TODO: think about moving to GameInstance
	//INTUITIONS 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Intuitions") TArray<UIntuition*> intuitions;

	ABattleGrid* battleGrid;
	AGridActor* selectedUnit;

	FVector nextLocation;
	FRotator nextRotation;

	UPROPERTY(EditAnywhere) float moveSpeedDuringBattle;
	UPROPERTY(EditAnywhere) float moveSpeedOutsideBattle;
	UPROPERTY(EditAnywhere) float rotateSpeed;

	const float moveDistance = 100.f;

	UPROPERTY(EditAnywhere) int maxActionPoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int currentActionPoints;

	UPROPERTY(EditAnywhere) int maxHealthPoints;
	UPROPERTY(VisibleAnywhere) int currentHealthPoints;

	UPROPERTY(VisibleAnywhere) int xIndex;
	UPROPERTY(VisibleAnywhere) int yIndex;
};
