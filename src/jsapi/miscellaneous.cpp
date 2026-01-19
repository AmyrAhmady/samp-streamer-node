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

#include "omp-node.hpp"
#include "../natives.h"
#include "../core.h"
#include "../utility.h"

OMPNODE_API(StreamerMisc, GetDistanceToItem, float x, float y, float z, int type, int id, int dimensions)
{
	bool success = false;
	float distance = 0.0f;

	Eigen::Vector3f position = Eigen::Vector3f::Zero();

	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			if (const auto o = core->getData()->objects.find(id); o != core->getData()->objects.end())
			{
				if (o->second->attach)
				{
					position = o->second->attach->position;
				}
				else
				{
					position = o->second->position;
				}
				break;
			}
			API_RETURN(bool success, float distance);
		}
		case STREAMER_TYPE_PICKUP:
		{
			if (const auto p = core->getData()->pickups.find(id); p != core->getData()->pickups.end())
			{
				position = p->second->position;
				break;
			}
			API_RETURN(bool success, float distance);
		}
		case STREAMER_TYPE_CP:
		{
			if (const auto c = core->getData()->checkpoints.find(id); c != core->getData()->checkpoints.end())
			{
				position = c->second->position;
				break;
			}
			API_RETURN(bool success, float distance);
		}
		case STREAMER_TYPE_RACE_CP:
		{
			if (const auto r = core->getData()->raceCheckpoints.find(id); r != core->getData()->raceCheckpoints.end())
			{
				position = r->second->position;
				break;
			}
			API_RETURN(bool success, float distance);
		}
		case STREAMER_TYPE_MAP_ICON:
		{
			if (const auto m = core->getData()->mapIcons.find(id); m != core->getData()->mapIcons.end())
			{
				position = m->second->position;
				break;
			}
			API_RETURN(bool success, float distance);
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			if (const auto t = core->getData()->textLabels.find(id); t != core->getData()->textLabels.end())
			{
				if (t->second->attach)
				{
					position = t->second->attach->position;
				}
				else
				{
					position = t->second->position;
				}
				break;
			}
			API_RETURN(bool success, float distance);
		}
		case STREAMER_TYPE_AREA:
		{
			if (const auto a = core->getData()->areas.find(id); a != core->getData()->areas.end())
			{
				std::variant<Polygon2d, Box2d, Box3d, Eigen::Vector2f, Eigen::Vector3f> areaPosition;
				if (a->second->attach)
				{
					areaPosition = a->second->attach->position;
				}
				else
				{
					areaPosition = a->second->position;
				}
				switch (a->second->type)
				{
					case STREAMER_AREA_TYPE_CIRCLE:
					case STREAMER_AREA_TYPE_CYLINDER:
					{
						distance = static_cast<float>(boost::geometry::distance(Eigen::Vector2f(x, y), std::get<Eigen::Vector2f>(areaPosition)));
						success = true;
						API_RETURN(bool success, float distance);
					}
					case STREAMER_AREA_TYPE_SPHERE:
					{
						position = std::get<Eigen::Vector3f>(areaPosition);
						success = true;
						break;
					}
					case STREAMER_AREA_TYPE_RECTANGLE:
					{
						auto centroid = boost::geometry::return_centroid<Eigen::Vector2f>(std::get<Box2d>(areaPosition));
						distance = static_cast<float>(boost::geometry::distance(Eigen::Vector2f(x, y), centroid));
						success = true;
						API_RETURN(bool success, float distance);
					}
					case STREAMER_AREA_TYPE_CUBOID:
					{
						position = boost::geometry::return_centroid<Eigen::Vector3f>(std::get<Box3d>(areaPosition));
						success = true;
						break;
					}
					case STREAMER_AREA_TYPE_POLYGON:
					{
						auto centroid = boost::geometry::return_centroid<Eigen::Vector2f>(std::get<Polygon2d>(areaPosition));
						distance = static_cast<float>(boost::geometry::distance(Eigen::Vector2f(x, y), centroid));
						success = true;
						API_RETURN(bool success, float distance);
					}
					default:
					{

					}
				}
			}
			if (success)
			{
				break;
			}
			API_RETURN(bool success, float distance);
		}
		case STREAMER_TYPE_ACTOR:
		{
			if (const auto a = core->getData()->actors.find(id); a != core->getData()->actors.end())
			{
				position = a->second->position;
				break;
			}
			API_RETURN(bool success, float distance);
		}
		default:
		{
			Utility::logError("Streamer_GetDistanceToItem: Invalid type specified.");
			API_RETURN(bool success, float distance);
		}
	}
	switch (dimensions)
	{
		case 2:
		{
			distance = static_cast<float>(boost::geometry::distance(Eigen::Vector2f(x, y), Eigen::Vector2f(position[0], position[1])));
			success = true;
			API_RETURN(bool success, float distance);
		}
		case 3:
		{
			distance = static_cast<float>(boost::geometry::distance(Eigen::Vector3f(x, y, z), position));
			success = true;
			API_RETURN(bool success, float distance);
		}
		default:
		{
			Utility::logError("Streamer_GetDistanceToItem: Invalid number of dimensions specified (outside range of 2-3).");
			API_RETURN(bool success, float distance);
		}
	}

	API_RETURN(bool success, float distance);
}

OMPNODE_API(StreamerMisc, ToggleItem, int playerId, int type, int id, int toggle) 
{
	bool success = false;

	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			if (const auto o = core->getData()->objects.find(id); o != core->getData()->objects.end())
			{
				if (!toggle)
				{
					success = Utility::removeFromContainer(o->second->players, playerId);
				}
				else
				{
					success = Utility::addToContainer(o->second->players, playerId);
				}
			}
			break;
		}
		case STREAMER_TYPE_PICKUP:
		{
			if (const auto p = core->getData()->pickups.find(id); p != core->getData()->pickups.end())
			{
				if (!toggle)
				{
					success = Utility::removeFromContainer(p->second->players, playerId);
				}
				else
				{
					success = Utility::addToContainer(p->second->players, playerId);
				}
			}
			break;
		}
		case STREAMER_TYPE_CP:
		{
			if (const auto c = core->getData()->checkpoints.find(id); c != core->getData()->checkpoints.end())
			{
				if (!toggle)
				{
					success = Utility::removeFromContainer(c->second->players, playerId);
				}
				else
				{
					success = Utility::addToContainer(c->second->players, playerId);
				}
			}
			break;
		}
		case STREAMER_TYPE_RACE_CP:
		{
			if (const auto r = core->getData()->raceCheckpoints.find(id); r != core->getData()->raceCheckpoints.end())
			{
				if (!toggle)
				{
					success = Utility::removeFromContainer(r->second->players, playerId);
				}
				else
				{
					success = Utility::addToContainer(r->second->players, playerId);
				}
			}
			break;
		}
		case STREAMER_TYPE_MAP_ICON:
		{
			if (const auto m = core->getData()->mapIcons.find(id); m != core->getData()->mapIcons.end())
			{
				if (!toggle)
				{
					success = Utility::removeFromContainer(m->second->players, playerId);
				}
				else
				{
					success = Utility::addToContainer(m->second->players, playerId);
				}
			}
			break;
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			if (const auto t = core->getData()->textLabels.find(id); t != core->getData()->textLabels.end())
			{
				if (!toggle)
				{
					success = Utility::removeFromContainer(t->second->players, playerId);
				}
				else
				{
					success = Utility::addToContainer(t->second->players, playerId);
				}
			}
			break;
		}
		case STREAMER_TYPE_AREA:
		{
			if (const auto a = core->getData()->areas.find(id); a != core->getData()->areas.end())
			{
				if (!toggle)
				{
					success = Utility::removeFromContainer(a->second->players, playerId);
				}
				else
				{
					success = Utility::addToContainer(a->second->players, playerId);
				}
			}
			break;
		}
		case STREAMER_TYPE_ACTOR:
		{
			if (const auto a = core->getData()->actors.find(id); a != core->getData()->actors.end())
			{
				if (!toggle)
				{
					success = Utility::removeFromContainer(a->second->players, playerId);
				}
				else
				{
					success = Utility::addToContainer(a->second->players, playerId);
				}
			}
			break;
		}
		default:
		{
			Utility::logError("Streamer_ToggleItem: Invalid type specified.");
			break;
		}
	}
	
	API_RETURN(bool success);
}

