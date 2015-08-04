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
#ifndef CHINACHU_RESERVES_H
#define CHINACHU_RESERVES_H
#include <iostream>

#include "picojson_ex.h"
#include "chinachu/genre.h"
#include "xbmc/xbmc_pvr_types.h"

namespace chinachu {
	struct RESERVE_ITEM {
		std::string strTitle;
		std::string strSummary;
		int iClientChannelUid;
		time_t startTime;
		time_t endTime;
		PVR_TIMER_STATE state;
		bool bIsRepeating;
		int iGenreType;
		int iGenreSubType;
	};
	class Reserve {
		private:
		public:
			std::vector<RESERVE_ITEM> reserves;
			Reserve()
			{
				// refresh(); /* initial refresh */
			}
			bool refreshIfNeeded();
			bool refresh();
	};
} // namespace chinachu

#endif /* end of include guard */
