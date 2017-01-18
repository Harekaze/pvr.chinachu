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
#ifndef CHINACHU_SCHEDULE_H
#define CHINACHU_SCHEDULE_H
#include <iostream>

#include "picojson/picojson.h"
#include "chinachu/genre.h"
#include "kodi/xbmc_pvr_types.h"
#include "kodi/xbmc_epg_types.h"

namespace chinachu {
	struct EPG_PROGRAM {
		unsigned int iUniqueBroadcastId;
		unsigned int iEpisodeNumber;
		time_t startTime;
		time_t endTime;
		std::string strUniqueBroadcastId;
		std::string strTitle;
		std::string strPlotOutline;
		std::string strPlot;
		std::string strOriginalTitle;
		std::string strEpisodeName;
		std::string strGenreDescription;
	};
	struct CHANNEL_INFO {
		unsigned int iUniqueId;
		unsigned int iChannelNumber;
		unsigned int iSubChannelNumber;
		std::string strChannelType;
		std::string strChannelId;
		std::string strChannelName;
		std::string strStreamURL;
		std::string strIconPath;
	};
	struct CHANNEL_EPG {
		CHANNEL_INFO channel;
		std::vector<EPG_PROGRAM> epgs;
	};
	class Schedule {
		private:
			time_t lastUpdated;
			time_t refreshInterval;
		public:
			std::string channelLogoPath;
			std::string liveStreamingPath;
			std::vector<CHANNEL_EPG> schedule;
			std::vector<std::string> groupNames;
			Schedule() : refreshInterval(3600)
			{
			}
			std::vector<EPG_PROGRAM> scheduleForChannel(PVR_CHANNEL ch);
			bool refresh();
			bool refreshIfNeeded();
	};
} // namespace chinachu

#endif /* end of include guard */
