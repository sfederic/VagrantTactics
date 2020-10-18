// Fill out your copyright notice in the Description page of Project Settings.

#include "SpellFire.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "GridActor.h"

void USpellFire::CastSpell(int x, int y, AGridActor* target)
{
	if (target)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), particleFire, target->GetActorLocation());
	}
}
