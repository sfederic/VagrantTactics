// Fill out your copyright notice in the Description page of Project Settings.

#include "IntuitionComponent.h"

UIntuitionComponent::UIntuitionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UIntuitionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UIntuitionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
