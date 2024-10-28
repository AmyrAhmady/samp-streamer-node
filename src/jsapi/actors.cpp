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

OMPNODE_API(StreamerActor, Create, int modelid, float x, float y, float z, float r, int invulnerable, float health, int worldid, int interiorid, int playerid, float streamdistance, int areaid, int priority)
{
	if (core->getData()->getGlobalMaxItems(STREAMER_TYPE_ACTOR) == core->getData()->actors.size())
	{
		return INVALID_STREAMER_ID;
	}
	int actorId = Item::Actor::identifier.get();
	Item::SharedActor actor(new Item::Actor);
	actor->amx = nullptr; // TODO must be checked if it's used anywhere
	actor->actorId = actorId;
	actor->inverseAreaChecking = false;
	actor->originalComparableStreamDistance = -1.0f;
	actor->positionOffset = Eigen::Vector3f::Zero();
	actor->modelId = static_cast<int>(modelid);
	actor->position = Eigen::Vector3f(x, y, z);
	actor->rotation = r;
	actor->invulnerable = invulnerable != 0;
	actor->health = health;
	Utility::addToContainer(actor->worlds, worldid);
	Utility::addToContainer(actor->interiors, interiorid);
	Utility::addToContainer(actor->players, playerid);
	actor->comparableStreamDistance = streamdistance < STREAMER_STATIC_DISTANCE_CUTOFF ? streamdistance : streamdistance * streamdistance;
	actor->streamDistance = streamdistance;
	Utility::addToContainer(actor->areas, areaid);
	actor->priority = priority;
	core->getGrid()->addActor(actor);
	core->getData()->actors.insert(std::make_pair(actorId, actor));

	int ret = actorId;
	API_RETURN(int ret);
}

OMPNODE_API(StreamerActor, Destroy, int actorid)
{
	bool ret = false;
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(actorid);
	if (a != core->getData()->actors.end())
	{
		Utility::destroyActor(a);
		ret = true;
	}
	API_RETURN(bool ret);
}

OMPNODE_API(StreamerActor, IsValid, int actorid)
{
	bool ret = false;
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(actorid);
	if (a != core->getData()->actors.end())
	{
		ret = true;
	}
	API_RETURN(bool ret);
}

