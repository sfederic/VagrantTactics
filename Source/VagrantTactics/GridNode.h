#pragma once

#include "GridNode.generated.h"

USTRUCT()
struct FGridNode
{
	GENERATED_BODY()

	FGridNode() {}
	FGridNode(int x, int y, int32 instancedMeshIndex_)
	{
		xIndex = x;
		yIndex = y;
		instancedMeshIndex = instancedMeshIndex_;
	}
	
	float GetFCost()
	{
		return gCost + hCost;
	}

	float gCost; //Distance from start node
	float hCost; //Distance to end node
	int xIndex;
	int yIndex;
	int32 instancedMeshIndex;
	bool bActive = true;
	bool bClosed;
};