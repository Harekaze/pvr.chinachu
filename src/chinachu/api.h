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
#ifndef CHINACHU_API_H
#define CHINACHU_API_H

#include <iostream>
#include "picojson/picojson.h"

namespace chinachu {
	namespace api {
		extern const int REQUEST_FAILED;
		extern std::string baseURL;

		// GET /schedule.json
		int getSchedule(picojson::value &response);

		// GET /recorded.json
		int getRecorded(picojson::value &response);

		// GET /recording.json
		int getRecording(picojson::value &response);

		// GET /reserves.json
		int getReserves(picojson::value &response);

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

		// GET /rules.json
		int getRules(picojson::value &response);

		// POST /rules.json
		int postRule(std::string type, std::string channel, std::string title, std::string genre);

		// PUT /rules/:id/:action.json
		int putRuleAction(int id, bool state);

		// DELETE /reserves/:id.json
		int deleteReserves(std::string id);

		// PUT /scheduler.json
		int putScheduler();

		// GET /storage.json
		int getStorage(picojson::value &response);

	} // namespace api
} // namespace chinachu

#endif /* end of include guard */
