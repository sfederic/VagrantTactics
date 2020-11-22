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
#include "Camera/CameraShake.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "EntranceTrigger.h"
#include "InteractWidget.h"
#include "InteractDetailsWidget.h"
#include "InteractTrigger.h"
#include "SpellBase.h"
#include "GameplayTags.h"
#include "GameFramework/PlayerStart.h" 
#include "MainGameInstance.h"
#include "TimerManager.h"
#include "Engine/StaticMeshSocket.h" 
#include "Blueprint/WidgetLayoutLibrary.h" 
#include "Components/BoxComponent.h"
#include "PickupItem.h"
#include "SpeechComponent.h"
#include "UnitSkillWidget.h"
#include "NPCUnit.h"
#include "ConversationInstance.h"
#include "GameStatics.h"
#include "Intuition.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "SpeechWidget.h"

APlayerUnit::APlayerUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerUnit::BeginPlay()
{
	Super::BeginPlay();

	//Get all intuitions on reset
	UMainGameInstance* mainInstance = GameStatics::GetMainInstance(GetWorld());
	intuitions = mainInstance->intuitions;
	mainInstance->bGameOver = false;
	bGameOver = false;

	currentStressPoints = mainInstance->playerStressPoints;

	//Opening camera fadein
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(1.f, 0.f, 1.f, FColor::Black, true, true);

	//Get previous level name for debugging purposes
	UMainGameInstance* gameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	previousLevelMovedFrom = gameInstance->previousLevelMovedFrom;

	//Setup components
	speechWidgetComponent = Cast<UWidgetComponent>(GetDefaultSubobjectByName(TEXT("WidgetSpeech")));
	speechWidget = Cast<USpeechWidget>(speechWidgetComponent->GetUserWidgetObject());

	//Setup meshes
	TArray<UActorComponent*> outMeshes;
	GetComponents(UStaticMeshComponent::StaticClass(), outMeshes);
	for (UActorComponent* component : outMeshes)
	{
		if (component->GetName() == TEXT("Mesh"))
		{
			mesh = Cast<UStaticMeshComponent>(component);
		}
		else if (component->GetName() == TEXT("Sword"))
		{
			meshSword = Cast<UStaticMeshComponent>(component);
			meshSword->SetVisibility(false);
		}
	}

	if (!mesh) { UE_LOG(LogTemp, Warning, TEXT("Main mesh component not set for player")); }
	if (!meshSword) { UE_LOG(LogTemp, Warning, TEXT("Main mesh component not set for player")); }

	//Setup player spawn point from level entrances
	//Needs to handle both AEntraceTriggers and APlayerStarts (player starts because of blocked off entraces)
	/*TArray<AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);
	for (AActor* spawnPoint : playerStarts)
	{
		APlayerStart* playerStart = Cast<APlayerStart>(spawnPoint);

		SetActorLocation(playerStart->GetActorLocation());
		SetActorRotation(playerStart->GetActorRotation());

		if (playerStart->PlayerStartTag == previousLevelMovedFrom)
		{
			SetActorLocation(playerStart->GetActorLocation());
			SetActorRotation(playerStart->GetActorRotation());
			break;
		}
	}*/
	
	TArray<AActor*> entraceTriggers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEntranceTrigger::StaticClass(), entraceTriggers);
	for (AActor* spawnPoint : entraceTriggers)
	{
		AEntranceTrigger* entraceTrigger = Cast<AEntranceTrigger>(spawnPoint);
		if (entraceTrigger->connectedLevel == previousLevelMovedFrom)
		{
			SetActorLocation(entraceTrigger->GetActorLocation());
			SetActorRotation(entraceTrigger->GetActorRotation());
			break;
		}
	}


	nextLocation = GetActorLocation();
	nextRotation = GetActorRotation();

	currentActionPoints = maxActionPoints;
	currentHealthPoints = maxHealthPoints;

	//Components
	APlayerController* controller = Cast<APlayerController>(GetController());
	if (controller)
	{
		controller->bShowMouseCursor = true;
	}

	//Main camera setup
	camera = FindComponentByClass<UCameraComponent>();
	cameraFocusRotation = camera->GetComponentRotation();
	currentCameraFOV = camera->FieldOfView;

	//TODO: gamemodes StartPlay() isn't called before APlayerUnit::BeginPlay() on build. Have to resort to GetAllActors for now
	/*AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode)
	{
		battleGrid = gameMode->activeBattleGrid;
	}*/
	TArray<AActor*> battleGridOut;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattleGrid::StaticClass(), battleGridOut);
	battleGrid = Cast<ABattleGrid>(battleGridOut[0]);

	//Create Widgets
	widgetActionPoints = CreateWidget<UUserWidget>(GetWorld(), classWidgetActionPoints);

	widgetInteract = CreateWidget<UInteractWidget>(GetWorld(), classWidgetInteract);

	widgetInteractDetails = CreateWidget<UInteractDetailsWidget>(GetWorld(), classWidgetInteractDetails);

	timeOfDayWidget = CreateWidget<UUserWidget>(GetWorld(), classTimeOfDayWidget);
	timeOfDayWidget->AddToViewport();

	widgetUnitSkill = CreateWidget<UUnitSkillWidget>(GetWorld(), classUnitSkillWidget);

	widgetGuard = CreateWidget<UUserWidget>(GetWorld(), classGuardWidget);

	widgetDebugControls = CreateWidget<UUserWidget>(GetWorld(), classDebugControlsWidget);
	widgetDebugControls->AddToViewport();
}

void APlayerUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Set grid indices
	xIndex = FMath::RoundToInt(nextLocation.X / LevelGridValues::gridUnitDistance);
	yIndex = FMath::RoundToInt(nextLocation.Y / LevelGridValues::gridUnitDistance);

	//Check for death
	if (currentHealthPoints <= 0)
	{
		FTimerHandle timerHandle;
		widgetDeath = CreateWidget<UUserWidget>(GetWorld(), classDeathWidget);
		widgetDeath->AddToViewport();
		GetWorldTimerManager().SetTimer(timerHandle, this, &APlayerUnit::WorldReset, 3.0f, false);
		this->SetActorTickEnabled(false);
	}

	//Guard window input
	if (bGuardWindowActive)
	{
		if (currentGuardWindowTimer < guardWindowTimerMax)
		{
			currentGuardWindowTimer += DeltaTime;
		}
		else if (currentGuardWindowTimer > guardWindowTimerMax)
		{
			ResetGuardWindow();
		}
	}

	//Set different move speeds for battle and exploration
	float moveSpeed;
	if (battleGrid->bBattleActive)
	{
		moveSpeed = moveSpeedDuringBattle;
	}
	else if (!battleGrid->bBattleActive)
	{
		moveSpeed = moveSpeedOutsideBattle;
	}

	//Lerp movement and rotation
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), nextLocation, DeltaTime, moveSpeed));
	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), nextRotation, DeltaTime, rotateSpeed));

	//CAMERA
	if (selectedUnit)
	{
		if (selectedUnit->ActorHasTag(GameplayTags::Speech))
		{
			//TODO: that Find() is expensive in a tick here
			//cameraFocusRotation = UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(),
			//	selectedUnit->FindComponentByClass<UWidgetComponent>()->GetComponentLocation());
			//currentCameraFOV = cameraFOVAttack;
		}
		else
		{
			cameraFocusRotation = UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(), selectedUnit->GetActorLocation());
		}
	}
	else if (connectedConversationInstance)
	{
		cameraFocusRotation = UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(), conversationWidgetFocus);
	}
	else
	{
		cameraFocusRotation = UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(), GetActorLocation());
	}
	
	camera->SetWorldRotation(FMath::RInterpTo(camera->GetComponentRotation(), cameraFocusRotation, DeltaTime, cameraFocusLerpSpeed));
	camera->SetFieldOfView(FMath::FInterpTo(camera->FieldOfView, currentCameraFOV, DeltaTime, cameraFOVLerpSpeed));

	//Interact trigger direction check - Check if player's forward is same as Triggers (way of getting around directional focus)
	if (overlappedInteractTrigger)
	{
		if (mesh->GetForwardVector().Equals(overlappedInteractTrigger->GetActorForwardVector()))
		{
			bCanInteractWithTriggersConnection = true;
			if (!widgetInteract->IsInViewport()) { widgetInteract->AddToViewport(); }
			widgetInteract->interactText = overlappedInteractTrigger->interactText;
		}
	}
	else
	{
		bCanInteractWithTriggersConnection = false;
		//if (widgetInteract->IsInViewport()) { widgetInteract->RemoveFromViewport(); }
	}
}

void APlayerUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Movement for battle (player movement is 1 node per unit)
	InputComponent->BindAction(TEXT("Forward"), EInputEvent::IE_Pressed, this, &APlayerUnit::MoveForward);
	InputComponent->BindAction(TEXT("Back"), EInputEvent::IE_Pressed, this, &APlayerUnit::MoveBack);
	InputComponent->BindAction(TEXT("Left"), EInputEvent::IE_Pressed, this, &APlayerUnit::MoveLeft);
	InputComponent->BindAction(TEXT("Right"), EInputEvent::IE_Pressed, this, &APlayerUnit::MoveRight);

	//Movement outside of battle (player input is continuous)
	InputComponent->BindAxis(TEXT("MoveForwardHold"), this, &APlayerUnit::MoveForwardHold);
	InputComponent->BindAxis(TEXT("MoveBackHold"), this, &APlayerUnit::MoveBackHold);
	InputComponent->BindAxis(TEXT("MoveRightHold"), this, &APlayerUnit::MoveRightHold);
	InputComponent->BindAxis(TEXT("MoveLeftHold"), this, &APlayerUnit::MoveLeftHold);

	InputComponent->BindAction(TEXT("RotateLeft"), EInputEvent::IE_Pressed, this, &APlayerUnit::RotateLeft);
	InputComponent->BindAction(TEXT("RotateRight"), EInputEvent::IE_Pressed, this, &APlayerUnit::RotateRight);

	InputComponent->BindAction(TEXT("PrimaryAction"), EInputEvent::IE_Pressed, this, &APlayerUnit::PrimaryAction);
	InputComponent->BindAction(TEXT("SecondaryAction"), EInputEvent::IE_Pressed, this, &APlayerUnit::SecondaryAction);

	InputComponent->BindAction(TEXT("EndTurn"), EInputEvent::IE_Pressed, this, &APlayerUnit::EndTurn);
	InputComponent->BindAction(TEXT("Click"), EInputEvent::IE_Pressed, this, &APlayerUnit::Click);
	InputComponent->BindAction(TEXT("PreviewBattleGrid"), EInputEvent::IE_Pressed, this, &APlayerUnit::PreviewBattleGrid);
	InputComponent->BindAction(TEXT("Cancel"), EInputEvent::IE_Pressed, this, &APlayerUnit::Cancel);
	InputComponent->BindAction(TEXT("ShowIntuitions"), EInputEvent::IE_Pressed, this, &APlayerUnit::ShowIntuitions);

	//Spells
	InputComponent->BindAction(TEXT("Spell1"), EInputEvent::IE_Pressed, this, &APlayerUnit::ChangeSpellToIce);
	InputComponent->BindAction(TEXT("Spell2"), EInputEvent::IE_Pressed, this, &APlayerUnit::ChangeSpellToFire);

	//TODO: Debug input. Make sure to delete
	InputComponent->BindAction(TEXT("RefreshAP"), EInputEvent::IE_Pressed, this, &APlayerUnit::ResetActionPointsToMax);
}

