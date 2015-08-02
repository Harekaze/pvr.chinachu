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
#include "recorded.h"
#include "xbmc/libXBMC_addon.h"

extern ADDON::CHelper_libXBMC_addon *XBMC;


namespace chinachu {
	bool Recorded::refreshIfNeeded() {
		if (programs.empty()) return refresh();
		return true;
	}


	bool Recorded::refresh() {
		picojson::value v;
		std::string response;

		chinachu::initGenreType();

		chinachu::api::getRecorded(response);
		std::string err = picojson::parse(v, response);
		if (!err.empty()) {
			XBMC->Log(ADDON::LOG_ERROR, "failed to parse JSON string: %s", err.c_str());
			return false;
		}

		bool showThumbnail = !recordedThumbnailPath.empty();
		programs.clear();

		picojson::array pa = v.get<picojson::array>();
		for (unsigned int i = 0, p_size = pa.size(); i < p_size; i++) {
			picojson::object &p = pa[i].get<picojson::object>();
			struct RECORDING rec;

			rec.strRecordingId = json::get<std::string>(p["id"]);
			rec.strTitle = json::get<std::string>(p["fullTitle"]);
			rec.strPlotOutline = json::get<std::string>(p["subTitle"]);
			rec.strPlot = json::get<std::string>(p["detail"]);
			rec.strChannelName = json::get<std::string>(json::get<picojson::object>(p["channel"])["name"]);
			rec.recordingTime = json::get<double>(p["start"]) / 1000;
			rec.iDuration = json::get<double>(p["seconds"]);
			rec.iGenreType = chinachu::iGenreType[json::get<std::string>(p["category"])];
			rec.iGenreSubType = chinachu::iGenreSubType[json::get<std::string>(p["category"])];
			char urlBuffer[PVR_ADDON_URL_STRING_LENGTH];
			snprintf(urlBuffer, PVR_ADDON_URL_STRING_LENGTH - 1, (const char*)(chinachu::api::baseURL + recordedStreamingPath).c_str(), json::get<std::string>(p["id"]).c_str());
			rec.strStreamURL = urlBuffer;
			if (showThumbnail) {
				snprintf(urlBuffer, PVR_ADDON_URL_STRING_LENGTH - 1, (const char*)(chinachu::api::baseURL + recordedThumbnailPath).c_str(), json::get<std::string>(p["id"]).c_str());
				rec.strThumbnailPath = urlBuffer;
			} else {
				rec.strThumbnailPath = "";
			}
			
			programs.push_back(rec);
		}
		return true;
	}
}
