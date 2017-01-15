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
#include "recorded.h"
#include "recording.h"
#include "schedule.h"
#include "kodi/libXBMC_addon.h"

extern ADDON::CHelper_libXBMC_addon *XBMC;


namespace chinachu {
	time_t Recording::nextUpdateTime = 0;
	bool Recording::refreshIfNeeded() {
		if (!bPlayback) {
			return true;
		}
		time_t now;
		time(&now);
		const time_t refreshInterval = 10*60; // every 10 minutes
		if ((now - lastUpdated) > refreshInterval || now > nextUpdateTime)
			return refresh();
		return true;
	}


	bool Recording::refresh() {
		picojson::value v;
		std::string response;

		chinachu::api::getRecording(response);
		std::string err = picojson::parse(v, response);
		if (!err.empty()) {
			XBMC->Log(ADDON::LOG_ERROR, "[recording.json] Failed to parse JSON string: %s", err.c_str());
			return false;
		}

		bool showThumbnail = !recordingThumbnailPath.empty();
		programs.clear();

		picojson::array pa = v.get<picojson::array>();
		for (unsigned int i = 0, p_size = pa.size(); i < p_size; i++) {
			picojson::object &p = pa[i].get<picojson::object>();
			struct RECORDING rec;
			char *endptr;

			rec.strRecordingId = p["id"].is<std::string>() ? p["id"].get<std::string>() : "";
			rec.strDirectory = p["title"].is<std::string>() ? p["title"].get<std::string>() : "";
			rec.strTitle = p["title"].is<std::string>() ? p["title"].get<std::string>() : "";
			rec.strEpisodeName = p["subTitle"].is<std::string>() ? p["subTitle"].get<std::string>() : "";
			rec.strPlotOutline = p["description"].is<std::string>() ? p["description"].get<std::string>() : p["subTitle"].get<std::string>();
			rec.strPlot = p["detail"].is<std::string>() ? p["detail"].get<std::string>() : "";
			rec.strChannelName = (p["channel"].get<picojson::object>())["name"].get<std::string>();
			rec.iEpisodeNumber = p["episode"].is<double>() ? (unsigned int)(p["episode"].get<double>()) : 0;
			rec.recordingTime = (time_t)(p["start"].get<double>() / 1000);
			rec.iDuration = (int)(p["seconds"].get<double>());
			rec.iPriority = p["priority"].is<double>() ? (int)(p["priority"].get<double>()) : 0;
			rec.strGenreDescription = p["category"].get<std::string>();
			int sid = p["channel"].get<picojson::object>()["sid"].is<std::string>() ?
				std::atoi((p["channel"].get<picojson::object>()["sid"].get<std::string>()).c_str()) :
				(int)(p["channel"].get<picojson::object>()["sid"].get<double>());
			rec.iEpgEventId = strtoul(rec.strRecordingId.c_str(), &endptr, 36);
			rec.iChannelUid = sid;
			char urlBuffer[PVR_ADDON_URL_STRING_LENGTH];
			snprintf(urlBuffer, PVR_ADDON_URL_STRING_LENGTH - 1, (const char*)(chinachu::api::baseURL + recordingStreamingPath).c_str(), p["id"].get<std::string>().c_str());
			rec.strStreamURL = urlBuffer;
			if (showThumbnail) {
				snprintf(urlBuffer, PVR_ADDON_URL_STRING_LENGTH - 1, (const char*)(chinachu::api::baseURL + recordingThumbnailPath).c_str(), p["id"].get<std::string>().c_str());
				rec.strThumbnailPath = urlBuffer;
			} else {
				rec.strThumbnailPath = "";
			}

			programs.push_back(rec);
		}

		time(&lastUpdated);
		if (!programs.empty()) {
			nextUpdateTime = (*programs.begin()).recordingTime + (*programs.begin()).iDuration + 10;
		}
		if (nextUpdateTime <= lastUpdated) {
			nextUpdateTime = lastUpdated + 10 * 60;
		}
		chinachu::Recorded::nextUpdateTime = nextUpdateTime;

		XBMC->Log(ADDON::LOG_NOTICE, "Updated recording program: ammount = %d", programs.size());
		XBMC->Log(ADDON::LOG_NOTICE, "Next recording program update at %s", asctime(localtime(&nextUpdateTime)));

		return true;
	}
}
