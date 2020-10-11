#pragma once

#include "Math/Vector.h"
#include "GridNode.generated.h"

USTRUCT()
struct FGridNode
{
	GENERATED_BODY()

	~FGridNode()
	{
		GLog->Logf(TEXT("deleted early"));
	}
	FGridNode() {}
	FGridNode(int x, int y, int32 instancedMeshIndex_)
	{
		xIndex = x;
		yIndex = y;
		instancedMeshIndex = instancedMeshIndex_;

		location = FVector((float)x * 100.f, (float)y * 100.f, 0.f);
	}
	
	float GetFCost()
	{
		return gCost + hCost;
	}

	void ResetValues()
	{
		gCost = 0.f;
		hCost = 0.f;
		parentNode = nullptr;
		bClosed = false;
	}

	FGridNode* parentNode;
	FVector location;

	float gCost; //Distance from start node
	float hCost; //Distance to end node
	UPROPERTY(VisibleAnywhere) int xIndex;
	UPROPERTY(VisibleAnywhere) int yIndex;
	int32 instancedMeshIndex;
	bool bActive;
	bool bClosed;
};