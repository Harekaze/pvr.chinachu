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
#include "api.h"
#include "reserves.h"
#include "recorded.h"
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
			if ((json::get<double>(p["end"]) / 1000) < now) {
				continue;
			}
			struct RESERVE_ITEM resv;

			std::string strChannelType = json::get<std::string>((json::get<picojson::object>(p["channel"])["type"]));

			// Channel type: GR, BS, CS, other
			unsigned int chType = 0;
			if (strChannelType == "GR") {
				chType = 0x01;
			} else if (strChannelType == "BS") {
				chType = 0x02;
			} else if (strChannelType == "CS") {
				chType = 0x03;
			}

			resv.iClientChannelUid = std::atoi((json::get<std::string>((json::get<picojson::object>(p["channel"])["sid"]))).c_str()) * 10 + chType;
			resv.strTitle = json::get<std::string>(p["fullTitle"]);
			resv.strSummary = json::get<std::string>(p["detail"]);
			resv.strProgramId = json::get<std::string>(p["id"]);
			if (json::get<bool>(p["isConflict"])) {
				resv.state =  PVR_TIMER_STATE_CONFLICT_NOK;
			} else if (json::get<bool>(p["isSkip"])) {
				resv.state =  PVR_TIMER_STATE_CANCELLED;
			} else {
				resv.state =  PVR_TIMER_STATE_SCHEDULED;
			}
			resv.startTime = json::get<double>(p["start"]) / 1000;
			resv.endTime = json::get<double>(p["end"]) / 1000;
			resv.iGenreType = iGenreType[json::get<std::string>(p["category"])];
			resv.iGenreSubType = iGenreSubType[json::get<std::string>(p["category"])];
			resv.bIsManualReserved = json::get<bool>(p["isManualReserved"]);
			
			reserves.push_back(resv);
		}

		time(&lastUpdated);
		if (!reserves.empty()) {
			nextUpdateTime = (*reserves.begin()).endTime + 10;
		}
		if (nextUpdateTime <= lastUpdated) {
			nextUpdateTime = std::numeric_limits<time_t>::max();
		}
		chinachu::Recorded::nextUpdateTime = nextUpdateTime;

		XBMC->Log(ADDON::LOG_NOTICE, "Updated reserved program: ammount = %d", reserves.size());

		return true;
	}
}
