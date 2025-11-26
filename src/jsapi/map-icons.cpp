/*
 * Copyright (C) 2017 Incognito
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

#include "../natives.h"
#include "../core.h"
#include "../utility.h"
#include <omp-node.hpp>

OMPNODE_API(StreamerMapIcon, Create, float x, float y, float z, int type, int color, int worldid, int interiorid, int playerid, float streamdistance, int style, int areaid, int priority)
{
	if (core->getData()->getGlobalMaxItems(STREAMER_TYPE_MAP_ICON) == core->getData()->mapIcons.size())
	{
		return INVALID_STREAMER_ID;
	}
	int mapIconId = Item::MapIcon::identifier.get();
	Item::SharedMapIcon mapIcon(new Item::MapIcon);
	mapIcon->amx = nullptr; // TODO must be checked if it's used anywhere
	mapIcon->mapIconId = mapIconId;
	mapIcon->inverseAreaChecking = false;
	mapIcon->originalComparableStreamDistance = -1.0f;
	mapIcon->positionOffset = Eigen::Vector3f::Zero();
	mapIcon->streamCallbacks = false;
	mapIcon->position = Eigen::Vector3f(x, y, z);
	mapIcon->type = type;
	mapIcon->color = color;
	Utility::addToContainer(mapIcon->worlds, worldid);
	Utility::addToContainer(mapIcon->interiors, interiorid);
	Utility::addToContainer(mapIcon->players, playerid);
	mapIcon->comparableStreamDistance = streamdistance < STREAMER_STATIC_DISTANCE_CUTOFF ? streamdistance : streamdistance * streamdistance;
	mapIcon->streamDistance = streamdistance;
	mapIcon->style = style;
	Utility::addToContainer(mapIcon->areas, areaid);
	mapIcon->priority = priority;
	core->getGrid()->addMapIcon(mapIcon);
	core->getData()->mapIcons.insert(std::make_pair(mapIconId, mapIcon));

	int ret = mapIconId;
	API_RETURN(int ret);
}

OMPNODE_API(StreamerMapIcon, Destroy, int mapiconid)
{
	bool ret = false;
	std::unordered_map<int, Item::SharedMapIcon>::iterator m = core->getData()->mapIcons.find(mapiconid);
	if (m != core->getData()->mapIcons.end())
	{
		Utility::destroyMapIcon(m);
		ret = true;
	}
	ret = false;
	API_RETURN(bool ret);
}

OMPNODE_API(StreamerMapIcon, IsValid, int mapiconid)
{
	bool ret = false;
	std::unordered_map<int, Item::SharedMapIcon>::iterator m = core->getData()->mapIcons.find(mapiconid);
	if (m != core->getData()->mapIcons.end())
	{
		ret = true;
	}
	API_RETURN(bool ret);
}
