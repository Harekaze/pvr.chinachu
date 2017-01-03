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
	unsigned int generateUniqueId(time_t time, unsigned int sid) {
		/*
		 * Unique ID format
		 * year:  day  : hour :  min : sid
		 * [0-]:[0-365]:[0-23]:[0-59]:[0-999]:[0-99999]
		 */
		struct tm *t2 = localtime(&time);
		unsigned int id = 0;
		id += t2->tm_year;
		id *= 366;
		id += t2->tm_yday;
		id *= 24;
		id += t2->tm_hour;
		id *= 60;
		id += t2->tm_min;
		id *= 100000;
		id += sid;
		return id;
	}


	bool Schedule::refreshIfNeeded() {
		time_t now;
		time(&now);
		const time_t refreshInterval = 60*60;
		if (schedule.empty() || (now - lastUpdated) > refreshInterval || now > nextUpdateTime)
			return refresh();
		return true;
	}


	bool Schedule::refresh() {
		picojson::value v;
		std::string response;

		chinachu::api::getSchedule(response);
		std::string err = picojson::parse(v, response);
		if (!err.empty()) {
			XBMC->Log(ADDON::LOG_ERROR, "[schedule.json] Failed to parse JSON string: %s", err.c_str());
			return false;
		}

		schedule.clear();
		groupNames.clear();

		picojson::array ca = v.get<picojson::array>();
		for (unsigned int i = 0, c_size = ca.size(); i < c_size; i++) {
			picojson::object &o = ca[i].get<picojson::object>();
			if (o["programs"].get<picojson::array>().size() == 0) {
				continue;
			}

			struct CHANNEL_EPG ch;

			ch.channel.strChannelType = o["type"].get<std::string>();

			// Check whether the channel type is already listed up in groupNames vector.
			if (find(groupNames.begin(), groupNames.end(), ch.channel.strChannelType) == groupNames.end()) {
				// If not, add it.
				groupNames.push_back(ch.channel.strChannelType);
			}

			std::string channel = o["channel"].get<std::string>();
			ch.channel.iUniqueId = o["sid"].is<std::string>() ?
				std::atoi((o["sid"].get<std::string>()).c_str()) :
				(int)(o["sid"].get<double>());

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
			ch.channel.iChannelNumber = std::atoi(channel.c_str());
			ch.channel.iSubChannelNumber = o["sid"].is<std::string>() ?
				std::atoi((o["sid"].get<std::string>()).c_str()) :
				(int)(o["sid"].get<double>());
			// use channel id as name instead when name field isn't available.
			ch.channel.strChannelName = o["name"].is<std::string>() ? o["name"].get<std::string>() : o["id"].get<std::string>();
			char strStreamURL[2048];
			if (!chinachu::api::mirakurunURL.empty()) {
				snprintf(strStreamURL, PVR_ADDON_URL_STRING_LENGTH - 1,
					(const char*)(chinachu::api::mirakurunURL + mirakurunLiveStreamingPath).c_str(),
					o["type"].get<std::string>().c_str(),
					o["channel"].get<std::string>().c_str(),
					o["sid"].is<std::string>() ? std::atoi((o["sid"].get<std::string>()).c_str()) : (int)(o["sid"].get<double>())
				);
			} else {
				snprintf(strStreamURL, PVR_ADDON_URL_STRING_LENGTH - 1,
					(const char*)(chinachu::api::baseURL + liveStreamingPath).c_str(),
					o["id"].get<std::string>().c_str());
			}
			ch.channel.strStreamURL = strStreamURL;

			if (o["hasLogoData"].get<bool>()) {
				char strIconPath[2048];
				snprintf(strIconPath, PVR_ADDON_URL_STRING_LENGTH - 1,
					(const char*)(chinachu::api::baseURL + channelLogoPath).c_str(),
					o["id"].get<std::string>().c_str());
				ch.channel.strIconPath = strIconPath;
			}

			picojson::array pa = o["programs"].get<picojson::array>();
			for (unsigned int j = 0, p_size = pa.size(); j < p_size; j++) {
				picojson::object &p = pa[j].get<picojson::object>();
				struct EPG_PROGRAM epg;

				epg.startTime = (time_t)(p["start"].get<double>() / 1000);
				epg.endTime = (time_t)(p["end"].get<double>() / 1000);
				epg.iUniqueBroadcastId = generateUniqueId(epg.startTime, ch.channel.iSubChannelNumber);
				epg.strUniqueBroadcastId = p["id"].get<std::string>();
				epg.strTitle = p["title"].get<std::string>();
				epg.strPlotOutline = p["subTitle"].get<std::string>();
				epg.strPlot = p["detail"].get<std::string>();
				epg.strOriginalTitle = p["fullTitle"].get<std::string>();
				epg.strGenreDescription = p["category"].is<std::string>() ? p["category"].get<std::string>() : "";
				epg.iEpisodeNumber = o["episode"].is<double>() ? (unsigned int)(o["episode"].get<double>()) : 0;

				ch.epgs.push_back(epg);
			}

			schedule.push_back(ch);
		}

		nextUpdateTime = std::numeric_limits<time_t>::max();
		time(&lastUpdated);

		XBMC->Log(ADDON::LOG_NOTICE, "Updated schedule: channel ammount = %d", schedule.size());

		return true;
	}


	std::vector<EPG_PROGRAM> Schedule::scheduleForChannel(PVR_CHANNEL ch) {
		for (unsigned int i = 0, size = schedule.size(); i < size; i++) {
			if (schedule[i].channel.iUniqueId == ch.iUniqueId) {
				return schedule[i].epgs;
			}
		}
		std::vector<EPG_PROGRAM> epmty;
		return epmty;
	}
}
