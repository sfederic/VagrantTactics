// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridActor.h"
#include "GridNode.h"
#include "SkillBase.h"
#include "Unit.generated.h"

class UParticleSystemComponent;
class UParticleSystem;
class UCameraShake;
class USpellBase;

UENUM(BlueprintType)
enum class EUnitState : uint8
{
	Chase, //Persue either an attack target or target of interest
	Flee, //Move further away from target
	Wander //Moves to random node
};

//Base class for all units.
UCLASS()
class VAGRANTTACTICS_API AUnit : public AGridActor
{
	GENERATED_BODY()
	
public:	
	AUnit();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable) void ShowMovementPath();
	void HideMovementPath();
	void MoveTo(FGridNode* destinationNode);
	FGridNode* FindPlayerNode();
	FGridNode* FindTargetFocusNode();
	void FindPointOfInterest();
	UFUNCTION(BlueprintCallable) void ShowUnitFocus();
	void HideUnitFocus();
	bool Attack();
	void HighlightUnitOnSkillUse();
	USkillBase* CycleThroughAttackChoices(AActor* target);
	void FinishDisplayingSkill();
	void UseSkill();

	UPROPERTY(VisibleAnywhere) AActor* actorToFocusOn;

	UParticleSystemComponent* particleFocusBeam;
	UPROPERTY(EditAnywhere, Category="Particles") UParticleSystem* particleTemplateFocusBeam;

	UPROPERTY(EditAnywhere, Category="Camera") TSubclassOf<UCameraShake> cameraShakeAttack;

	TArray<FGridNode*> movementPathNodes; //Movement nodes are the preview nodes shown on click
	TArray<FGridNode*> pathNodes; //Path nodes are the final path the unit will take in Tick()
	TArray<FGridNode*> attackPathNodes; //Nodes that a charged attack will hit on next turn.

	UPROPERTY(EditAnywhere, Category = "Skills") TArray<TSubclassOf<USkillBase>> skillClasses;
	UPROPERTY(VisibleAnywhere, Category = "Skills") TArray<USkillBase*> skills;
	USkillBase* activeSkill;

	UPROPERTY(EditAnywhere, Category = "Spells") TArray<TSubclassOf<USpellBase>> spellClasses;

	//Collection of tags that the unit can focus on per turn
	UPROPERTY(EditAnywhere) TArray<FName> focusTags;

	//AI state of unit
	UPROPERTY(EditAnywhere) EUnitState unitState;

	//For displaying enemy name on UI widgets
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText unitName;

	FVector nextMoveLocation;
	int movementPathNodeIndex = 0;

	UPROPERTY(EditAnywhere) float moveSpeed;

	UPROPERTY(EditAnywhere) int turnSpeed;

	UPROPERTY(EditAnywhere) int maxMovementPoints;
	UPROPERTY(VisibleAnywhere) int currentMovementPoints;

	UPROPERTY(EditAnywhere) int maxAttackDistancePoints;
	UPROPERTY(VisibleAnywhere) int currentAttackDistancePoints;

	UPROPERTY(EditAnywhere) int maxAttackPoints;
	UPROPERTY(VisibleAnywhere) int currentAttackPoints;

	UPROPERTY(VisibleAnywhere) bool bSetToMove = false;
	UPROPERTY(VisibleAnywhere) bool bCurrentlyMoving = false;
	UPROPERTY(VisibleAnywhere) bool bSetToUseSkill = false;
	UPROPERTY(VisibleAnywhere) bool bTurnFinished = false;
	UPROPERTY(VisibleAnywhere) bool bInBattle = false;

	//Whether the unit is sheidled or can be attacked from cardinal directions.
	UPROPERTY(EditAnywhere, Category = "Attack Directions") bool bFrontVulnerable = true;
	UPROPERTY(EditAnywhere, Category = "Attack Directions") bool bBackVulnerable = true;
	UPROPERTY(EditAnywhere, Category = "Attack Directions") bool bLeftVulnerable = true;
	UPROPERTY(EditAnywhere, Category = "Attack Directions") bool bRightVulnerable = true;

	UPROPERTY(VisibleAnywhere) bool bChargingSkill;
	UPROPERTY(EditAnywhere) bool bCanEnterBattle = true;
};
