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
#include "api.h"
#include "reserves.h"
#include "recorded.h"
#include "recording.h"
#include "schedule.h"
#include "xbmc/libXBMC_addon.h"

extern ADDON::CHelper_libXBMC_addon *XBMC;


namespace chinachu {
	bool Reserve::refreshIfNeeded() {
		time_t now;
		time(&now);
		const time_t refreshInterval = 10*60; // every 10 minutes
		if (reserves.empty() || (now - lastUpdated) > refreshInterval || now > nextUpdateTime)
			return refresh();

		for (std::vector<RESERVE_ITEM>::iterator it = reserves.begin(); it != reserves.end(); it++) {
			if (it->endTime < now) {
				reserves.erase(it);
			}
		}

		return true;
	}


	bool Reserve::refresh() {
		picojson::value v;
		std::string response;

		std::map<std::string, int> iGenreType;
		std::map<std::string, int> iGenreSubType;
		chinachu::initGenreType(iGenreType, iGenreSubType);

		chinachu::api::getReserves(response);
		std::string err = picojson::parse(v, response);
		if (!err.empty()) {
			XBMC->Log(ADDON::LOG_ERROR, "[reserves.json] Failed to parse JSON string: %s", err.c_str());
			return false;
		}

		reserves.clear();

		time_t now;
		time(&now);

		picojson::array pa = v.get<picojson::array>();
		for (unsigned int i = 0, p_size = pa.size(); i < p_size; i++) {

			picojson::object &p = pa[i].get<picojson::object>();
			// Skip past tv program
			if ((p["end"].get<double>() / 1000) < now) {
				continue;
			}
			struct RESERVE_ITEM resv;

			resv.iClientChannelUid = std::atoi((p["channel"].get<picojson::object>())["sid"].get<std::string>().c_str());
			resv.strTitle = p["fullTitle"].get<std::string>();
			resv.strSummary = p["detail"].get<std::string>();
			resv.strProgramId = p["id"].get<std::string>();
			if (p["isConflict"].is<bool>() && p["isConflict"].get<bool>()) {
				resv.state =  PVR_TIMER_STATE_CONFLICT_NOK;
			} else if (p["isSkip"].is<bool>() && p["isSkip"].get<bool>()) {
				resv.state =  PVR_TIMER_STATE_DISABLED;
			} else {
				resv.state =  PVR_TIMER_STATE_SCHEDULED;
			}
			resv.startTime = (time_t)(p["start"].get<double>() / 1000);
			resv.endTime = (time_t)(p["end"].get<double>() / 1000);
			resv.iGenreType = iGenreType[p["category"].get<std::string>()];
			resv.iGenreSubType = iGenreSubType[p["category"].get<std::string>()];
			resv.iEpgUid = chinachu::generateUniqueId(resv.startTime, resv.iClientChannelUid);
			resv.bIsManualReserved = (p["isManualReserved"].is<bool>() && p["isManualReserved"].get<bool>());

			reserves.push_back(resv);
		}

		time(&lastUpdated);
		if (!reserves.empty()) {
			nextUpdateTime = (*reserves.begin()).endTime + 60;
		}
		if (nextUpdateTime <= lastUpdated) {
			nextUpdateTime = lastUpdated + 10 * 60;
		}
		chinachu::Recording::nextUpdateTime = nextUpdateTime - 60;

		XBMC->Log(ADDON::LOG_NOTICE, "Updated reserved program: ammount = %d", reserves.size());
		XBMC->Log(ADDON::LOG_NOTICE, "Next reserved program update at %s", asctime(localtime(&nextUpdateTime)));

		return true;
	}
}