OMPNODE_API(StreamerMisc, IsToggleItem, int playerId, int type, int id)
{
	bool success = false;

	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			if (const auto o = core->getData()->objects.find(id); o != core->getData()->objects.end())
			{
				success = Utility::isInContainer(o->second->players, playerId);
			}
			break;
		}
		case STREAMER_TYPE_PICKUP:
		{
			if (const auto p = core->getData()->pickups.find(id); p != core->getData()->pickups.end())
			{
				success = Utility::isInContainer(p->second->players, playerId);
			}
			break;
		}
		case STREAMER_TYPE_CP:
		{
			if (const auto c = core->getData()->checkpoints.find(id); c != core->getData()->checkpoints.end())
			{
				success = Utility::isInContainer(c->second->players, playerId);
			}
			break;
		}
		case STREAMER_TYPE_RACE_CP:
		{
			if (const auto r = core->getData()->raceCheckpoints.find(id); r != core->getData()->raceCheckpoints.end())
			{
				success = Utility::isInContainer(r->second->players, playerId);
			}
			break;
		}
		case STREAMER_TYPE_MAP_ICON:
		{
			if (const auto m = core->getData()->mapIcons.find(id); m != core->getData()->mapIcons.end())
			{
				success = Utility::isInContainer(m->second->players, playerId);
			}
			break;
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			if (const auto t = core->getData()->textLabels.find(id); t != core->getData()->textLabels.end())
			{
				success = Utility::isInContainer(t->second->players, playerId);
			}
			break;
		}
		case STREAMER_TYPE_AREA:
		{
			if (const auto a = core->getData()->areas.find(id); a != core->getData()->areas.end())
			{
				success = Utility::isInContainer(a->second->players, playerId);
			}
			break;
		}
		case STREAMER_TYPE_ACTOR:
		{
			if (const auto a = core->getData()->actors.find(id); a != core->getData()->actors.end())
			{
				success = Utility::isInContainer(a->second->players, playerId);
			}
			break;
		}
		default:
		{
			Utility::logError("Streamer_IsToggleItem: Invalid type specified.");
			break;
		}
	}

	API_RETURN(bool success);
}

