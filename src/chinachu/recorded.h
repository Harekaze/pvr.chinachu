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
#ifndef CHINACHU_RECORDED_H
#define CHINACHU_RECORDED_H
#include <iostream>

#include "picojson/picojson.h"
#include "chinachu/genre.h"
#include "xbmc/xbmc_pvr_types.h"

namespace chinachu {
	struct RECORDING {
		std::string strRecordingId;
		std::string strTitle;
		std::string strEpisodeName;
		std::string strPlotOutline;
		std::string strPlot;
		std::string strChannelName;
		std::string strStreamURL;
		std::string strThumbnailPath;
		int iEpisodeNumber;
		time_t recordingTime;
		int iDuration;
		unsigned int iEpgEventId;
		std::string strGenreDescription;
	};
	class Recorded {
		private:
			time_t lastUpdated;
		public:
			static time_t nextUpdateTime;
			std::string recordedStreamingPath;
			std::string recordedThumbnailPath;
			std::vector<RECORDING> programs;
			Recorded()
			{
				// refresh(); /* initial refresh */
			}
			bool refreshIfNeeded();
			bool refresh();
	};
} // namespace chinachu

#endif /* end of include guard */
