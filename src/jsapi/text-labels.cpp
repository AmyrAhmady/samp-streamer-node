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

OMPNODE_API(StreamerTextLabel, Create, JSString text, int color, float x, float y, float z, float drawDistance, int attachedPlayer, int attachedVehicle, int testLos, int worldId, int interiorId, int playerId, float streamDistance, int areaId, int priority)
{
	if (core->getData()->getGlobalMaxItems(STREAMER_TYPE_3D_TEXT_LABEL) == core->getData()->textLabels.size())
	{
		int ret = INVALID_STREAMER_ID;
		API_RETURN(int ret);
		return INVALID_STREAMER_ID;
	}

	int textLabelId = Item::TextLabel::identifier.get();
	Item::SharedTextLabel textLabel(new Item::TextLabel);
	textLabel->amx = nullptr; // TODO must be checked if it's used anywhere
	textLabel->textLabelId = textLabelId;
	textLabel->inverseAreaChecking = false;
	textLabel->originalComparableStreamDistance = -1.0f;
	textLabel->positionOffset = Eigen::Vector3f::Zero();
	textLabel->streamCallbacks = false;
	textLabel->text = text;
	textLabel->color = color;
	textLabel->position = Eigen::Vector3f(x, y, z);
	textLabel->drawDistance = drawDistance;
	if (attachedPlayer != INVALID_PLAYER_ID || attachedVehicle != INVALID_VEHICLE_ID)
	{
		textLabel->attach = std::make_shared<Item::TextLabel::Attach>();
		textLabel->attach->player = attachedPlayer;
		textLabel->attach->vehicle = attachedVehicle;
		if (textLabel->position.cwiseAbs().maxCoeff() > 50.0f)
		{
			textLabel->position.setZero();
		}
		core->getStreamer()->attachedTextLabels.insert(textLabel);
	}
	textLabel->testLOS = testLos != 0;
	Utility::addToContainer(textLabel->worlds, worldId);
	Utility::addToContainer(textLabel->interiors, interiorId);
	Utility::addToContainer(textLabel->players, playerId);
	textLabel->comparableStreamDistance = streamDistance < STREAMER_STATIC_DISTANCE_CUTOFF ? streamDistance : streamDistance * streamDistance;
	textLabel->streamDistance = streamDistance;
	Utility::addToContainer(textLabel->areas, areaId);
	textLabel->priority = priority;
	core->getGrid()->addTextLabel(textLabel);
	core->getData()->textLabels.insert(std::make_pair(textLabelId, textLabel));

	int ret = textLabelId;
	API_RETURN(int ret);
}

OMPNODE_API(StreamerTextLabel, Destroy, int id)
{
	bool ret = false;
	std::unordered_map<int, Item::SharedTextLabel>::iterator t = core->getData()->textLabels.find(id);
	if (t != core->getData()->textLabels.end())
	{
		Utility::destroyTextLabel(t);
		ret = true;
	}
	API_RETURN(bool ret);
}

OMPNODE_API(StreamerTextLabel, IsValid, int id)
{
	bool ret = false;
	std::unordered_map<int, Item::SharedTextLabel>::iterator t = core->getData()->textLabels.find(id);
	if (t != core->getData()->textLabels.end())
	{
		ret = true;
	}
	API_RETURN(bool ret);
}

OMPNODE_API(StreamerTextLabel, Update, int id, int color, JSString text)
{
	bool ret = false;
	std::unordered_map<int, Item::SharedTextLabel>::iterator t = core->getData()->textLabels.find(id);
	if (t != core->getData()->textLabels.end())
	{
		t->second->color = color;
		t->second->text = text;
		for (std::unordered_map<int, Player>::iterator p = core->getData()->players.begin(); p != core->getData()->players.end(); ++p)
		{
			std::unordered_map<int, int>::iterator i = p->second.internalTextLabels.find(t->first);
			if (i != p->second.internalTextLabels.end())
			{
				ompgdk::UpdatePlayer3DTextLabelText(p->first, i->second, t->second->color, t->second->text.c_str());
			}
		}
		ret = true;
	};
	API_RETURN(bool ret);
}

/*
cell AMX_NATIVE_CALL Natives::GetDynamic3DTextLabelText(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	std::unordered_map<int, Item::SharedTextLabel>::iterator t = core->getData()->textLabels.find(static_cast<int>(params[1]));
	if (t != core->getData()->textLabels.end())
	{
		cell *text = NULL;
		amx_GetAddr(amx, params[2], &text);
		amx_SetString(text, t->second->text.c_str(), 0, 0, static_cast<size_t>(params[3]));
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::UpdateDynamic3DTextLabelText(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	std::unordered_map<int, Item::SharedTextLabel>::iterator t = core->getData()->textLabels.find(static_cast<int>(params[1]));
	if (t != core->getData()->textLabels.end())
	{
		t->second->color = static_cast<int>(params[2]);
		t->second->text = Utility::convertNativeStringToString(amx, params[3]);
		for (std::unordered_map<int, Player>::iterator p = core->getData()->players.begin(); p != core->getData()->players.end(); ++p)
		{
			std::unordered_map<int, int>::iterator i = p->second.internalTextLabels.find(t->first);
			if (i != p->second.internalTextLabels.end())
			{
				ompgdk::UpdatePlayer3DTextLabelText(p->first, i->second, t->second->color, t->second->text.c_str());
			}
		}
		return 1;
	}
	return 0;
}
*/