void APlayerUnit::Move(FVector direction)
{
	if (bInConversation)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player in conversation. Cannot move."));
		return;
	}

	if (battleGrid->bBattleActive)
	{
		if (!battleGrid->bPlayerTurn)
		{
			if (bGuardWindowActive)
			{
				UE_LOG(LogTemp, Warning, TEXT("Player dodge!"));
				goto Move;
			}

			UE_LOG(LogTemp, Warning, TEXT("Not player turn. Cannot move."));
			return;
		}
	}

	Move:

	if (nextLocation.Equals(GetActorLocation()) && nextRotation.Equals(GetActorRotation()))
	{
		if (battleGrid->bBattleActive)
		{
			if (currentActionPoints < costToMove)
			{
				UE_LOG(LogTemp, Warning, TEXT("Not enough AP to move"));
				return;
			}
		}

		//Check for hit against fence or small walls between nodes
		FHitResult fenceHit;
		FCollisionQueryParams fenceParams;
		fenceParams.AddIgnoredActor(this);
		if (GetWorld()->LineTraceSingleByChannel(fenceHit, GetActorLocation(), GetActorLocation() + (direction * 100.f),
			ECC_WorldStatic, fenceParams))
		{
			if(fenceHit.GetActor())
			{
				if (fenceHit.GetActor()->ActorHasTag(GameplayTags::Fence))
				{
					UE_LOG(LogTemp, Warning, TEXT("fence or small wall hit"));
					return;
				}
			}
		}

		currentCameraFOV = maxCameraFOV;

		nextLocation += (direction * moveDistance);

		//This is here to giev the player dodge a more visual effect instead of the usual walk direction effect
		if (!bGuardWindowActive)
		{
			mesh->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), nextLocation));
		}

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

		//Check next node height
		if (nextNodeToMoveTo->location.Z >= (GetActorLocation().Z + 100.f))
		{
			UE_LOG(LogTemp, Warning, TEXT("Node to move to too high."));
			nextLocation = GetActorLocation();
			return;
		}
		else if (!nextNodeToMoveTo->bActive)
		{
			nextLocation = GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Player can't move into inactive node"));
			return;
		}
		else
		{
			nextLocation = nextNodeToMoveTo->location;

			if (battleGrid->bBattleActive)
			{
				currentActionPoints -= costToMove;
			}
		}
	}
}

void APlayerUnit::MoveForward()
{
	if (battleGrid->bBattleActive)
	{
		Move(GetActorForwardVector());
	}
}

void APlayerUnit::MoveBack()
{
	if (battleGrid->bBattleActive)
	{
		Move(-GetActorForwardVector());
	}
}

void APlayerUnit::MoveLeft()
{
	if (battleGrid->bBattleActive)
	{
		Move(-GetActorRightVector());
	}
}

void APlayerUnit::MoveRight()
{
	if (battleGrid->bBattleActive)
	{
		Move(GetActorRightVector());
	}
}

void APlayerUnit::MoveForwardHold(float val)
{
	if (!battleGrid->bBattleActive && val)
	{
		Move(GetActorForwardVector());
	}
}

void APlayerUnit::MoveBackHold(float val)
{
	if (!battleGrid->bBattleActive && val)
	{
		Move(-GetActorForwardVector());
	}
}

void APlayerUnit::MoveRightHold(float val)
{
	if (!battleGrid->bBattleActive && val)
	{
		Move(GetActorRightVector());
	}
}

void APlayerUnit::MoveLeftHold(float val)
{
	if (!battleGrid->bBattleActive && val)
	{
		Move(-GetActorRightVector());
	}
}

void APlayerUnit::RotateLeft()
{
	Rotate(-90.f);
}

void APlayerUnit::RotateRight()
{
	Rotate(90.f);
}

void APlayerUnit::Rotate(float angle)
{
	if (bInConversation)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot turn. Player in conversation."));
		return;
	}

	if (!battleGrid->bPlayerTurn)
	{
		if (bGuardWindowActive)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player rotate dodge!"));
			ResetGuardWindow();
			goto Turn;
		}

		UE_LOG(LogTemp, Warning, TEXT("Cannot turn. Not Player turn."));
		return;
	}

Turn:

	if (nextLocation.Equals(GetActorLocation()) && nextRotation.Equals(GetActorRotation()))
	{
		nextRotation.Yaw += angle;
	}
}

