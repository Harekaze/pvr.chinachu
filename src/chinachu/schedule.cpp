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
#include "schedule.h"
#include "api.h"
#include "xbmc/libXBMC_addon.h"

extern ADDON::CHelper_libXBMC_addon *XBMC;

namespace chinachu {
	unsigned int uniqueId(time_t time, unsigned int type, unsigned int ch, unsigned int sid) {
		/*
		 * Unique ID format
		 * year:  day  : hour :  min : type:channel:sid(0)
		 * [0-]:[0-365]:[0-23]:[0-59]:[0-3]:[0-999]:[0-9]
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
		id *= 4;
		id += type;
		id *= 1000;
		id += ch;
		id *= 10;
		id += (sid % 10);
		return id;
	}


	bool Schedule::refreshIfNeeded() {
		if (schedule.empty()) return refresh();
		return true;
	}


	bool Schedule::refresh() {
		picojson::value v;
		std::string response;

		chinachu::initGenreType();

		chinachu::api::getSchedule(response);
		std::string err = picojson::parse(v, response);
		if (!err.empty()) {
			XBMC->Log(ADDON::LOG_ERROR, "failed to parse JSON string: %s", err.c_str());
			return false;
		}

		schedule.clear();

		picojson::array ca = v.get<picojson::array>();
		for (unsigned int i = 0, c_size = ca.size(); i < c_size; i++) {
			picojson::object &o = ca[i].get<picojson::object>();
			struct CHANNEL_EPG ch;
			std::string channel = json::get<std::string>(o["channel"]);

			// Channel type: GR, BS, CS, other
			unsigned int chType = 0;
			if (json::get<std::string>(o["type"]) == "GR") {
				chType = 0x01;
			} else if (json::get<std::string>(o["type"]) == "BS") {
				chType = 0x02;
			} else if (json::get<std::string>(o["type"]) == "CS") {
				chType = 0x03;
			}

			ch.channel.iUniqueId = std::atoi((json::get<std::string>(o["sid"])).c_str()) * 10 + chType;

			while (channel.find_first_of("0123456789") != 0) {
				channel.erase(channel.begin());
			}
			ch.channel.iChannelNumber = std::atoi(channel.c_str());
			ch.channel.iSubChannelNumber = std::atoi((json::get<std::string>(o["sid"])).c_str());
			ch.channel.strChannelName = json::get<std::string>(o["name"]);
			char strStreamURL[2048];
			snprintf(strStreamURL, PVR_ADDON_URL_STRING_LENGTH - 1, (const char*)(chinachu::api::baseURL + liveStreamingPath).c_str(), json::get<std::string>(o["id"]).c_str());
			ch.channel.strStreamURL = strStreamURL;
			
			picojson::array pa = o["programs"].get<picojson::array>();
			for (unsigned int j = 0, p_size = pa.size(); j < p_size; j++) {
				picojson::object &p = pa[j].get<picojson::object>();
				struct EPG_PROGRAM epg;

				epg.startTime = json::get<double>(p["start"]) / 1000;
				epg.endTime = json::get<double>(p["end"]) / 1000;
				epg.iUniqueBroadcastId = uniqueId(epg.startTime, chType, ch.channel.iChannelNumber, ch.channel.iUniqueId);
				epg.strTitle = json::get<std::string>(p["title"]);
				epg.iChannelNumber = ch.channel.iChannelNumber;
				epg.strPlotOutline = json::get<std::string>(p["subTitle"]);
				epg.strPlot = json::get<std::string>(p["detail"]);
				epg.strOriginalTitle = json::get<std::string>(p["fullTitle"]);
				epg.iGenreType = useOriginalGenreType ? EPG_GENRE_USE_STRING : chinachu::iGenreType[json::get<std::string>(p["category"])];
				epg.iGenreSubType = chinachu::iGenreSubType[json::get<std::string>(p["category"])];
				epg.strGenreDescription = json::get<std::string>(p["category"]);
				epg.iEpisodeNumber = json::get<double>(o["episode"]);

				ch.epgs.push_back(epg);
			}

			schedule.push_back(ch);
		}

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
