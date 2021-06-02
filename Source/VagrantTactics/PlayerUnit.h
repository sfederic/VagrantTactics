// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerUnit.generated.h"

class UCameraComponent;
class ABattleGrid;
class AUnit;
class ANPCUnit;
class UCameraShakeBase;
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
class UWidgetComponent;
class USpeechWidget;

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
	void PrimaryAction(); //Talking, attacking, using
	void SecondaryAction(); //Defending, canceling
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
	void AddIntuition(TSubclassOf<UIntuition> intuitionClass);
	void AddIntuition(UIntuition* intuitionToAdd);
	void AddStress(int stressPoints, ANPCUnit* npc);
	void PlayerThought(FText* text);
	void PlayerThoughtEnd();
	void IntuitionGainedWidgetHide();
		
	//PARTICLES
	UPROPERTY(EditAnywhere, Category="Particles") UParticleSystem* particleSystemFocus;

	//CAMERA
	UCameraComponent* camera;
	FRotator cameraFocusRotation;
	UPROPERTY(EditAnywhere, Category="Camera") float cameraFocusLerpSpeed;
	UPROPERTY(VisibleAnywhere, Category="Camera") float currentCameraFOV;
	UPROPERTY(EditAnywhere, Category="Camera") float cameraFOVLerpSpeed;
	UPROPERTY(EditAnywhere, Category="Camera") TSubclassOf<UCameraShakeBase> cameraShakeAttack;
	const float maxCameraFOV = 90.f;
	const float cameraFOVAttack = 45.f;
	const float cameraFOVConversation = 60.f;

	//WIDGETS
	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> classWidgetIntuitionGained;
	UPROPERTY() UUserWidget* widgetIntuitionGained;

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

	UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> classIntuitionTransfer;
	UPROPERTY() UUserWidget* widgetIntuitionTransfer;

	//WIDGET COMPONENTS
	UWidgetComponent* speechWidgetComponent;
	USpeechWidget* speechWidget;

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

	//DEBUG INTUITIONS (Both only for debug purposes, main intuition handling is in game isntance)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Intuitions") TArray<UIntuition*> intuitions;
	UPROPERTY(EditAnywhere, Category = "Intuitions") TArray<TSubclassOf<UIntuition>> intuitionsToSpawnWith;

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
	bool bGuardWindowActive = false;
	bool bIsGuarding;
	float guardWindowTimerMax;
	float currentGuardWindowTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bGameOver = false;

	//Whether player is reading, checking, opening something
	UPROPERTY(VisibleAnywhere) bool bInteracting = false;

	//STRESS
	UPROPERTY(EditAnywhere) int currentStressPoints;
	UPROPERTY(EditAnywhere) int maxStressPoints;
};