void APlayerUnit::PrimaryAction()
{
	if (overlappedSavePoint)
	{

	}

	//Attack target if weapon unsheathed and not in battle
	if (bWeaponUnsheathed)
	{
		//TODO: Player can't go through doors if weapon is out
		goto Attack;
	}

	if (connectedConversationInstance)
	{
		connectedConversationInstance->ShowNextDialogueLineOnPlayerInput();
		return;
	}

	//Talk to NPC
	if(!battleGrid->bBattleActive && !bWeaponUnsheathed)
	{
		FHitResult talkHit;
		FCollisionQueryParams talkParams;
		talkParams.AddIgnoredActor(this);
		if (GetWorld()->LineTraceSingleByChannel(talkHit, GetActorLocation(), GetActorLocation() + (mesh->GetForwardVector() * 150.f),
			ECC_WorldStatic, talkParams))
		{
			AActor* talkHitActor = talkHit.GetActor();
			if (talkHitActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("hit talk actor : %s"), *talkHitActor->GetName());

				ANPCUnit* npc = Cast<ANPCUnit>(talkHitActor);
				if (npc)
				{
					if (npc->conversationInstance)
					{
						if (npc->conversationInstance->bIsPlayerConnected)
						{
							npc->conversationInstance->ShowNextDialogueLineOnPlayerInput();
							connectedConversationInstance = npc->conversationInstance;
							bInConversation = true;
							npc->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(npc->GetActorLocation(), this->GetActorLocation()));
							currentCameraFOV = cameraFOVConversation;
							return;
						}
					}
				}
				else
				{
					USpeechComponent* sc = talkHitActor->FindComponentByClass<USpeechComponent>();
					if (sc)
					{
						sc->ShowDialogue(false);
						return;
					}
				}
			}
		}
	}

	if (bInConversation)
	{
		return;
	}

	//Open doors
	if (overlappedEntrace)
	{
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.f, 1.f, 1.f, FColor::Black, true, true);
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, this, &APlayerUnit::MoveToLevel, 2.0f, false);
		APlayerController* controller = Cast<APlayerController>(GetController());
		DisableInput(controller);

		//TODO: This works roughly to remove all NPC dialogue while moving level, but new dialogue will still pop up
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

		//Keep the time of day widget to show time progression when camera fades between entrances
		timeOfDayWidget->AddToViewport();

		widgetInteract->RemoveFromViewport();

		//Change time of day
		UMainGameInstance* mainInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		mainInstance->ProgressTimeOfDay();

		return;
	}

	//Check for interaction
	if (overlappedInteractTrigger)
	{
		//Add entrace key to inventory
		if (overlappedInteractTrigger->keyToPickup != TEXT(""))
		{
			UMainGameInstance* gameInstance = GameStatics::GetMainInstance(GetWorld());
			gameInstance->entraceKeys.Add(overlappedInteractTrigger->keyToPickup);
			UE_LOG(LogTemp, Warning, TEXT("Key: %s added to inventory."), *overlappedInteractTrigger->keyToPickup.ToString());
			return;
		}

		if (overlappedInteractTrigger->intuitionToGain)
		{
			AddIntuition(overlappedInteractTrigger->intuitionToGain);
		}

		if (bCanInteractWithTriggersConnection)
		{
			//interact widgets
			widgetInteract->RemoveFromViewport();
			widgetInteractDetails->detailsText = overlappedInteractTrigger->detailsText;
			if (!widgetInteractDetails->IsInViewport())
			{
				widgetInteractDetails->AddToViewport();
			}

			//Zoom in on inspected object
			currentCameraFOV = cameraFOVAttack;
			selectedUnit = overlappedInteractTrigger->connectedActor;

			//Handle items to add to player's hand during combat
			if (overlappedInteractTrigger->bPickupConnectedActor)
			{
				const UStaticMeshSocket* socket = mesh->GetSocketByName(TEXT("LH_Item"));
				if (socket)
				{
					socket->AttachActor(overlappedInteractTrigger->connectedActor, mesh);
					holdingItemActor = overlappedInteractTrigger->connectedActor;
					overlappedInteractTrigger->Destroy();

					selectedUnit = nullptr;
					bHoldingItem = true;
					return;
				}
			}
		}
	}

	//Deal with pikcup item that is on battlegrid
	if (overlappedPickupItem)
	{
		const UStaticMeshSocket* socket = mesh->GetSocketByName(TEXT("LH_Item"));
		if (socket)
		{
			overlappedPickupItem->box->Deactivate();

			//Upcasting here to AActor removes its "class" 
			socket->AttachActor((AActor*)overlappedPickupItem, mesh);
			holdingItemActor = (AActor*)overlappedPickupItem;

			selectedUnit = nullptr;
			bHoldingItem = true;

			if (widgetInteract->IsInViewport())
			{
				widgetInteract->RemoveFromViewport();
			}

			return;
		}

	}

	if (battleGrid->bBattleActive)
	{
		if (!battleGrid->bPlayerTurn)
		{
			UE_LOG(LogTemp, Warning, TEXT("Not player turn. Cannot move."));
			return;
		}

		if (currentActionPoints < costToAttack)
		{
			UE_LOG(LogTemp, Warning, TEXT("Not enough AP to attack"));
			return;
		}
	}

	Attack:

	if (nextLocation.Equals(GetActorLocation()) && nextRotation.Equals(GetActorRotation()))
	{
		FHitResult hit;
		FCollisionQueryParams hitParams;
		hitParams.AddIgnoredActor(this);
		FVector endHit = GetActorLocation() + (mesh->GetForwardVector() * moveDistance);

		//TODO: This trace is junk. need to find a way to attack based on node and mesh directiom
		if (GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), endHit, ECC_WorldStatic, hitParams))
		{
			AGridActor* gridActor = Cast<AGridActor>(hit.GetActor());
			if (gridActor)
			{
				//if (battleGrid->bBattleActive)
				{
					//Deal damage and stress to unit
					gridActor->currentHealth -= attackPoints;
					currentActionPoints -= costToAttack;
					AUnit* unit = Cast<AUnit>(gridActor);
					if(unit)
					{
						//TODO: don't know if player will be able to deal more stress damage somehow
						if (unit->currentStressPoints < unit->maxStressPoints)
						{
							unit->currentStressPoints += 1;
							if (unit->currentStressPoints >= unit->maxStressPoints)
							{
								//TODO: add an interface here
								unit->ActivateStress();
							}
						}
					}

					currentCameraFOV = cameraFOVAttack;

					UGameplayStatics::PlayWorldCameraShake(GetWorld(), cameraShakeAttack, camera->GetComponentLocation(), 5.0f, 5.0f);
				}

				AUnit* unit = Cast<AUnit>(gridActor);
				if (unit)
				{
					selectedUnit = unit;

					unit->FindComponentByClass<UWidgetComponent>()->SetHiddenInGame(false);

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

				//Case for initiating combat with NPCs outside of battle
				ANPCUnit* npcToStartCombatWith = Cast<ANPCUnit>(gridActor);
				if(npcToStartCombatWith)
				{
					npcToStartCombatWith->ActivateForBattle();
				}
			}
		}
	}
}

