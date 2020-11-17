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
class UStaticMeshComponent;
class APickupItem;
class UUnitSkillWidget;
class AConversationInstance;
class ASavePoint;

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
	void Rotate(float angle);
	void PrimaryAction();
	void SecondaryAction();
	void EndTurn();
	void Click();
	void PreviewBattleGrid();
	void ResetActionPointsToMax();
	void Cancel();
	void ShowIntuitions();
	void ResetCameraFocusAndFOV();
	void ChangeSpellToIce();
	void ChangeSpellToFire();
	void MoveToLevel();
	void ZoomInOnTarget(AActor* target);
	void ResetGuardWindow();
	void ActivateGuardWindow(float windUpTime);
	void WorldReset();
	void AddIntuition(UIntuition* intuitionToAdd);

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
	const float cameraFOVConversation = 60.f;

	//WIDGETS
	UPROPERTY(EditAnywhere, Category="Widgets") TSubclassOf<UUserWidget> classWidgetActionPoints;
	UPROPERTY() UUserWidget* widgetActionPoints;

	UPROPERTY(EditAnywhere, Category="Widgets") TSubclassOf<UInteractWidget> classWidgetInteract;
	UPROPERTY() UInteractWidget* widgetInteract;

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UInteractDetailsWidget> classWidgetInteractDetails;
	UPROPERTY() UInteractDetailsWidget* widgetInteractDetails;

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> classIntuitionsWidget;
	UPROPERTY() UUserWidget* intuitionsWidget;

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> classTimeOfDayWidget;
	UPROPERTY() UUserWidget* timeOfDayWidget;

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> classEnemyTurnOrderWidget;
	UPROPERTY() UUserWidget* widgetEnemyTurnOrder;

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUnitSkillWidget> classUnitSkillWidget;
	UPROPERTY() UUnitSkillWidget* widgetUnitSkill;

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> classGuardWidget;
	UPROPERTY() UUserWidget* widgetGuard;

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> classDeathWidget;
	UPROPERTY() UUserWidget* widgetDeath;

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> classDebugControlsWidget;
	UPROPERTY() UUserWidget* widgetDebugControls;

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> classIntuitionTransfer;
	UPROPERTY() UUserWidget* widgetIntuitionTransfer;

	//SPELLS
	UPROPERTY(EditAnywhere, Category = "Spells") TArray<TSubclassOf<USpellBase>> spells;
	UPROPERTY(VisibleAnywhere, Category = "Spells") TSubclassOf<USpellBase> activeSpell;

	//OVERLAPS
	UPROPERTY(VisibleAnywhere, Category = "Overlaps") AEntranceTrigger* overlappedEntrace;
	UPROPERTY(VisibleAnywhere, Category = "Overlaps") AInteractTrigger* overlappedInteractTrigger;
	UPROPERTY(VisibleAnywhere, Category = "Overlaps") APickupItem* overlappedPickupItem;
	ASavePoint* overlappedSavePoint;

	//AP COSTS
	UPROPERTY(EditAnywhere, Category="AP Costs") int costToAttack;
	UPROPERTY(EditAnywhere, Category="AP Costs") int costToMove;

	UPROPERTY(EditAnywhere) int attackPoints;
	UPROPERTY(EditAnywhere) int guardPoints;

	//INTUITIONS 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Intuitions") TArray<UIntuition*> intuitions;
	UPROPERTY(BlueprintReadOnly) TArray<UIntuition*> intuitionsToCarryOver;
	UPROPERTY(BlueprintReadOnly) int maxIntuitionsToCarryOver = 4;

	//MESHES
	UStaticMeshComponent* mesh;
	UStaticMeshComponent* meshSword;

	ABattleGrid* battleGrid;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) AActor* selectedUnit;

	FVector nextLocation;
	FRotator nextRotation;

	UPROPERTY(VisibleAnywhere) FName previousLevelMovedFrom;

	UPROPERTY(EditAnywhere) float moveSpeedDuringBattle;
	UPROPERTY(EditAnywhere) float moveSpeedOutsideBattle;
	UPROPERTY(EditAnywhere) float rotateSpeed;

	const float moveDistance = 100.f;

	UPROPERTY(EditAnywhere) int maxActionPoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int currentActionPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) int maxHealthPoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int currentHealthPoints;

	UPROPERTY(VisibleAnywhere) int xIndex;
	UPROPERTY(VisibleAnywhere) int yIndex;

	UPROPERTY(VisibleAnywhere) bool bCanInteractWithTriggersConnection;

	//Needed a bool to tell if player was not in battle, but able to discern between PrimaryAction() presses
	UPROPERTY(VisibleAnywhere) bool bWeaponUnsheathed = false;

	//ITEMS
	UPROPERTY(VisibleAnywhere) bool bHoldingItem;
	UPROPERTY(VisibleAnywhere) AActor* holdingItemActor;

	//CONVERSATION
	UPROPERTY(VisibleAnywhere) AConversationInstance* connectedConversationInstance;
	UPROPERTY(VisibleAnywhere) bool bInConversation = false;
	FVector conversationWidgetFocus;

	//GUARDING & DODGING
	UPROPERTY(VisibleAnywhere) bool bGuardWindowActive = false;
	UPROPERTY(VisibleAnywhere) float guardWindowTimerMax;
	UPROPERTY(VisibleAnywhere) float currentGuardWindowTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bGameOver = false;
};
