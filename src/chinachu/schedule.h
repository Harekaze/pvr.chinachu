/*
 *         Copyright (C) 2015-2016 Yuki MIZUNO
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
#ifndef CHINACHU_SCHEDULE_H
#define CHINACHU_SCHEDULE_H
#include <iostream>

#include "picojson/picojson.h"
#include "chinachu/genre.h"
#include "xbmc/xbmc_pvr_types.h"
#include "xbmc/xbmc_epg_types.h"

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
		std::string strGenreDescription;
	};
	struct CHANNEL_INFO {
		unsigned int iUniqueId;
		unsigned int iChannelNumber;
		unsigned int iSubChannelNumber;
		std::string strChannelType;
		std::string strChannelName;
		std::string strStreamURL;
	};
	struct CHANNEL_EPG {
		CHANNEL_INFO channel;
		std::vector<EPG_PROGRAM> epgs;
	};
	class Schedule {
		private:
			time_t lastUpdated;
		public:
			time_t nextUpdateTime;
			std::string liveStreamingPath;
			std::vector<CHANNEL_EPG> schedule;
			std::vector<std::string> groupNames;
			Schedule()
			{
				// refresh(); /* initial refresh */
			}
			std::vector<EPG_PROGRAM> scheduleForChannel(PVR_CHANNEL ch);
			bool refresh();
			bool refreshIfNeeded();
	};
	unsigned int generateUniqueId(time_t time, unsigned int sid);
} // namespace chinachu

#endif /* end of include guard */