void APlayerUnit::SecondaryAction()
{
	//GUARD
	if (bGuardWindowActive)
	{
		currentActionPoints -= 20;

		guardPoints = 2;

		widgetGuard->RemoveFromViewport();

		bGuardWindowActive = false;
		currentGuardWindowTimer = 0.f;
	}
}

void APlayerUnit::EndTurn()
{
	battleGrid->ChangeTurn();
}

void APlayerUnit::Click()
{
	APlayerController* controller = Cast<APlayerController>(GetController());
	FHitResult hit;

	//GameTraceChannel1 = AGridActor's
	if (controller->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), true, hit))
	{
		if (hit.GetActor() == nullptr)
		{
			return;
		}


		UE_LOG(LogTemp, Warning, TEXT("Clicked Actor: %s | Index: %d"), *hit.GetActor()->GetName(), hit.Item);


		//Testing throw item at clicked node
		if (bHoldingItem)
		{
			holdingItemActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			holdingItemActor->SetActorLocation(battleGrid->nodeMap.Find(hit.Item)->location);

			bHoldingItem = false;
			overlappedPickupItem = nullptr;
			holdingItemActor = nullptr;
			return;
		}


		if (hit.GetActor()->IsA<AGridActor>())
		{
			selectedUnit = Cast<AGridActor>(hit.GetActor());

			if (activeSpell)
			{
				auto spell = NewObject<USpellBase>(this, activeSpell);
				ISpellInterface* spellInterface = Cast<ISpellInterface>(spell);
				FGridNode* hitNode = battleGrid->nodeMap.Find(hit.Item);
				spellInterface->CastSpell(hitNode->xIndex, hitNode->yIndex, Cast<AGridActor>(hit.GetActor()));
			}
		}

		AUnit* unit = Cast<AUnit>(hit.GetActor());
		if (unit)
		{
			unit->ShowUnitFocus();
			unit->ShowMovementPath();

			battleGrid->gridMesh->SetHiddenInGame(false);

			//Hide previous health bar widget
			if (selectedUnit)
			{
				if (selectedUnit != unit)
				{
					selectedUnit->FindComponentByClass<UWidgetComponent>()->SetHiddenInGame(true);
				}
			}

			selectedUnit = unit;
			unit->FindComponentByClass<UWidgetComponent>()->SetHiddenInGame(false);
		}

		//Node clicked
		if (hit.Item > 0) //-1 is the index for non instanced meshes
		{
			if (activeSpell)
			{
				auto spell = NewObject<USpellBase>(this, activeSpell);
				ISpellInterface* spellInterface = Cast<ISpellInterface>(spell);
				FGridNode* hitNode = battleGrid->nodeMap.Find(hit.Item);
				spellInterface->CastSpell(hitNode->xIndex, hitNode->yIndex, Cast<AGridActor>(hit.GetActor()));
			}
		}

		//Handle generic actors in level to zoom onto and inspect from afar.
		if (hit.GetActor()->IsA<AGridActor>())
		{
			selectedUnit = Cast<AGridActor>(hit.GetActor());
			currentCameraFOV = cameraFOVAttack;
		}
	}
}

