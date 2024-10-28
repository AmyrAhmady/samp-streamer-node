/*
 * Copyright (C) 2024 AmyrAhmady (iAmir), Incognito
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../main.h"

#include "omp-node.hpp"
#include "../core.h"
#include "../utility.h"


OMPNODE_API(StreamerCheckpoint, Create, float x, float y, float z, float size, int worldid, int interiorid, int playerid, float streamdistance, int areaid, int priority)
{
	if (core->getData()->getGlobalMaxItems(STREAMER_TYPE_CP) == core->getData()->checkpoints.size())
	{
		return INVALID_STREAMER_ID;
	}
	int checkpointId = Item::Checkpoint::identifier.get();
	Item::SharedCheckpoint checkpoint(new Item::Checkpoint);
	checkpoint->amx = nullptr; // TODO must be checked if it's used anywhere
	checkpoint->checkpointId = checkpointId;
	checkpoint->inverseAreaChecking = false;
	checkpoint->originalComparableStreamDistance = -1.0f;
	checkpoint->positionOffset = Eigen::Vector3f::Zero();
	checkpoint->streamCallbacks = false;
	checkpoint->position = Eigen::Vector3f(x, y, z);
	checkpoint->size = size;
	Utility::addToContainer(checkpoint->worlds, worldid);
	Utility::addToContainer(checkpoint->interiors, interiorid);
	Utility::addToContainer(checkpoint->players, playerid);
	checkpoint->comparableStreamDistance = streamdistance < STREAMER_STATIC_DISTANCE_CUTOFF ? streamdistance : streamdistance * streamdistance;
	checkpoint->streamDistance = streamdistance;
	Utility::addToContainer(checkpoint->areas, areaid);
	checkpoint->priority = priority;
	core->getGrid()->addCheckpoint(checkpoint);
	core->getData()->checkpoints.insert(std::make_pair(checkpointId, checkpoint));

	int ret = checkpointId;
	API_RETURN(int ret);
}

OMPNODE_API(StreamerCheckpoint, Destroy, int checkpointid)
{
	bool ret = false;
	std::unordered_map<int, Item::SharedCheckpoint>::iterator c = core->getData()->checkpoints.find(checkpointid);
	if (c != core->getData()->checkpoints.end())
	{
		Utility::destroyCheckpoint(c);
		ret = true;
	}
	API_RETURN(bool ret);
}

OMPNODE_API(StreamerCheckpoint, IsValid, int checkpointid)
{
	bool ret = false;
	std::unordered_map<int, Item::SharedCheckpoint>::iterator c = core->getData()->checkpoints.find(checkpointid);
	if (c != core->getData()->checkpoints.end())
	{
		ret = true;
	}
	API_RETURN(bool ret);
}

/*

cell AMX_NATIVE_CALL Natives::IsPlayerInDynamicCP(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	std::unordered_map<int, Player>::iterator p = core->getData()->players.find(static_cast<int>(params[1]));
	if (p != core->getData()->players.end())
	{
		if (p->second.activeCheckpoint == static_cast<int>(params[2]))
		{
			return 1;
		}
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::GetPlayerVisibleDynamicCP(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	std::unordered_map<int, Player>::iterator p = core->getData()->players.find(static_cast<int>(params[1]));
	if (p != core->getData()->players.end())
	{
		return p->second.visibleCheckpoint;
	}
	return 0;
}

*/
