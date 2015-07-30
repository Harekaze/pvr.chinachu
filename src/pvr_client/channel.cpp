/*
 *         Copylight (C) 2015 Yuki MIZUNO
 *         https://github.com/mzyy94/pvr.chinachu/
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
#include "xbmc/libKODI_guilib.h"
#include "xbmc/libXBMC_addon.h"
#include "xbmc/libXBMC_pvr.h"
#include "chinachu/chinachu.h"

using namespace ADDON;

chinachu::CHANNEL_INFO currentChannel;

extern chinachu::Schedule g_schedule;
extern CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr *PVR;

extern "C" {

int GetChannelsAmount(void) {
	g_schedule.refreshIfNeeded();
	return g_schedule.schedule.size();
}

PVR_ERROR GetChannels(ADDON_HANDLE handle, bool bRadio) {
	if (bRadio) {
		return PVR_ERROR_NO_ERROR;
	}

	g_schedule.refreshIfNeeded();

	for (int i = 0; i < g_schedule.schedule.size(); i++) {
		chinachu::CHANNEL_INFO &channel = g_schedule.schedule[i].channel;

		PVR_CHANNEL ch;
		memset(&ch, 0, sizeof(PVR_CHANNEL));

		ch.iUniqueId         = channel.iUniqueId;
		ch.bIsRadio          = false;
		ch.iChannelNumber    = channel.iChannelNumber;
		strncpy(ch.strChannelName, channel.strChannelName.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
		strncpy(ch.strStreamURL, channel.strStreamURL.c_str(), PVR_ADDON_URL_STRING_LENGTH - 1);
		ch.bIsHidden         = false;
		// strncpy(ch.strInputFormat, "InputFormat", PVR_ADDON_INPUT_FORMAT_STRING_LENGTH - 1); /* not implemented */
		// ch.iEncryptionSystem = 0; /* not implemented */
		// strncpy(ch.strIconPath, "IconPath", PVR_ADDON_URL_STRING_LENGTH - 1); /* not implemented */

		PVR->TransferChannelEntry(handle, &ch);
	}

	return PVR_ERROR_NO_ERROR;
}

int GetCurrentClientChannel(void) {
	return currentChannel.iUniqueId;
}

/* not implemented */
int GetChannelGroupsAmount(void) { return -1; }
PVR_ERROR GetChannelGroups(ADDON_HANDLE handle, bool bRadio) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR GetChannelGroupMembers(ADDON_HANDLE handle, const PVR_CHANNEL_GROUP &group) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR OpenDialogChannelScan(void) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR DeleteChannel(const PVR_CHANNEL &channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR RenameChannel(const PVR_CHANNEL &channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR MoveChannel(const PVR_CHANNEL &channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR OpenDialogChannelSettings(const PVR_CHANNEL &channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR OpenDialogChannelAdd(const PVR_CHANNEL &channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
unsigned int GetChannelSwitchDelay(void) { return 0; }

}