void APlayerUnit::PreviewBattleGrid()
{
	AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode->activeBattleGrid->gridMesh->bHiddenInGame)
	{
		bWeaponUnsheathed = true;
		meshSword->SetVisibility(true);
		gameMode->activeBattleGrid->ToggleGridOn();
	}
	else
	{
		bWeaponUnsheathed = false;
		meshSword->SetVisibility(false);
		gameMode->activeBattleGrid->ToggleGridOff();
	}
}

void APlayerUnit::ResetActionPointsToMax()
{
	currentActionPoints = maxActionPoints;
}

//For now just resets camera focus
void APlayerUnit::Cancel()
{
	if (selectedUnit)
	{
		AGridActor* gridActorToCancel = Cast<AGridActor>(selectedUnit);
		if(gridActorToCancel)
		{
			if (gridActorToCancel->healthbarWidgetComponent)
			{
				selectedUnit->FindComponentByClass<UWidgetComponent>()->SetHiddenInGame(true);
			}
		}

		AUnit* unit = Cast<AUnit>(selectedUnit);
		if (unit)
		{
			unit->HideUnitFocus();
			unit->HideMovementPath();
		}
	}

	selectedUnit = nullptr;
	currentCameraFOV = maxCameraFOV;

	if (widgetInteractDetails->IsInViewport()) { widgetInteractDetails->RemoveFromViewport(); }
}

void APlayerUnit::ShowIntuitions()
{
	if (intuitionsWidget == nullptr)
	{
		intuitionsWidget = CreateWidget<UUserWidget>(GetWorld(), classIntuitionsWidget);
		intuitionsWidget->AddToViewport();
	}
	else
	{
		intuitionsWidget->RemoveFromViewport();
		intuitionsWidget = nullptr;
	}
}

