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
extern chinachu::CHANNEL_INFO currentChannel;
extern ADDON::CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr *PVR;

extern "C" {

PVR_ERROR GetEPGForChannel(ADDON_HANDLE handle, const PVR_CHANNEL &channel, time_t iStart, time_t iEnd) {
	std::map<std::string, int> iGenreType;
	std::map<std::string, int> iGenreSubType;
	chinachu::initGenreType(iGenreType, iGenreSubType);

	for (unsigned int i = 0, lim = g_schedule.schedule.size(); i < lim; i++) {
		if (g_schedule.schedule[i].channel.iUniqueId != channel.iUniqueId) {
			continue;
		}

		for (unsigned int j = 0, lims = g_schedule.schedule[i].epgs.size(); j < lims; j++) {
			const chinachu::EPG_PROGRAM epg = g_schedule.schedule[i].epgs[j];

			if (epg.endTime < iStart) continue;

			EPG_TAG tag;
			memset(&tag, 0, sizeof(EPG_TAG));

			tag.iUniqueBroadcastId = epg.iUniqueBroadcastId;
			tag.strTitle = epg.strTitle.c_str();
			tag.iChannelNumber = g_schedule.schedule[i].channel.iChannelNumber;
			tag.startTime = epg.startTime;
			tag.endTime = epg.endTime;
			tag.strPlotOutline = epg.strPlotOutline.c_str();
			tag.strPlot = epg.strPlot.c_str();
			tag.iGenreType = iGenreType[epg.strGenreDescription];
			tag.iGenreSubType = iGenreSubType[epg.strGenreDescription];
			tag.iEpisodeNumber = epg.iEpisodeNumber;
			tag.strEpisodeName = epg.strEpisodeName.c_str();
			tag.strGenreDescription = epg.strGenreDescription.c_str();

			PVR->TransferEpgEntry(handle, &tag);

			if (epg.startTime > iEnd) break;
		}
	}

	return PVR_ERROR_NO_ERROR;
}

void CloseLiveStream(void) {
}

bool OpenLiveStream(const PVR_CHANNEL &channel) {
	CloseLiveStream();
	currentChannel.iUniqueId = channel.iUniqueId;
	return true;
}

bool SwitchChannel(const PVR_CHANNEL &channel) {
	CloseLiveStream();
	return OpenLiveStream(channel);
}

bool IsRealTimeStream() {
	return true;
}

/* not implemented */
const char* GetLiveStreamURL(const PVR_CHANNEL &channel) { return ""; }
int ReadLiveStream(unsigned char *pBuffer, unsigned int iBufferSize) { return 0; }
long long SeekLiveStream(long long iPosition, int iWhence) { return -1; }
long long PositionLiveStream(void) { return -1; }
long long LengthLiveStream(void) { return -1; }

}
