// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleInstance.h"
#include "Components/BoxComponent.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "VagrantTacticsGameModeBase.h"
#include "BattleGrid.h"
#include "PlayerUnit.h"
#include "Blueprint/UserWidget.h"
#include "ConversationInstance.h"
#include "SpeechWidget.h"
#include "GameStatics.h"
#include "Intuition.h"

ABattleInstance::ABattleInstance()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABattleInstance::BeginPlay()
{
	Super::BeginPlay();
	
	box = FindComponentByClass<UBoxComponent>();
	box->OnComponentBeginOverlap.AddDynamic(this, &ABattleInstance::ActivateBattleOnOverlap);

	//Populate unit array
	unitsToActivateOnBattleStart.Empty();

	TArray<AActor*> outUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), outUnits);
	for (AActor* actor : outUnits)
	{
		unitsToActivateOnBattleStart.Add(Cast<AUnit>(actor));
	}

	numOfUnitsAlive = unitsToActivateOnBattleStart.Num();
}

void ABattleInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//End battle if all units destroyed
	if(numOfUnitsAlive <= 0)
	{
		AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		gameMode->activeBattleGrid->ActivateBattle();

		//Remove enemy turn order widget from viewport
		APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (player)
		{
			if (player->widgetEnemyTurnOrder->IsInViewport())
			{
				player->widgetEnemyTurnOrder->RemoveFromViewport();
			}

			//Make player say shit based on intuitions held on battle end
			UMainGameInstance* gameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

			if (speechOnBattleEnd.Num() > 0)
			{
				for (UIntuition* intuition : gameInstance->intuitions)
				{
					for (FIntuitionDialogue& dialogue : speechOnBattleEnd)
					{
						if (intuition->intuitonID == dialogue.intuitionID)
						{
							player->PlayerThought(&dialogue.dialogueLine);
							goto EndThought;
						}
					}
				}

				//Else just play default dialogue line (denoted by no ID)
				for (FIntuitionDialogue& dialogue : speechOnBattleEnd)
				{
					if (dialogue.intuitionID == TEXT(""))
					{
						player->PlayerThought(&dialogue.dialogueLine);
						break;
					}
				}
			}
			
			EndThought:

			//Give player intuition at end of battle
			if (intuitionToGainOnBattleEnd)
			{
				UIntuition* battleEndIntuition = NewObject<UIntuition>(gameInstance, intuitionToGainOnBattleEnd);
				GameStatics::GetPlayer(GetWorld())->AddIntuition(battleEndIntuition);
			}
		}

		Destroy();
	}
}

void ABattleInstance::ActivateBattleOnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Check for dialogue at start of battle
	if (conversationInstanceToActivateOnOverlap)
	{
		APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		conversationInstanceToActivateOnOverlap->battleInstanceToActivateOnEnd = this;
		conversationInstanceToActivateOnOverlap->ShowNextDialogueLineOnTimer();
		player->connectedConversationInstance = conversationInstanceToActivateOnOverlap;
		return;
	}

	AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	gameMode->activeBattleGrid->ActivateBattle();

	for (int i = 0; i < unitsToActivateOnBattleStart.Num(); i++)
	{
		unitsToActivateOnBattleStart[i]->bInBattle = true;
	}

	//Add enemy turn order widget to viewport
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player)
	{
		player->widgetEnemyTurnOrder = CreateWidget<UUserWidget>(GetWorld(), player->classEnemyTurnOrderWidget);
		player->widgetEnemyTurnOrder->AddToViewport();

		player->meshSword->SetVisibility(true);
	}

	box->DestroyComponent();
}