OMPNODE_API(StreamerMisc, ToggleAllItems, int playerId, int type, int toggle, JSString exceptionsJson)
{
	bool success = false;
	std::unordered_set<int> exceptions;

	if (!exceptionsJson.empty())
	{
		if (nlohmann::json jsonArray = nlohmann::json::parse(exceptionsJson); jsonArray.is_array())
		{
			for (const auto& item : jsonArray)
			{
				if (item.is_number())
				{
					exceptions.insert(item.get<int>());
				}
			}
		}
	}

	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			for (auto &[id, object] : core->getData()->objects)
			{
				if (const auto e = exceptions.find(id); e == exceptions.end())
				{
					if (!toggle)
					{
						success = Utility::removeFromContainer(object->players, playerId);
					}
					else
					{
						success = Utility::addToContainer(object->players, playerId);
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_PICKUP:
		{
			for (auto &[id, pickup] : core->getData()->pickups)
			{
				if (const auto e = exceptions.find(id); e == exceptions.end())
				{
					if (!toggle)
					{
						success = Utility::removeFromContainer(pickup->players, playerId);
					}
					else
					{
						success = Utility::addToContainer(pickup->players, playerId);
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_CP:
		{
			for (auto &[id, cp] : core->getData()->checkpoints)
			{
				if (const auto e = exceptions.find(id); e == exceptions.end())
				{
					if (!toggle)
					{
						success = Utility::removeFromContainer(cp->players, playerId);
					}
					else
					{
						success = Utility::addToContainer(cp->players, playerId);
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_RACE_CP:
		{
			for (auto &[id, rcp] : core->getData()->raceCheckpoints)
			{
				if (const auto e = exceptions.find(id); e == exceptions.end())
				{
					if (!toggle)
					{
						success = Utility::removeFromContainer(rcp->players, playerId);
					}
					else
					{
						success = Utility::addToContainer(rcp->players, playerId);
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_MAP_ICON:
		{
			for (auto &[id, mapIcon] : core->getData()->mapIcons)
			{
				if (const auto e = exceptions.find(id); e == exceptions.end())
				{
					if (!toggle)
					{
						success = Utility::removeFromContainer(mapIcon->players, playerId);
					}
					else
					{
						success = Utility::addToContainer(mapIcon->players, playerId);
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			for (auto &[id, textLabel] : core->getData()->textLabels)
			{
				if (const auto e = exceptions.find(id); e == exceptions.end())
				{
					if (!toggle)
					{
						success = Utility::removeFromContainer(textLabel->players, playerId);
					}
					else
					{
						success = Utility::addToContainer(textLabel->players, playerId);
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_AREA:
		{
			for (auto &[id, area] : core->getData()->areas)
			{
				if (const auto e = exceptions.find(id); e == exceptions.end())
				{
					if (!toggle)
					{
						success = Utility::removeFromContainer(area->players, playerId);
					}
					else
					{
						success = Utility::addToContainer(area->players, playerId);
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_ACTOR:
		{
			for (auto &[id, actor] : core->getData()->actors)
			{
				if (const auto e = exceptions.find(id); e == exceptions.end())
				{
					if (!toggle)
					{
						success = Utility::removeFromContainer(actor->players, playerId);
					}
					else
					{
						success = Utility::addToContainer(actor->players, playerId);
					}
				}
			}
			break;
		}
		default:
		{
			Utility::logError("Streamer_ToggleAllItems: Invalid type specified.");
			break;
		}
	}

	API_RETURN(bool success);
}

OMPNODE_API(StreamerMisc, IsItemVisible, int playerId, int type, int id)
{
	bool success = false;
	
	switch (type)
	{
		case STREAMER_TYPE_PICKUP:
		{
			int pickupId = id;
			Item::SharedPickup p = core->getData()->pickups[pickupId];
			for (int world : p->worlds)
			{
				if (const auto i = core->getData()->internalPickups.find(std::make_pair(pickupId, world)); i != core->getData()->internalPickups.end())
				{
					success = true;
					API_RETURN(bool success);
				}
			}
			API_RETURN(bool success);
		}
		case STREAMER_TYPE_ACTOR:
		{
			int actorId = id;
			Item::SharedActor a = core->getData()->actors[actorId];
			for (int world : a->worlds)
			{
				if (const auto i = core->getData()->internalActors.find(std::make_pair(actorId, world)); i != core->getData()->internalActors.end())
				{
					success = true;
					API_RETURN(bool success);
				}
			}
			API_RETURN(bool success);
		}
		default:
		{

		}
	}
	if (const auto p = core->getData()->players.find(playerId); p != core->getData()->players.end())
	{
		switch (type)
		{
			case STREAMER_TYPE_OBJECT:
			{
				if (const auto i = p->second.internalObjects.find(id); i != p->second.internalObjects.end())
				{
					success = true;
					API_RETURN(bool success);
				}
				API_RETURN(bool success);
			}
			case STREAMER_TYPE_CP:
			{
				if (p->second.visibleCheckpoint == id)
				{
					success = true;
					API_RETURN(bool success);
				}
				API_RETURN(bool success);
			}
			case STREAMER_TYPE_RACE_CP:
			{
				if (p->second.visibleRaceCheckpoint == id)
				{
					success = true;
					API_RETURN(bool success);
				}
				API_RETURN(bool success);
			}
			case STREAMER_TYPE_MAP_ICON:
			{
				if (const auto i = p->second.internalMapIcons.find(id); i != p->second.internalMapIcons.end())
				{
					success = true;
					API_RETURN(bool success);
				}
				API_RETURN(bool success);
			}
			case STREAMER_TYPE_3D_TEXT_LABEL:
			{
				if (const auto i = p->second.internalTextLabels.find(id); i != p->second.internalTextLabels.end())
				{
					success = true;
					API_RETURN(bool success);
				}
			}
			case STREAMER_TYPE_AREA:
			{
				if (const auto i = p->second.internalAreas.find(id); i != p->second.internalAreas.end())
				{
					success = true;
					API_RETURN(bool success);
				}
				API_RETURN(bool success);
			}
			default:
			{
				Utility::logError("Streamer_IsItemVisible: Invalid type specified.");
				API_RETURN(bool success);
			}
		}
	}

	API_RETURN(bool success);
}

OMPNODE_API(StreamerMisc, DestroyAllVisibleItems, int playerId, int type)
{
	bool success = false;
	constexpr bool serverWide = true;

	switch (type)
	{
		case STREAMER_TYPE_PICKUP:
		{
			auto i = core->getData()->internalPickups.begin();
			while (i != core->getData()->internalPickups.end())
			{
				if constexpr (const auto p = core->getData()->pickups.find(i->first.first);
					serverWide || (p != core->getData()->pickups.end() && p->second->amx == nullptr))
				{
					ompgdk::DestroyPickup(i->second);
					i = core->getData()->internalPickups.erase(i);
				}
				else
				{
					++i;
				}
			}

			success = true;
			API_RETURN(bool success);
		}
		case STREAMER_TYPE_ACTOR:
		{
			auto i = core->getData()->internalActors.begin();
			while (i != core->getData()->internalActors.end())
			{
				if constexpr (const auto a = core->getData()->actors.find(i->first.first);
					serverWide || (a != core->getData()->actors.end() && a->second->amx == nullptr))
				{
					ompgdk::DestroyActor(i->second);
					i = core->getData()->internalActors.erase(i);
				}
				else
				{
					++i;
				}
			}

			success = true;
			API_RETURN(bool success);
		}
		default:
		{

		}
	}

	if (const auto p = core->getData()->players.find(playerId); p != core->getData()->players.end())
	{
		switch (type)
		{
			case STREAMER_TYPE_OBJECT:
			{
				auto i = p->second.internalObjects.begin();
				while (i != p->second.internalObjects.end())
				{
					if constexpr (const auto o = core->getData()->objects.find(i->first);
						serverWide || (o != core->getData()->objects.end() && o->second->amx == nullptr))
					{
						ompgdk::DestroyPlayerObject(p->first, i->second);
						i = p->second.internalObjects.erase(i);
					}
					else
					{
						++i;
					}
				}
				
				success = true;
				API_RETURN(bool success);
			}
			case STREAMER_TYPE_CP:
			{
				if (p->second.visibleCheckpoint)
				{
					if constexpr (const auto c = core->getData()->checkpoints.find(p->second.visibleCheckpoint);
						serverWide || (c != core->getData()->checkpoints.end() && c->second->amx == nullptr))
					{
						ompgdk::DisablePlayerCheckpoint(p->first);
						p->second.activeCheckpoint = 0;
						p->second.visibleCheckpoint = 0;
						
						success = true;
						API_RETURN(bool success);
					}
				}

				API_RETURN(bool success);
			}
			case STREAMER_TYPE_RACE_CP:
			{
				if (p->second.visibleRaceCheckpoint)
				{
					if constexpr (const auto r = core->getData()->raceCheckpoints.find(p->second.visibleRaceCheckpoint);
						serverWide || (r != core->getData()->raceCheckpoints.end() && r->second->amx == nullptr))
					{
						ompgdk::DisablePlayerRaceCheckpoint(p->first);
						p->second.activeRaceCheckpoint = 0;
						p->second.visibleRaceCheckpoint = 0;

						success = true;
						API_RETURN(bool success);
					}
				}

				API_RETURN(bool success);
			}
			case STREAMER_TYPE_MAP_ICON:
			{
				auto i = p->second.internalMapIcons.begin();
				while (i != p->second.internalMapIcons.end())
				{
					if constexpr (const auto m = core->getData()->mapIcons.find(i->first);
						serverWide || (m != core->getData()->mapIcons.end() && m->second->amx == nullptr))
					{
						ompgdk::RemovePlayerMapIcon(p->first, i->second);
						i = p->second.internalMapIcons.erase(i);
					}
					else
					{
						++i;
					}
				}

				success = true;
				API_RETURN(bool success);
			}
			case STREAMER_TYPE_3D_TEXT_LABEL:
			{
				auto i = p->second.internalTextLabels.begin();
				while (i != p->second.internalTextLabels.end())
				{
					if constexpr (const auto t = core->getData()->textLabels.find(i->first);
						serverWide || (t != core->getData()->textLabels.end() && t->second->amx == nullptr))
					{
						ompgdk::DeletePlayer3DTextLabel(p->first, i->second);
						i = p->second.internalTextLabels.erase(i);
					}
					else
					{
						++i;
					}
				}

				success = true;
				API_RETURN(bool success);
			}
			case STREAMER_TYPE_AREA:
			{
				auto i = p->second.internalAreas.begin();
				while (i != p->second.internalAreas.end())
				{
					if constexpr (const auto a = core->getData()->areas.find(*i);
						serverWide || (a != core->getData()->areas.end() && a->second->amx == nullptr))
					{
						i = p->second.internalAreas.erase(i);
					}
					else
					{
						++i;
					}
				}

				success = true;
				API_RETURN(bool success);
			}
			default:
			{
				Utility::logError("Streamer_DestroyAllVisibleItems: Invalid type specified.");
				API_RETURN(bool success);
			}
		}
	}

	API_RETURN(bool success);
}

OMPNODE_API(StreamerMisc, CountVisibleItems, int playerId, int type)
{
	int count = 0;
	constexpr bool serverWide = true;

	switch (type)
	{
		case STREAMER_TYPE_PICKUP:
		{
			count = static_cast<cell>(core->getData()->internalPickups.size());
			API_RETURN(int count);
		}
		case STREAMER_TYPE_ACTOR:
		{
			count = static_cast<cell>(core->getData()->internalActors.size());
			API_RETURN(int count);
		}
		default:
		{

		}
	}

	if (const auto p = core->getData()->players.find(playerId); p != core->getData()->players.end())
	{
		switch (type)
		{
			case STREAMER_TYPE_OBJECT:
			{
				if (serverWide)
				{
					count = static_cast<cell>(p->second.internalObjects.size());
					API_RETURN(int count);
				}

				for (auto &[id, _] : p->second.internalObjects)
				{
					if (const auto o = core->getData()->objects.find(id);
						o != core->getData()->objects.end() && o->second->amx == nullptr)
					{
						++count;
					}
				}
				API_RETURN(int count);
			}
			case STREAMER_TYPE_CP:
			{
				if (p->second.visibleCheckpoint)
				{
					if constexpr (const auto c = core->getData()->checkpoints.find(p->second.visibleCheckpoint);
						serverWide || (c != core->getData()->checkpoints.end() && c->second->amx == nullptr))
					{
						count = 1;
						API_RETURN(int count);
					}
				}
				API_RETURN(int count);
			}
			case STREAMER_TYPE_RACE_CP:
			{
				if (p->second.visibleRaceCheckpoint)
				{
					if constexpr (const auto r = core->getData()->raceCheckpoints.find(p->second.visibleRaceCheckpoint);
						serverWide || (r != core->getData()->raceCheckpoints.end() && r->second->amx == nullptr))
					{
						count = 1;
						API_RETURN(int count);
					}
				}
				API_RETURN(int count);
			}
			case STREAMER_TYPE_MAP_ICON:
			{
				if (serverWide)
				{
					count = static_cast<cell>(p->second.internalMapIcons.size());
					API_RETURN(int count);
				}

				for (auto &[id, _] : p->second.internalMapIcons)
				{
					if (const auto m = core->getData()->mapIcons.find(id);
						m != core->getData()->mapIcons.end() && m->second->amx == nullptr)
					{
						++count;
					}
				}
				API_RETURN(int count);
			}
			case STREAMER_TYPE_3D_TEXT_LABEL:
			{
				if (serverWide)
				{
					count = static_cast<cell>(p->second.internalTextLabels.size());
					API_RETURN(int count);
				}

				for (auto &[id, _] : p->second.internalTextLabels)
				{
					if (const auto t = core->getData()->textLabels.find(id);
						t != core->getData()->textLabels.end() && t->second->amx == nullptr)
					{
						++count;
					}
				}
				API_RETURN(int count);
			}
			case STREAMER_TYPE_AREA:
			{
				if (serverWide)
				{
					count = static_cast<cell>(p->second.internalAreas.size());
					API_RETURN(int count);
				}

				for (int internalArea : p->second.internalAreas)
				{
					if (const auto a = core->getData()->areas.find(internalArea);
						a != core->getData()->areas.end() && a->second->amx == nullptr)
					{
						++count;
					}
				}
				API_RETURN(int count);
			}
			default:
			{
				Utility::logError("Streamer_CountVisibleItems: Invalid type specified.");
				API_RETURN(int count);
			}
		}
	}

	API_RETURN(int count);
}

OMPNODE_API(StreamerMisc, DestroyAllItems, int type)
{
	bool success = false;
	constexpr bool serverWide = true;

	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			auto o = core->getData()->objects.begin();
			while (o != core->getData()->objects.end())
			{
				if constexpr (serverWide || o->second->amx == nullptr)
				{
					o = Utility::destroyObject(o);
				}
				else
				{
					++o;
				}
			}

			success = true;
			API_RETURN(bool success);
		}
		case STREAMER_TYPE_PICKUP:
		{
			auto p = core->getData()->pickups.begin();
			while (p != core->getData()->pickups.end())
			{
				if constexpr (serverWide || p->second->amx == nullptr)
				{
					p = Utility::destroyPickup(p);
				}
				else
				{
					++p;
				}
			}

			success = true;
			API_RETURN(bool success);
		}
		case STREAMER_TYPE_CP:
		{
			auto c = core->getData()->checkpoints.begin();
			while (c != core->getData()->checkpoints.end())
			{
				if constexpr (serverWide || c->second->amx == nullptr)
				{
					c = Utility::destroyCheckpoint(c);
				}
				else
				{
					++c;
				}
			}

			success = true;
			API_RETURN(bool success);
		}
		case STREAMER_TYPE_RACE_CP:
		{
			auto r = core->getData()->raceCheckpoints.begin();
			while (r != core->getData()->raceCheckpoints.end())
			{
				if constexpr (serverWide || r->second->amx == nullptr)
				{
					r = Utility::destroyRaceCheckpoint(r);
				}
				else
				{
					++r;
				}
			}

			success = true;
			API_RETURN(bool success);
		}
		case STREAMER_TYPE_MAP_ICON:
		{
			auto m = core->getData()->mapIcons.begin();
			while (m != core->getData()->mapIcons.end())
			{
				if constexpr (serverWide || m->second->amx == nullptr)
				{
					m = Utility::destroyMapIcon(m);
				}
				else
				{
					++m;
				}
			}

			success = true;
			API_RETURN(bool success);
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			auto t = core->getData()->textLabels.begin();
			while (t != core->getData()->textLabels.end())
			{
				if constexpr (serverWide || t->second->amx == nullptr)
				{
					t = Utility::destroyTextLabel(t);
				}
				else
				{
					++t;
				}
			}

			success = true;
			API_RETURN(bool success);
		}
		case STREAMER_TYPE_AREA:
		{
			Utility::executeFinalAreaCallbacksForAllAreas(nullptr, serverWide);
			auto a = core->getData()->areas.begin();
			while (a != core->getData()->areas.end())
			{
				if constexpr (serverWide || a->second->amx == nullptr)
				{
					a = Utility::destroyArea(a);
				}
				else
				{
					++a;
				}
			}

			success = true;
			API_RETURN(bool success);
		}
		case STREAMER_TYPE_ACTOR:
		{
			auto a = core->getData()->actors.begin();
			while (a != core->getData()->actors.end())
			{
				if constexpr (serverWide || a->second->amx == nullptr)
				{
					a = Utility::destroyActor(a);
				}
				else
				{
					++a;
				}
			}

			success = true;
			API_RETURN(bool success);
		}
		default:
		{
			Utility::logError("Streamer_DestroyAllItems: Invalid type specified.");
			API_RETURN(bool success);
		}
	}

	API_RETURN(bool success);
}

OMPNODE_API(StreamerMisc, CountItems, int type)
{
	int count = 0;
	constexpr bool serverWide = true;

	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			if (serverWide)
			{
				count = static_cast<cell>(core->getData()->objects.size());
				API_RETURN(int count);
			}

			for (auto &[_, object] : core->getData()->objects)
			{
				if (object->amx == nullptr)
				{
					++count;
				}
			}

			API_RETURN(int count);
		}
		case STREAMER_TYPE_PICKUP:
		{
			if (serverWide)
			{
				count = static_cast<cell>(core->getData()->pickups.size());
				API_RETURN(int count);
			}

			for (auto &[_, pickup] : core->getData()->pickups)
			{
				if (pickup->amx == nullptr)
				{
					++count;
				}
			}

			API_RETURN(int count);

		}
		case STREAMER_TYPE_CP:
		{
			if (serverWide)
			{
				count = static_cast<cell>(core->getData()->checkpoints.size());
				API_RETURN(int count);
			}

			for (auto &[_, cp] : core->getData()->checkpoints)
			{
				if (cp->amx == nullptr)
				{
					++count;
				}
			}

			API_RETURN(int count);

		}
		case STREAMER_TYPE_RACE_CP:
		{
			if (serverWide)
			{
				count = static_cast<cell>(core->getData()->raceCheckpoints.size());
				API_RETURN(int count);
			}

			for (auto &[id, rcp] : core->getData()->raceCheckpoints)
			{
				if (rcp->amx == nullptr)
				{
					++count;
				}
			}

			API_RETURN(int count);

		}
		case STREAMER_TYPE_MAP_ICON:
		{
			if (serverWide)
			{
				count = static_cast<cell>(core->getData()->mapIcons.size());
				API_RETURN(int count);
			}

			for (auto &[_, mapIcon] : core->getData()->mapIcons)
			{
				if (mapIcon->amx == nullptr)
				{
					++count;
				}
			}

			API_RETURN(int count);
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			if (serverWide)
			{
				count = static_cast<cell>(core->getData()->textLabels.size());
				API_RETURN(int count);
			}

			for (auto &[_, textLabel] : core->getData()->textLabels)
			{
				if (textLabel->amx == nullptr)
				{
					++count;
				}
			}

			API_RETURN(int count);

		}
		case STREAMER_TYPE_AREA:
		{
			if (serverWide)
			{
				count = static_cast<cell>(core->getData()->areas.size());
				API_RETURN(int count);
			}

			for (auto &[_, area] : core->getData()->areas)
			{
				if (area->amx == nullptr)
				{
					++count;
				}
			}

			API_RETURN(int count);
		}
		case STREAMER_TYPE_ACTOR:
		{
			if (serverWide)
			{
				count = static_cast<cell>(core->getData()->actors.size());
				API_RETURN(int count);
			}

			for (auto &[_, actor] : core->getData()->actors)
			{
				if (actor->amx == nullptr)
				{
					++count;
				}
			}

			API_RETURN(int count);
		}
		default:
		{
			Utility::logError("Streamer_CountItems: Invalid type specified.");
			API_RETURN(int count);
		}
	}

	API_RETURN(int count);
}

OMPNODE_API(StreamerMisc, GetNearbyItems, float x, float y, float z, int type, float range, int worldId)
{
	int count = 0;
	JSString resultJson = "[]";

	auto position2d = Eigen::Vector2f(x, y);
	auto position3d = Eigen::Vector3f(x, y, z);

	range *= range;

	std::multimap<float, int> orderedItems;
	std::vector<SharedCell> pointCells;

	core->getGrid()->findMinimalCellsForPoint(position2d, pointCells, range);

	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			for (const auto & pointCell : pointCells)
			{
				for (const auto &[id, object] : pointCell->objects)
				{
					if (worldId == -1 || object->worlds.find(worldId) != object->worlds.end())
					{
						float distance = 0.0f;
						if (object->attach)
						{
							distance = static_cast<float>(boost::geometry::comparable_distance(position3d, object->attach->position));
						}
						else
						{
							distance = static_cast<float>(boost::geometry::comparable_distance(position3d, object->position));
						}
						if (distance < range)
						{
							orderedItems.insert(std::pair(distance, id));
						}
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_PICKUP:
		{
			for (const auto & pointCell : pointCells)
			{
				for (const auto &[id, pickup] : pointCell->pickups)
				{
					if (worldId == -1 || pickup->worlds.find(worldId) != pickup->worlds.end())
					{
						if (const auto distance = static_cast<float>(boost::geometry::comparable_distance(
							position3d, pickup->position)); distance < range)
						{
							orderedItems.insert(std::pair(distance, id));
						}
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_CP:
		{
			for (const auto & pointCell : pointCells)
			{
				for (const auto &[id, checkpoint] : pointCell->checkpoints)
				{
					if (worldId == -1 || checkpoint->worlds.find(worldId) != checkpoint->worlds.end())
					{
						if (const auto distance = static_cast<float>(boost::geometry::comparable_distance(
							position3d, checkpoint->position)); distance < range)
						{
							orderedItems.insert(std::pair(distance, id));
						}
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_RACE_CP:
		{
			for (const auto & pointCell : pointCells)
			{
				for (const auto &[id, raceCheckpoint] : pointCell->raceCheckpoints)
				{
					if (worldId == -1 || raceCheckpoint->worlds.find(worldId) != raceCheckpoint->worlds.end())
					{
						if (const auto distance = static_cast<float>(boost::geometry::comparable_distance(
							position3d, raceCheckpoint->position)); distance < range)
						{
							orderedItems.insert(std::pair(distance, id));
						}
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_MAP_ICON:
		{
			for (const auto & pointCell : pointCells)
			{
				for (const auto &[id, mapIcon] : pointCell->mapIcons)
				{
					if (worldId == -1 || mapIcon->worlds.find(worldId) != mapIcon->worlds.end())
					{
						if (const auto distance = static_cast<float>(boost::geometry::comparable_distance(
							position3d, mapIcon->position)); distance < range)
						{
							orderedItems.insert(std::pair(distance, id));
						}
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			for (const auto & pointCell : pointCells)
			{
				for (const auto &[id, textLabel] : pointCell->textLabels)
				{
					if (worldId == -1 || textLabel->worlds.find(worldId) != textLabel->worlds.end())
					{
						if (const auto distance = static_cast<float>(boost::geometry::comparable_distance(
							position3d, textLabel->position)); distance < range)
						{
							orderedItems.insert(std::pair(distance, id));
						}
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_AREA:
		{
			for (const auto & pointCell : pointCells)
			{
				for (const auto &[id, area] : pointCell->areas)
				{
					if (worldId == -1 || area->worlds.find(worldId) != area->worlds.end())
					{
						std::variant<Polygon2d, Box2d, Box3d, Eigen::Vector2f, Eigen::Vector3f> position;
						if (area->attach)
						{
							position = area->attach->position;
						}
						else
						{
							position = area->position;
						}
						float distance = 0.0f;

						switch (area->type)
						{
							case STREAMER_AREA_TYPE_CIRCLE:
							case STREAMER_AREA_TYPE_CYLINDER:
							{
								distance = static_cast<float>(boost::geometry::distance(position2d, std::get<Eigen::Vector2f>(position)));
								break;
							}
							case STREAMER_AREA_TYPE_SPHERE:
							{
								distance = static_cast<float>(boost::geometry::comparable_distance(position3d, std::get<Eigen::Vector3f>(position)));
								break;
							}
							case STREAMER_AREA_TYPE_RECTANGLE:
							{
								auto centroid = boost::geometry::return_centroid<Eigen::Vector2f>(std::get<Box2d>(position));
								distance = static_cast<float>(boost::geometry::comparable_distance(position2d, centroid));
								break;
							}
							case STREAMER_AREA_TYPE_CUBOID:
							{
								auto centroid = boost::geometry::return_centroid<Eigen::Vector3f>(std::get<Box3d>(position));
								distance = static_cast<float>(boost::geometry::comparable_distance(position3d, centroid));
								break;
							}
							case STREAMER_AREA_TYPE_POLYGON:
							{
								auto centroid = boost::geometry::return_centroid<Eigen::Vector2f>(std::get<Polygon2d>(position));
								distance = static_cast<float>(boost::geometry::comparable_distance(position2d, centroid));
								break;
							}
							default:
							{

							}
						}
						if (distance < range)
						{
							orderedItems.insert(std::pair(distance, id));
						}
					}
				}
			}
			break;
		}
		case STREAMER_TYPE_ACTOR:
		{
			for (const auto & pointCell : pointCells)
			{
				for (const auto &[id, actor] : pointCell->actors)
				{
					if (worldId == -1 || actor->worlds.find(worldId) != actor->worlds.end())
					{
						if (const auto distance = static_cast<float>(boost::geometry::comparable_distance(
							position3d, actor->position)); distance < range)
						{
							orderedItems.insert(std::pair(distance, id));
						}
					}
				}
			}
			break;
		}
		default:
		{
			Utility::logError("Streamer_GetNearbyItems: Invalid type specified.");
			API_RETURN(int count, JSString resultJson);
		}
	}

	std::vector<int> finalItems;
	for (auto &[_, orderedId] : orderedItems)
	{
		finalItems.push_back(orderedId);
	}

	count = static_cast<cell>(finalItems.size());
	resultJson = nlohmann::json(finalItems).dump();

	API_RETURN(int count, JSString resultJson);
}

OMPNODE_API(StreamerMisc, GetAllVisibleItems, int playerId, int type)
{
	int count = 0;
	JSString resultJson = "[]";

	std::multimap<float, int> orderedItems;

	if (const auto p = core->getData()->players.find(playerId); p != core->getData()->players.end())
	{
		switch (type)
		{
			case STREAMER_TYPE_OBJECT:
			{
				for (auto &[id, _] : p->second.internalObjects)
				{
					if (const auto o = core->getData()->objects.find(id); o != core->getData()->objects.end())
					{
						float distance = 0.0f;
						if (o->second->attach)
						{
							distance = static_cast<float>(boost::geometry::comparable_distance(p->second.position, o->second->attach->position));
						}
						else
						{
							distance = static_cast<float>(boost::geometry::comparable_distance(p->second.position, o->second->position));
						}
						orderedItems.insert(std::pair(distance, o->first));
					}
				}
				break;
			}
			case STREAMER_TYPE_PICKUP:
			{
				for (auto &[id, _] : core->getData()->internalPickups)
				{
					if (const auto q = core->getData()->pickups.find(id.first); q != core->getData()->pickups.end())
					{
						auto distance = static_cast<float>(boost::geometry::comparable_distance(p->second.position, q->second->position));
						orderedItems.insert(std::pair(distance, q->first));
					}
				}
				break;
			}
			case STREAMER_TYPE_CP:
			{
				if (p->second.visibleCheckpoint != INVALID_STREAMER_ID)
				{
					if (const auto c = core->getData()->checkpoints.find(p->second.visibleCheckpoint); c != core->getData()->checkpoints.end())
					{
						auto distance = static_cast<float>(boost::geometry::comparable_distance(p->second.position, c->second->position));
						orderedItems.insert(std::pair(distance, c->first));
					}
				}
				break;
			}
			case STREAMER_TYPE_RACE_CP:
			{
				if (p->second.visibleRaceCheckpoint != INVALID_STREAMER_ID)
				{
					if (const auto c = core->getData()->raceCheckpoints.find(p->second.visibleRaceCheckpoint); c != core->getData()->raceCheckpoints.end())
					{
						auto distance = static_cast<float>(boost::geometry::comparable_distance(p->second.position, c->second->position));
						orderedItems.insert(std::pair(distance, c->first));
					}
				}
				break;
			}
			case STREAMER_TYPE_MAP_ICON:
			{
				for (auto &[id, _] : p->second.internalMapIcons)
				{
					if (const auto m = core->getData()->mapIcons.find(id); m != core->getData()->mapIcons.end())
					{
						auto distance = static_cast<float>(boost::geometry::comparable_distance(p->second.position, m->second->position));
						orderedItems.insert(std::pair(distance, m->first));
					}
				}
				break;
			}
			case STREAMER_TYPE_3D_TEXT_LABEL:
			{
				for (auto &[id, _] : p->second.internalTextLabels)
				{
					if (const auto t = core->getData()->textLabels.find(id); t != core->getData()->textLabels.end())
					{
						float distance = 0.0f;
						if (t->second->attach)
						{
							distance = static_cast<float>(boost::geometry::comparable_distance(p->second.position, t->second->attach->position));
						}
						else
						{
							distance = static_cast<float>(boost::geometry::comparable_distance(p->second.position, t->second->position));
						}
						orderedItems.insert(std::pair(distance, t->first));
					}
				}
				break;
			}
			case STREAMER_TYPE_ACTOR:
			{
				for (auto &[id, _] : core->getData()->internalActors)
				{
					if (const auto a = core->getData()->actors.find(id.first); a != core->getData()->actors.end())
					{
						auto distance = static_cast<float>(boost::geometry::comparable_distance(p->second.position, a->second->position));
						orderedItems.insert(std::pair(distance, a->first));
					}
				}
				break;
			}
			default:
			{

			}
		}
	}

	std::vector<int> finalItems;
	for (auto &[_, orderedId] : orderedItems)
	{
		finalItems.push_back(orderedId);
	}

	count = static_cast<cell>(finalItems.size());
	resultJson = nlohmann::json(finalItems).dump();

	API_RETURN(int count, JSString resultJson);
}

OMPNODE_API(StreamerMisc, GetItemPos, int type, int id)
{
	bool success = false;

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Eigen::Vector3f position = Eigen::Vector3f::Zero();

	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			if (const auto o = core->getData()->objects.find(id); o != core->getData()->objects.end())
			{
				if (o->second->attach)
				{
					position = o->second->attach->position;
				}
				else
				{
					position = o->second->position;
				}
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_PICKUP:
		{
			if (const auto p = core->getData()->pickups.find(id); p != core->getData()->pickups.end())
			{
				position = p->second->position;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_CP:
		{
			if (const auto c = core->getData()->checkpoints.find(id); c != core->getData()->checkpoints.end())
			{
				position = c->second->position;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_RACE_CP:
		{
			if (const auto r = core->getData()->raceCheckpoints.find(id); r != core->getData()->raceCheckpoints.end())
			{
				position = r->second->position;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_MAP_ICON:
		{
			if (const auto m = core->getData()->mapIcons.find(id); m != core->getData()->mapIcons.end())
			{
				position = m->second->position;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			if (const auto t = core->getData()->textLabels.find(id); t != core->getData()->textLabels.end())
			{
				if (t->second->attach)
				{
					position = t->second->attach->position;
				}
				else
				{
					position = t->second->position;
				}
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_AREA:
		{
			if (const auto a = core->getData()->areas.find(id); a != core->getData()->areas.end())
			{
				std::variant<Polygon2d, Box2d, Box3d, Eigen::Vector2f, Eigen::Vector3f> areaPosition;
				if (a->second->attach)
				{
					areaPosition = a->second->attach->position;
				}
				else
				{
					areaPosition = a->second->position;
				}
				switch (a->second->type)
				{
					case STREAMER_AREA_TYPE_CIRCLE:
					{
						position[0] = std::get<Eigen::Vector2f>(areaPosition)[0];
						position[1] = std::get<Eigen::Vector2f>(areaPosition)[1];
						position[2] = 0.0f;
						success = true;
						break;
					}
					case STREAMER_AREA_TYPE_CYLINDER:
					{
						position[0] = std::get<Eigen::Vector2f>(areaPosition)[0];
						position[1] = std::get<Eigen::Vector2f>(areaPosition)[1];
						if (a->second->height[0] == -std::numeric_limits<float>::infinity() || a->second->height[1] == std::numeric_limits<float>::infinity())
						{
							position[2] = 0.0f;
						}
						else
						{
							position[2] = (a->second->height[0] + a->second->height[1]) / 2.0f;
						}
						success = true;
						break;
					}
					case STREAMER_AREA_TYPE_SPHERE:
					{
						position = std::get<Eigen::Vector3f>(areaPosition);
						success = true;
						break;
					}
					case STREAMER_AREA_TYPE_RECTANGLE:
					{
						auto centroid = boost::geometry::return_centroid<Eigen::Vector2f>(std::get<Box2d>(areaPosition));
						position[0] = centroid[0];
						position[1] = centroid[1];
						position[2] = 0.0f;
						success = true;
						break;
					}
					case STREAMER_AREA_TYPE_CUBOID:
					{
						position = boost::geometry::return_centroid<Eigen::Vector3f>(std::get<Box3d>(areaPosition));
						success = true;
						break;
					}
					case STREAMER_AREA_TYPE_POLYGON:
					{
						auto centroid = boost::geometry::return_centroid<Eigen::Vector2f>(std::get<Polygon2d>(areaPosition));
						position[0] = centroid[0];
						position[1] = centroid[1];
						if (a->second->height[0] == -std::numeric_limits<float>::infinity() || a->second->height[1] == std::numeric_limits<float>::infinity())
						{
							position[2] = 0.0f;
						}
						else
						{
							position[2] = (a->second->height[0] + a->second->height[1]) / 2.0f;
						}
						success = true;
						break;
					}
					default:
					{

					}
				}
			}

			if (success)
			{
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_ACTOR:
		{
			if (const auto a = core->getData()->actors.find(id); a != core->getData()->actors.end())
			{
				position = a->second->position;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		default:
		{
			Utility::logError("Streamer_GetItemPos: Invalid type specified.");
			API_RETURN(bool success, float x, float y, float z);
		}
	}

	x = position[0];
	y = position[1];
	z = position[2];

	API_RETURN(bool success, float x, float y, float z);
}

OMPNODE_API(StreamerMisc, SetItemPos, int type, int id, float x, float y, float z)
{
	bool success = false;

	auto newPos = Eigen::Vector3f(x, y, z);

	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			if (const auto o = core->getData()->objects.find(id); o != core->getData()->objects.end())
			{
				Eigen::Vector3f position = o->second->position;
				o->second->position = newPos;

				for (auto &[playerId, player] : core->getData()->players)
				{
					if (const auto i = player.internalObjects.find(o->first); i != player.internalObjects.end())
					{
						ompgdk::SetPlayerObjectPos(playerId, i->second, o->second->position[0], o->second->position[1], o->second->position[2]);
					}
				}
				if (position[0] != o->second->position[0] || position[1] != o->second->position[1])
				{
					if (o->second->cell)
					{
						core->getGrid()->removeObject(o->second, true);
					}
				}
				if (o->second->move)
				{
					o->second->move->duration = static_cast<int>((static_cast<float>(boost::geometry::distance(std::get<0>(o->second->move->position), o->second->position) / o->second->move->speed) * 1000.0f));
					std::get<1>(o->second->move->position) = o->second->position;
					std::get<2>(o->second->move->position) = (std::get<0>(o->second->move->position) - o->second->position) / static_cast<float>(o->second->move->duration);
					if ((std::get<0>(o->second->move->rotation).maxCoeff() + 1000.0f) > std::numeric_limits<float>::epsilon())
					{
						std::get<1>(o->second->move->rotation) = o->second->rotation;
						std::get<2>(o->second->move->rotation) = (std::get<0>(o->second->move->rotation) - o->second->rotation) / static_cast<float>(o->second->move->duration);
					}
					o->second->move->time = std::chrono::steady_clock::now();
				}

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_PICKUP:
		{
			if (const auto p = core->getData()->pickups.find(id); p != core->getData()->pickups.end())
			{
				Eigen::Vector3f position = p->second->position;
				p->second->position = newPos;

				if (position[0] != p->second->position[0] || position[1] != p->second->position[1])
				{
					if (p->second->cell)
					{
						core->getGrid()->removePickup(p->second, true);
					}
				}
				for (auto w = p->second->worlds.begin(); w != p->second->worlds.end(); ++w)
				{
					if (const auto i = core->getData()->internalPickups.find(std::make_pair(p->first, *w));
						i != core->getData()->internalPickups.end())
					{
						ompgdk::DestroyPickup(i->second);
						i->second = ompgdk::CreatePickup(p->second->modelId, p->second->type, p->second->position[0],
						                                 p->second->position[1], p->second->position[2], *w);
					}
				}

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_CP:
		{
			if (const auto c = core->getData()->checkpoints.find(id); c != core->getData()->checkpoints.end())
			{
				Eigen::Vector3f position = c->second->position;
				c->second->position = newPos;
				if (position[0] != c->second->position[0] || position[1] != c->second->position[1])
				{
					if (c->second->cell)
					{
						core->getGrid()->removeCheckpoint(c->second, true);
					}
				}

				for (auto &[playerId, player] : core->getData()->players)
				{
					if (player.visibleCheckpoint == c->first)
					{
						ompgdk::DisablePlayerCheckpoint(playerId);
						player.activeCheckpoint = 0;
						player.visibleCheckpoint = 0;
					}
				}

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_RACE_CP:
		{
			if (const auto r = core->getData()->raceCheckpoints.find(id); r != core->getData()->raceCheckpoints.end())
			{
				Eigen::Vector3f position = r->second->position;
				r->second->position = newPos;
				if (position[0] != r->second->position[0] || position[1] != r->second->position[1])
				{
					if (r->second->cell)
					{
						core->getGrid()->removeRaceCheckpoint(r->second, true);
					}
				}

				for (auto &[playerId, player] : core->getData()->players)
				{
					if (player.visibleRaceCheckpoint == r->first)
					{
						ompgdk::DisablePlayerRaceCheckpoint(playerId);
						player.activeRaceCheckpoint = 0;
						player.visibleRaceCheckpoint = 0;
					}
				}

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_MAP_ICON:
		{
			if (const auto m = core->getData()->mapIcons.find(id); m != core->getData()->mapIcons.end())
			{
				Eigen::Vector3f position = m->second->position;
				m->second->position = newPos;

				if (position[0] != m->second->position[0] || position[1] != m->second->position[1])
				{
					if (m->second->cell)
					{
						core->getGrid()->removeMapIcon(m->second, true);
					}
				}

				for (auto &[playerId, player] : core->getData()->players)
				{
					if (const auto i = player.internalMapIcons.find(m->first); i != player.internalMapIcons.end())
					{
						ompgdk::RemovePlayerMapIcon(playerId, i->second);
						ompgdk::SetPlayerMapIcon(playerId, i->second, m->second->position[0], m->second->position[1], m->second->position[2], m->second->type, m->second->color, m->second->style);
					}
				}

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			if (const auto t = core->getData()->textLabels.find(id); t != core->getData()->textLabels.end())
			{
				Eigen::Vector3f position = t->second->position;
				t->second->position = newPos;

				if (position[0] != t->second->position[0] || position[1] != t->second->position[1])
				{
					if (t->second->cell)
					{
						core->getGrid()->removeTextLabel(t->second, true);
					}
				}

				for (auto &[playerId, player] : core->getData()->players)
				{
					if (const auto i = player.internalTextLabels.find(t->first); i != player.internalTextLabels.end())
					{
						ompgdk::DeletePlayer3DTextLabel(playerId, i->second);
						i->second = ompgdk::CreatePlayer3DTextLabel(playerId, t->second->text.c_str(), t->second->color, t->second->position[0], t->second->position[1], t->second->position[2], t->second->drawDistance, t->second->attach ? t->second->attach->player : INVALID_PLAYER_ID, t->second->attach ? t->second->attach->vehicle : INVALID_VEHICLE_ID, t->second->testLOS);
					}
				}

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_AREA:
		{
			if (const auto a = core->getData()->areas.find(id); a != core->getData()->areas.end())
			{
				switch (a->second->type)
				{
					case STREAMER_AREA_TYPE_CIRCLE:
					{
						a->second->position = Eigen::Vector2f(newPos.head<2>());
						break;
					}
					case STREAMER_AREA_TYPE_SPHERE:
					{
						a->second->position = newPos;
						break;
					}
					default:
					{
						Utility::logError("Streamer_SetItemPos: Invalid area type specified (only circles and spheres are supported).");

						API_RETURN(bool success);
					}
				}

				core->getGrid()->removeArea(a->second, true);

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_ACTOR:
		{
			if (const auto a = core->getData()->actors.find(id); a != core->getData()->actors.end())
			{
				Eigen::Vector3f position = a->second->position;
				a->second->position = newPos;

				if (position[0] != a->second->position[0] || position[1] != a->second->position[1])
				{
					if (a->second->cell)
					{
						core->getGrid()->removeActor(a->second, true);
					}
				}

				for (auto w = a->second->worlds.begin(); w != a->second->worlds.end(); ++w)
				{
					if (const auto i = core->getData()->internalActors.find(std::make_pair(a->first, *w)); i != core->getData()->internalActors.end())
					{
						ompgdk::DestroyActor(i->second);
						i->second = ompgdk::CreateActor(a->second->modelId, a->second->position[0], a->second->position[1], a->second->position[2], a->second->rotation);
						ompgdk::SetActorInvulnerable(i->second, a->second->invulnerable);
						ompgdk::SetActorHealth(i->second, a->second->health);
						ompgdk::SetActorVirtualWorld(i->second, *w);
						if (a->second->anim)
						{
							ompgdk::ApplyActorAnimation(i->second, a->second->anim->lib.c_str(), a->second->anim->name.c_str(), a->second->anim->delta, a->second->anim->loop, a->second->anim->lockx, a->second->anim->locky, a->second->anim->freeze, a->second->anim->time);
						}
					}
				}

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		default:
		{
			Utility::logError("Streamer_SetItemPos: Invalid type specified.");
			API_RETURN(bool success);
		}
	}

	API_RETURN(bool success);
}

OMPNODE_API(StreamerMisc, GetItemOffset, int type, int id)
{
	bool success = false;

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Eigen::Vector3f positionOffset = Eigen::Vector3f::Zero();
	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			if (const auto o = core->getData()->objects.find(id); o != core->getData()->objects.end())
			{
				positionOffset = o->second->positionOffset;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_PICKUP:
		{
			if (const auto p = core->getData()->pickups.find(id); p != core->getData()->pickups.end())
			{
				positionOffset = p->second->positionOffset;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_CP:
		{
			if (const auto c = core->getData()->checkpoints.find(id); c != core->getData()->checkpoints.end())
			{
				positionOffset = c->second->positionOffset;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_RACE_CP:
		{
			if (const auto r = core->getData()->raceCheckpoints.find(id); r != core->getData()->raceCheckpoints.end())
			{
				positionOffset = r->second->positionOffset;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_MAP_ICON:
		{
			if (const auto m = core->getData()->mapIcons.find(id); m != core->getData()->mapIcons.end())
			{
				positionOffset = m->second->positionOffset;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			if (const auto t = core->getData()->textLabels.find(id); t != core->getData()->textLabels.end())
			{
				positionOffset = t->second->positionOffset;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		case STREAMER_TYPE_ACTOR:
		{
			if (const auto a = core->getData()->actors.find(id); a != core->getData()->actors.end())
			{
				positionOffset = a->second->positionOffset;
				break;
			}

			API_RETURN(bool success, float x, float y, float z);
		}
		default:
		{
			Utility::logError("Streamer_GetItemPosOffset: Invalid type specified.");

			API_RETURN(bool success, float x, float y, float z);
		}
	}

	x = positionOffset[0];
	y = positionOffset[1];
	z = positionOffset[2];

	API_RETURN(bool success, float x, float y, float z);
}

OMPNODE_API(StreamerMisc, SetItemOffset, int type, int id, float x, float y, float z)
{
	bool success = false;

	switch (type)
	{
		case STREAMER_TYPE_OBJECT:
		{
			if (const auto o = core->getData()->objects.find(id); o != core->getData()->objects.end())
			{
				o->second->positionOffset = Eigen::Vector3f(x, y, z);

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_PICKUP:
		{
			if (const auto p = core->getData()->pickups.find(id); p != core->getData()->pickups.end())
			{
				p->second->positionOffset = Eigen::Vector3f(x, y, z);

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_CP:
		{
			if (const auto c = core->getData()->checkpoints.find(id); c != core->getData()->checkpoints.end())
			{
				c->second->positionOffset = Eigen::Vector3f(x, y, z);

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_RACE_CP:
		{
			if (const auto r = core->getData()->raceCheckpoints.find(id); r != core->getData()->raceCheckpoints.end())
			{
				r->second->positionOffset = Eigen::Vector3f(x, y, z);

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_MAP_ICON:
		{
			if (const auto m = core->getData()->mapIcons.find(id); m != core->getData()->mapIcons.end())
			{
				m->second->positionOffset = Eigen::Vector3f(x, y, z);

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_3D_TEXT_LABEL:
		{
			if (const auto t = core->getData()->textLabels.find(id); t != core->getData()->textLabels.end())
			{
				t->second->positionOffset = Eigen::Vector3f(x, y, z);

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		case STREAMER_TYPE_ACTOR:
		{
			if (const auto a = core->getData()->actors.find(id); a != core->getData()->actors.end())
			{
				a->second->positionOffset = Eigen::Vector3f(x, y, z);

				success = true;
				API_RETURN(bool success);
			}
			break;
		}
		default:
		{
			Utility::logError("Streamer_SetItemPosOffset: Invalid type specified.");

			API_RETURN(bool success);
		}
	}

	API_RETURN(bool success);
}

/*
cell AMX_NATIVE_CALL Natives::Streamer_GetItemInternalID(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	switch (static_cast<int>(params[2]))
	{
		case STREAMER_TYPE_PICKUP:
		{
			int pickupId = static_cast<int>(params[3]);
			Item::SharedPickup p = core->getData()->pickups[pickupId];
			for (std::unordered_set<int>::const_iterator w = p->worlds.begin(); w != p->worlds.end(); ++w)
			{
				std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalPickups.find(std::make_pair(pickupId, *w));
				if (i != core->getData()->internalPickups.end())
				{
					return static_cast<cell>(i->second);
				}
			}
			return INVALID_PICKUP_ID;
		}
		case STREAMER_TYPE_ACTOR:
		{
			int actorId = static_cast<int>(params[3]);
			Item::SharedActor a = core->getData()->actors[actorId];
			for (std::unordered_set<int>::const_iterator w = a->worlds.begin(); w != a->worlds.end(); ++w)
			{
				std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.find(std::make_pair(actorId, *w));
				if (i != core->getData()->internalActors.end())
				{
					return static_cast<cell>(i->second);
				}
			}
			return INVALID_ACTOR_ID;
		}
	}
	std::unordered_map<int, Player>::iterator p = core->getData()->players.find(static_cast<int>(params[1]));
	if (p != core->getData()->players.end())
	{
		switch (static_cast<int>(params[2]))
		{
			case STREAMER_TYPE_OBJECT:
			{
				std::unordered_map<int, int>::iterator i = p->second.internalObjects.find(static_cast<int>(params[3]));
				if (i != p->second.internalObjects.end())
				{
					return static_cast<cell>(i->second);
				}
				return INVALID_OBJECT_ID;
			}
			case STREAMER_TYPE_CP:
			{
				if (p->second.visibleCheckpoint == static_cast<int>(params[3]))
				{
					return 1;
				}
				return -1;
			}
			case STREAMER_TYPE_RACE_CP:
			{
				if (p->second.visibleRaceCheckpoint == static_cast<int>(params[3]))
				{
					return 1;
				}
				return -1;
			}
			case STREAMER_TYPE_MAP_ICON:
			{
				std::unordered_map<int, int>::iterator i = p->second.internalMapIcons.find(static_cast<int>(params[3]));
				if (i != p->second.internalMapIcons.end())
				{
					return static_cast<cell>(i->second);
				}
				return -1;
			}
			case STREAMER_TYPE_3D_TEXT_LABEL:
			{
				std::unordered_map<int, int>::iterator i = p->second.internalTextLabels.find(static_cast<int>(params[3]));
				if (i != p->second.internalTextLabels.end())
				{
					return static_cast<cell>(i->second);
				}
				return INVALID_TEXT_LABEL_ID;
			}
			case STREAMER_TYPE_AREA:
			{
				std::unordered_set<int>::iterator i = p->second.internalAreas.find(static_cast<int>(params[3]));
				if (i != p->second.internalAreas.end())
				{
					return *i;
				}
				return INVALID_STREAMER_ID;
			}
			default:
			{
				Utility::logError("Streamer_GetItemInternalID: Invalid type specified.");
				return -1;
			}
		}
	}
	return -1;
}

cell AMX_NATIVE_CALL Natives::Streamer_GetItemStreamerID(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	switch (static_cast<int>(params[2]))
	{
		case STREAMER_TYPE_PICKUP:
		{
			for (std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalPickups.begin(); i != core->getData()->internalPickups.end(); ++i)
			{
				if (i->second == static_cast<int>(params[3]))
				{
					return i->first.first;
				}
			}
			return INVALID_STREAMER_ID;
		}
		case STREAMER_TYPE_ACTOR:
		{
			for (std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.begin(); i != core->getData()->internalActors.end(); ++i)
			{
				if (i->second == static_cast<int>(params[3]))
				{
					return i->first.first;
				}
			}
			return INVALID_STREAMER_ID;
		}
	}
	std::unordered_map<int, Player>::iterator p = core->getData()->players.find(static_cast<int>(params[1]));
	if (p != core->getData()->players.end())
	{
		switch (static_cast<int>(params[2]))
		{
			case STREAMER_TYPE_OBJECT:
			{
				for (std::unordered_map<int, int>::iterator i = p->second.internalObjects.begin(); i != p->second.internalObjects.end(); ++i)
				{
					if (i->second == static_cast<int>(params[3]))
					{
						return i->first;
					}
				}
				return INVALID_STREAMER_ID;
			}
			case STREAMER_TYPE_CP:
			{
				if (p->second.visibleCheckpoint == static_cast<int>(params[3]))
				{
					return 1;
				}
				return INVALID_STREAMER_ID;
			}
			case STREAMER_TYPE_RACE_CP:
			{
				if (p->second.visibleRaceCheckpoint == static_cast<int>(params[3]))
				{
					return 1;
				}
				return INVALID_STREAMER_ID;
			}
			case STREAMER_TYPE_MAP_ICON:
			{
				for (std::unordered_map<int, int>::iterator i = p->second.internalMapIcons.begin(); i != p->second.internalMapIcons.end(); ++i)
				{
					if (i->second == static_cast<int>(params[3]))
					{
						return i->first;
					}
				}
				return INVALID_STREAMER_ID;
			}
			case STREAMER_TYPE_3D_TEXT_LABEL:
			{
				for (std::unordered_map<int, int>::iterator i = p->second.internalTextLabels.begin(); i != p->second.internalTextLabels.end(); ++i)
				{
					if (i->second == static_cast<int>(params[3]))
					{
						return i->first;
					}
				}
				return INVALID_STREAMER_ID;
			}
			case STREAMER_TYPE_AREA:
			{
				std::unordered_set<int>::iterator i = p->second.internalAreas.find(static_cast<int>(params[3]));
				if (i != p->second.internalAreas.end())
				{
					return *i;
				}
				return INVALID_STREAMER_ID;
			}
			default:
			{
				Utility::logError("Streamer_GetItemStreamerID: Invalid type specified.");
				return INVALID_STREAMER_ID;
			}
		}
	}
	return INVALID_STREAMER_ID;
}
*/
