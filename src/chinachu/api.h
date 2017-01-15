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
#ifndef CHINACHU_API_H
#define CHINACHU_API_H

#include <iostream>
#include "kodi/xbmc_addon_dll.h"

namespace chinachu {
	namespace api {
		extern std::string baseURL;

		// Mirakurun live tv watching url
		extern std::string mirakurunURL;

		int request(std::string apiPath, std::string &response);

		// GET /schedule.json
		int getSchedule(std::string &response);

		// GET /recorded.json
		int getRecorded(std::string &response);

		// GET /recording.json
		int getRecording(std::string &response);

		// GET /reserves.json
		int getReserves(std::string &response);

		// DELETE /recorded/:id.json
		int deleteRecordedProgram(std::string id);

		// DELETE /recording/:id.json
		int deleteRecordingProgram(std::string id);

		// PUT /reserves/:id/skip.json
		int putReservesSkip(std::string id);

		// PUT /reserves/:id/unskip.json
		int putReservesUnskip(std::string id);

		// PUT /program/:id.json
		int putProgram(std::string id);

		// DELETE /reserves/:id.json
		int deleteReserves(std::string id);

		// GET /storage.json
		int getStorage(std::string &response);

	} // namespace api
} // namespace chinachu

#endif /* end of include guard */
