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

OMPNODE_API(StreamerPickup, Create, int modelid, int type, float x, float y, float z, int worldid, int interiorid, int playerid, float streamdistance, int areaid, int priority)
{
	if (core->getData()->getGlobalMaxItems(STREAMER_TYPE_PICKUP) == core->getData()->pickups.size())
	{
		return INVALID_STREAMER_ID;
	}
	int pickupId = Item::Pickup::identifier.get();
	Item::SharedPickup pickup(new Item::Pickup);
	pickup->amx = nullptr; // TODO must be checked if it's used anywhere
	pickup->pickupId = pickupId;
	pickup->inverseAreaChecking = false;
	pickup->originalComparableStreamDistance = -1.0f;
	pickup->positionOffset = Eigen::Vector3f::Zero();
	pickup->streamCallbacks = false;
	pickup->modelId = modelid;
	pickup->type = type;
	pickup->position = Eigen::Vector3f(x, y, z);
	Utility::addToContainer(pickup->worlds, worldid);
	Utility::addToContainer(pickup->interiors, interiorid);
	Utility::addToContainer(pickup->players, playerid);
	pickup->comparableStreamDistance = streamdistance < STREAMER_STATIC_DISTANCE_CUTOFF ? streamdistance : streamdistance * streamdistance;
	pickup->streamDistance = streamdistance;
	Utility::addToContainer(pickup->areas, areaid);
	pickup->priority = priority;
	core->getGrid()->addPickup(pickup);
	core->getData()->pickups.insert(std::make_pair(pickupId, pickup));

	int ret = pickupId;
	API_RETURN(int ret);
}

OMPNODE_API(StreamerPickup, Destroy, int pickupid)
{
	bool ret = false;
	std::unordered_map<int, Item::SharedPickup>::iterator p = core->getData()->pickups.find(pickupid);
	if (p != core->getData()->pickups.end())
	{
		Utility::destroyPickup(p);
		ret = true;
	}
	API_RETURN(bool ret);
}

OMPNODE_API(StreamerPickup, IsValid, int pickupid)
{
	bool ret = false;
	std::unordered_map<int, Item::SharedPickup>::iterator p = core->getData()->pickups.find(pickupid);
	if (p != core->getData()->pickups.end())
	{
		ret = true;
	}
	API_RETURN(bool ret);
}