void APlayerUnit::ResetCameraFocusAndFOV()
{
	selectedUnit = nullptr;
	currentCameraFOV = maxCameraFOV;
}

void APlayerUnit::ChangeSpellToIce()
{
	activeSpell = spells[0];
}

void APlayerUnit::ChangeSpellToFire()
{
	activeSpell = spells[1];
}

void APlayerUnit::MoveToLevel()
{
	UMainGameInstance* gameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	gameInstance->previousLevelMovedFrom = *UGameplayStatics::GetCurrentLevelName(GetWorld());
	previousLevelMovedFrom = gameInstance->previousLevelMovedFrom;
	UGameplayStatics::OpenLevel(GetWorld(), overlappedEntrace->connectedLevel);
}

void APlayerUnit::ZoomInOnTarget(AActor* target)
{
	selectedUnit = target;
	currentCameraFOV = cameraFOVAttack;
}

//Keep in mind that window here means 'period of time', not anything widget related
void APlayerUnit::ResetGuardWindow()
{
	bGuardWindowActive = false;
	currentGuardWindowTimer = 0.f;
	widgetGuard->RemoveFromViewport();
}

void APlayerUnit::ActivateGuardWindow(float windUpTime)
{
	bGuardWindowActive = true;
	guardWindowTimerMax = windUpTime;
	widgetGuard->AddToViewport();
}

//Reset world on player death or time of day end
void APlayerUnit::WorldReset()
{
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

	bGameOver = true;

	widgetIntuitionTransfer = CreateWidget<UUserWidget>(GetWorld(), classIntuitionTransfer);
	widgetIntuitionTransfer->AddToViewport();
}

void APlayerUnit::AddIntuition(TSubclassOf<UIntuition> intuitionClass)
{
	UMainGameInstance* gameInstance = GameStatics::GetMainInstance(GetWorld());
	UIntuition* intuitionToAdd = NewObject<UIntuition>(gameInstance, intuitionClass);

	for (UIntuition* intuition : gameInstance->intuitions)
	{
		if (intuition->intuitonID == intuitionToAdd->intuitonID)
		{
			UE_LOG(LogTemp, Warning, TEXT("Intuition %s already in game instance."), *intuitionToAdd->GetName());
			return;
		}
	}

	gameInstance->intuitions.Add(intuitionToAdd);
	intuitions.Add(intuitionToAdd);
	GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, TEXT("Intuition added"));
	UE_LOG(LogTemp, Warning, TEXT("Intuition %s added"), *intuitionToAdd->GetName());
}

void APlayerUnit::AddIntuition(UIntuition* intuitionToAdd)
{
	UMainGameInstance* gameInstance = GameStatics::GetMainInstance(GetWorld());

	for (UIntuition* intuition : gameInstance->intuitions)
	{
		if (intuition->intuitonID == intuitionToAdd->intuitonID)
		{
			UE_LOG(LogTemp, Warning, TEXT("Intuition %s already in game instance."), *intuitionToAdd->GetName());
			return;
		}
	}

	gameInstance->intuitions.Add(intuitionToAdd);
	intuitions.Add(intuitionToAdd);
	GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, TEXT("Intuition added"));
	UE_LOG(LogTemp, Warning, TEXT("Intuition %s added"), *intuitionToAdd->GetName());
}

void APlayerUnit::AddStress(int stressPoints, ANPCUnit* npc)
{
	currentStressPoints += stressPoints;
	if (currentStressPoints >= maxStressPoints)
	{
		FTimerHandle handle;
		GetWorldTimerManager().SetTimer(handle, this, &APlayerUnit::PlayerThoughtEnd, 3.0f, false);
		speechWidgetComponent->SetHiddenInGame(false);
		speechWidget->dialogueLine = npc->playerDialogueOnDeath;
	}
}

//Set speech widget only for player on a timer
void APlayerUnit::PlayerThoughtEnd()
{
	speechWidgetComponent->SetHiddenInGame(true);
	speechWidget->dialogueLine = FText::FromString("");
}
