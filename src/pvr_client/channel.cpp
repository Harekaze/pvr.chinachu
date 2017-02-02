/*
 *         Copyright (C) 2015-2017 Yuki MIZUNO
 *         https://github.com/Harekaze/pvr.chinachu/
 *
 *
 * This file is part of pvr.chinachu.
 *
 * pvr.chinachu is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * pvr.chinachu is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pvr.chinachu.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <iostream>
#include "kodi/libKODI_guilib.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include "chinachu/chinachu.h"

extern chinachu::Schedule g_schedule;
extern ADDON::CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr *PVR;

extern "C" {

int GetChannelsAmount(void) {
	return g_schedule.schedule.size();
}

PVR_ERROR GetChannels(ADDON_HANDLE handle, bool bRadio) {
	if (bRadio) {
		return PVR_ERROR_NO_ERROR;
	}

	if (!g_schedule.refresh()) {
		return PVR_ERROR_SERVER_ERROR;
	}

	for (const std::pair<std::string, std::vector<PVR_CHANNEL>> schedule: g_schedule.channelGroups) {
		for (const PVR_CHANNEL channel: schedule.second) {
			PVR->TransferChannelEntry(handle, &channel);
		}
	}

	return PVR_ERROR_NO_ERROR;
}

int GetChannelGroupsAmount(void) {
	return g_schedule.channelGroups.size();
}

PVR_ERROR GetChannelGroups(ADDON_HANDLE handle, bool bRadio) {
	for (const std::pair<std::string, std::vector<PVR_CHANNEL>> channelGroup: g_schedule.channelGroups) {
		PVR_CHANNEL_GROUP chGroup;
		memset(&chGroup, 0, sizeof(PVR_CHANNEL_GROUP));

		strncpy(chGroup.strGroupName, channelGroup.first.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
		chGroup.bIsRadio = false;
		// chGroup.iPosition = 0; /* not implemented */

		PVR->TransferChannelGroup(handle, &chGroup);
	}

	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR GetChannelGroupMembers(ADDON_HANDLE handle, const PVR_CHANNEL_GROUP &group) {
	for (const PVR_CHANNEL channel: g_schedule.channelGroups[group.strGroupName]) {
		PVR_CHANNEL_GROUP_MEMBER chMem;
		memset(&chMem, 0, sizeof(PVR_CHANNEL_GROUP_MEMBER));

		chMem.iChannelUniqueId = channel.iUniqueId;
		chMem.iChannelNumber = channel.iChannelNumber;
		strncpy(chMem.strGroupName, group.strGroupName, PVR_ADDON_NAME_STRING_LENGTH - 1);

		PVR->TransferChannelGroupMember(handle, &chMem);
	}

	return PVR_ERROR_NO_ERROR;
}

/* not implemented */
PVR_ERROR OpenDialogChannelScan(void) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR DeleteChannel(const PVR_CHANNEL &channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR RenameChannel(const PVR_CHANNEL &channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR MoveChannel(const PVR_CHANNEL &channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR OpenDialogChannelSettings(const PVR_CHANNEL &channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR OpenDialogChannelAdd(const PVR_CHANNEL &channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
unsigned int GetChannelSwitchDelay(void) { return 0; }
PVR_ERROR SetEPGTimeFrame(int iDays) { return PVR_ERROR_NOT_IMPLEMENTED; }

}
