// Fill out your copyright notice in the Description page of Project Settings.

#include "Intuition.h"

void UIntuition::AddIntuition()
{

}

UIntuition::~UIntuition()
{
	UE_LOG(LogTemp, Warning, TEXT("%s deleted."), *GetName());
}