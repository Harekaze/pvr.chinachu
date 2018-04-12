/*
 *         Copyright (C) 2015-2018 Yuki MIZUNO
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
#ifndef CHINACHU_RECORDED_H
#define CHINACHU_RECORDED_H
#include <iostream>

#include "picojson/picojson.h"
#include "chinachu/genre.h"
#include "kodi/xbmc_pvr_types.h"

namespace chinachu {
	class Recorded {
		public:
			std::string recordedStreamingPath;
			std::string recordedThumbnailPath;
			std::vector<PVR_RECORDING> programs;
			bool refresh();
	};
} // namespace chinachu

#endif /* end of include guard */
