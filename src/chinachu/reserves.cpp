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
#include "api.h"
#include "reserves.h"
#include "recorded.h"
#include "recording.h"
#include "schedule.h"
#include "kodi/libXBMC_addon.h"

extern ADDON::CHelper_libXBMC_addon *XBMC;


namespace chinachu {
	bool Reserve::refresh() {
		picojson::value response;

		if (chinachu::api::getReserves(response) == chinachu::api::REQUEST_FAILED) {
			return false;
		}

		reserves.clear();

		time_t now;
		time(&now);

		for (unsigned int i = 0, p_size = response.get<picojson::array>().size(); i < p_size; i++) {
			picojson::object &p = response.get<picojson::array>()[i].get<picojson::object>();
			// Skip past tv program
			if ((p["end"].get<double>() / 1000) < now) {
				continue;
			}
			struct PVR_TIMER resv;
			char *endptr;

			const std::string strSubstrId = p["id"].get<std::string>().substr(p["id"].get<std::string>().size() - 6, 6);
			resv.iEpgUid = strtoul(strSubstrId.c_str(), &endptr, 36);
			resv.iClientIndex = resv.iEpgUid;
			resv.iClientChannelUid = p["channel"].get<picojson::object>()["sid"].is<std::string>() ?
				std::atoi((p["channel"].get<picojson::object>()["sid"].get<std::string>()).c_str()) :
				(int)(p["channel"].get<picojson::object>()["sid"].get<double>());
			strncpy(resv.strTitle, p["fullTitle"].get<std::string>().c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
			strncpy(resv.strSummary, p["detail"].get<std::string>().c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
			strncpy(resv.strDirectory, p["id"].get<std::string>().c_str(), PVR_ADDON_URL_STRING_LENGTH - 1); // instead of strProgramId
			if (p["isConflict"].is<bool>() && p["isConflict"].get<bool>()) {
				resv.state = PVR_TIMER_STATE_CONFLICT_NOK;
			} else if (p["isSkip"].is<bool>() && p["isSkip"].get<bool>()) {
				resv.state = PVR_TIMER_STATE_DISABLED;
			} else {
				resv.state = PVR_TIMER_STATE_SCHEDULED;
			}
			resv.startTime = (time_t)(p["start"].get<double>() / 1000);
			resv.endTime = (time_t)(p["end"].get<double>() / 1000);
			resv.iGenreType = chinachu::iGenreTypePair[p["category"].get<std::string>()] & chinachu::GENRE_TYPE_MASK;
			resv.iGenreSubType = chinachu::iGenreTypePair[p["category"].get<std::string>()] & chinachu::GENRE_SUBTYPE_MASK;
			resv.bStartAnyTime = false;
			resv.bEndAnyTime = false;
			resv.iTimerType = (p["isManualReserved"].is<bool>() && p["isManualReserved"].get<bool>()) ? TIMER_MANUAL_RESERVED : TIMER_PATTERN_MATCHED;

			reserves.push_back(resv);
		}

		XBMC->Log(ADDON::LOG_NOTICE, "Updated reserved program: ammount = %d", reserves.size());

		return true;
	}
}
