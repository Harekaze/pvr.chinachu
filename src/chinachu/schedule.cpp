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
#include "schedule.h"
#include "api.h"
#include "kodi/libXBMC_addon.h"

extern ADDON::CHelper_libXBMC_addon *XBMC;

namespace chinachu {
	bool Schedule::refresh() {
		picojson::value response;
		const time_t refreshInterval = 10*60; // every 10 minutes
		static time_t lastUpdated;
		time_t now;

		time(&now);
		if (now - lastUpdated < refreshInterval) {
			return true;
		}

		if (chinachu::api::getSchedule(response) == chinachu::api::REQUEST_FAILED) {
			return false;
		}

		schedule.clear();
		channelGroups.clear();

		for (unsigned int i = 0, c_size = response.get<picojson::array>().size(); i < c_size; i++) {
			picojson::object &o = response.get<picojson::array>()[i].get<picojson::object>();
			if (o["programs"].get<picojson::array>().size() == 0) {
				continue;
			}

			PVR_CHANNEL ch;
			char *endptr;

			ch.iUniqueId = o["sid"].is<double>() ? (int)(o["sid"].get<double>()) : 0;
			ch.bIsHidden = false;
			ch.bIsRadio = false;

			std::string channel = o["channel"].is<std::string>() ? o["channel"].get<std::string>() : "0";
			// Remove non-numerical charactor from channel number
			while (channel.find_first_of("0123456789") != 0) {
				channel.erase(channel.begin());
				// If channel number contains only charactors,
				if (channel.empty()) {
					// use sid instead.
					channel = o["sid"].is<std::string>() ?
						std::atoi((o["sid"].get<std::string>()).c_str()) :
						(int)(o["sid"].get<double>());
					break;
				}
			}
			ch.iChannelNumber = std::atoi(channel.c_str());

			ch.iSubChannelNumber = o["nid"].is<double>() ? (int)(o["nid"].get<double>()) : 0;
			// use channel id as name instead when name field isn't available.
			strncpy(ch.strChannelName, o["name"].is<std::string>() ? o["name"].get<std::string>().c_str() : o["id"].get<std::string>().c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
			snprintf(ch.strStreamURL, PVR_ADDON_URL_STRING_LENGTH - 1,
				(const char*)(chinachu::api::baseURL + liveStreamingPath).c_str(),
				o["id"].get<std::string>().c_str());

			if (o["hasLogoData"].get<bool>()) {
				snprintf(ch.strIconPath, PVR_ADDON_URL_STRING_LENGTH - 1,
					(const char*)(chinachu::api::baseURL + channelLogoPath).c_str(),
					o["id"].get<std::string>().c_str());
			} else {
				ch.strIconPath[0] = '\0';
			}

			const std::string strChannelType = o["type"].get<std::string>();
			channelGroups[strChannelType].push_back(ch);

			for (unsigned int j = 0, p_size = o["programs"].get<picojson::array>().size(); j < p_size; j++) {
				picojson::object &p = o["programs"].get<picojson::array>()[j].get<picojson::object>();
				struct EPG_PROGRAM epg;
				char *endptr;

				epg.startTime = (time_t)(p["start"].get<double>() / 1000);
				epg.endTime = (time_t)(p["end"].get<double>() / 1000);
				epg.strUniqueBroadcastId = p["id"].get<std::string>();
				epg.iUniqueBroadcastId = strtoul(epg.strUniqueBroadcastId.c_str(), &endptr, 36);
				epg.strTitle = p["title"].get<std::string>();
				epg.strEpisodeName = p["subTitle"].is<std::string>() ? p["subTitle"].get<std::string>() : "";
				epg.strPlotOutline = p["description"].is<std::string>() ? p["description"].get<std::string>() : p["subTitle"].get<std::string>();
				epg.strPlot = p["detail"].is<std::string>() ? p["detail"].get<std::string>() : "";
				epg.strOriginalTitle = p["fullTitle"].is<std::string>() ? p["fullTitle"].get<std::string>() : "";
				epg.strGenreDescription = p["category"].is<std::string>() ? p["category"].get<std::string>() : "";
				epg.iEpisodeNumber = p["episode"].is<double>() ? (unsigned int)(p["episode"].get<double>()) : 0;

				schedule[ch.iUniqueId].push_back(epg);
			}
		}

		XBMC->Log(ADDON::LOG_NOTICE, "Updated schedule: channel ammount = %d", schedule.size());

		lastUpdated = now;
		return true;
	}
}