/*

cell AMX_NATIVE_CALL Natives::IsValidDynamicActor(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::IsDynamicActorStreamedIn(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	std::unordered_map<int, Player>::iterator p = core->getData()->players.find(static_cast<int>(params[2]));
	if (p != core->getData()->players.end())
	{
		int actorId = static_cast<int>(params[1]);

		std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(actorId);
		if (a != core->getData()->actors.end())
		{
			for (std::unordered_set<int>::const_iterator w = a->second->worlds.begin(); w != a->second->worlds.end(); ++w)
			{
				std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.find(std::make_pair(actorId, *w));
				if (i != core->getData()->internalActors.end())
				{
					return ompgdk::IsActorStreamedIn(i->second, p->first);
				}
			}
			return 1;
		}
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::GetDynamicActorVirtualWorld(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		return Utility::getFirstValueInContainer(a->second->worlds);
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::SetDynamicActorVirtualWorld(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		Utility::setFirstValueInContainer(a->second->worlds, static_cast<int>(params[2]));

		for (std::unordered_set<int>::const_iterator w = a->second->worlds.begin(); w != a->second->worlds.end(); ++w)
		{
			std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.find(std::make_pair(a->first, *w));
			if (i != core->getData()->internalActors.end())
			{
				ompgdk::SetActorVirtualWorld(i->second, *w);
			}
		}
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::GetDynamicActorAnimation(AMX *amx, cell *params)
{
	CHECK_PARAMS(11);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		if (a->second->anim)
		{
			Utility::convertStringToNativeString(amx, params[2], params[10], a->second->anim->lib);
			Utility::convertStringToNativeString(amx, params[3], params[11], a->second->anim->name);
			Utility::storeFloatInNative(amx, params[4], a->second->anim->delta);
			Utility::storeIntegerInNative(amx, params[5], a->second->anim->loop != 0);
			Utility::storeIntegerInNative(amx, params[6], a->second->anim->lockx != 0);
			Utility::storeIntegerInNative(amx, params[7], a->second->anim->locky != 0);
			Utility::storeIntegerInNative(amx, params[8], a->second->anim->freeze != 0);
			Utility::storeIntegerInNative(amx, params[9], a->second->anim->time);
			return 1;
		}
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::ApplyDynamicActorAnimation(AMX *amx, cell *params)
{
	CHECK_PARAMS(9);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		a->second->anim = std::make_shared<Item::Actor::Anim>();
		a->second->anim->lib = Utility::convertNativeStringToString(amx, params[2]);
		a->second->anim->name = Utility::convertNativeStringToString(amx, params[3]);
		a->second->anim->delta = amx_ctof(params[4]);
		a->second->anim->loop = static_cast<int>(params[5]) != 0;
		a->second->anim->lockx = static_cast<int>(params[6]) != 0;
		a->second->anim->locky = static_cast<int>(params[7]) != 0;
		a->second->anim->freeze = static_cast<int>(params[8]) != 0;
		a->second->anim->time = static_cast<int>(params[9]);

		for (std::unordered_set<int>::const_iterator w = a->second->worlds.begin(); w != a->second->worlds.end(); ++w)
		{
			std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.find(std::make_pair(a->first, *w));
			if (i != core->getData()->internalActors.end())
			{
				ompgdk::ApplyActorAnimation(i->second, a->second->anim->lib.c_str(), a->second->anim->name.c_str(), a->second->anim->delta, a->second->anim->loop, a->second->anim->lockx, a->second->anim->locky, a->second->anim->freeze, a->second->anim->time);
			}
		}
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::ClearDynamicActorAnimations(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		a->second->anim = NULL;

		for (std::unordered_set<int>::const_iterator w = a->second->worlds.begin(); w != a->second->worlds.end(); ++w)
		{
			std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.find(std::make_pair(a->first, *w));
			if (i != core->getData()->internalActors.end())
			{
				ompgdk::ClearActorAnimations(i->second);
			}
		}
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::GetDynamicActorFacingAngle(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		Utility::storeFloatInNative(amx, params[2], a->second->rotation);
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::SetDynamicActorFacingAngle(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		a->second->rotation = amx_ctof(params[2]);

		for (std::unordered_set<int>::const_iterator w = a->second->worlds.begin(); w != a->second->worlds.end(); ++w)
		{
			std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.find(std::make_pair(a->first, *w));
			if (i != core->getData()->internalActors.end())
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
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::GetDynamicActorPos(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		Utility::storeFloatInNative(amx, params[2], a->second->position[0]);
		Utility::storeFloatInNative(amx, params[3], a->second->position[1]);
		Utility::storeFloatInNative(amx, params[4], a->second->position[2]);
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::SetDynamicActorPos(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		a->second->position[0] = amx_ctof(params[2]);
		a->second->position[1] = amx_ctof(params[3]);
		a->second->position[2] = amx_ctof(params[4]);

		for (std::unordered_set<int>::const_iterator w = a->second->worlds.begin(); w != a->second->worlds.end(); ++w)
		{
			std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.find(std::make_pair(a->first, *w));
			if (i != core->getData()->internalActors.end())
			{
				core->getGrid()->removeActor(a->second, true);

				ompgdk::SetActorPos(i->second, a->second->position[0], a->second->position[1], a->second->position[2]);
			}
		}
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::GetDynamicActorHealth(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		Utility::storeFloatInNative(amx, params[2], a->second->health);
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::SetDynamicActorHealth(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	int actorId = static_cast<int>(params[1]);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(actorId);
	if (a != core->getData()->actors.end())
	{
		a->second->health = amx_ctof(params[2]);

		for (std::unordered_set<int>::const_iterator w = a->second->worlds.begin(); w != a->second->worlds.end(); ++w)
		{
			std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.find(std::make_pair(a->first, *w));
			if (i != core->getData()->internalActors.end())
			{
				ompgdk::SetActorHealth(i->second, a->second->health);
			}
		}
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::SetDynamicActorInvulnerable(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	int actorId = static_cast<int>(params[1]);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(actorId);
	if (a != core->getData()->actors.end())
	{
		a->second->invulnerable = static_cast<int>(params[2]) != 0;

		for (std::unordered_set<int>::const_iterator w = a->second->worlds.begin(); w != a->second->worlds.end(); ++w)
		{
			std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.find(std::make_pair(a->first, *w));
			if (i != core->getData()->internalActors.end())
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
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::IsDynamicActorInvulnerable(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	std::unordered_map<int, Item::SharedActor>::iterator a = core->getData()->actors.find(static_cast<int>(params[1]));
	if (a != core->getData()->actors.end())
	{
		return a->second->invulnerable;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::GetPlayerTargetDynamicActor(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	std::unordered_map<int, Player>::iterator p = core->getData()->players.find(static_cast<int>(params[1]));
	if (p != core->getData()->players.end())
	{
		int actorid = ompgdk::GetPlayerTargetActor(p->second.playerId);
		if (actorid != INVALID_ACTOR_ID)
		{
			for (std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.begin(); i != core->getData()->internalActors.end(); ++i)
			{
				if (i->second == actorid)
				{
					return i->first.first;
				}
			}
		}
	}
	return INVALID_STREAMER_ID;
}

cell AMX_NATIVE_CALL Natives::GetPlayerCameraTargetDynActor(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	std::unordered_map<int, Player>::iterator p = core->getData()->players.find(static_cast<int>(params[1]));
	if (p != core->getData()->players.end())
	{
		int actorid = ompgdk::GetPlayerCameraTargetActor(p->second.playerId);
		if (actorid != INVALID_ACTOR_ID)
		{
			for (std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator i = core->getData()->internalActors.begin(); i != core->getData()->internalActors.end(); ++i)
			{
				if (i->second == actorid)
				{
					return i->first.first;
				}
			}
		}
	}
	return INVALID_STREAMER_ID;
}

*/
