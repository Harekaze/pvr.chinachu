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
#include "api.h"
#include "reserves.h"
#include "rules.h"
#include "recorded.h"
#include "recording.h"
#include "schedule.h"
#include "kodi/libXBMC_addon.h"

extern ADDON::CHelper_libXBMC_addon *XBMC;


namespace chinachu {
	bool Rule::refresh() {
		picojson::value response;

		if (chinachu::api::getRules(response) == chinachu::api::REQUEST_FAILED) {
			return false;
		}

		rules.clear();

		unsigned int i;
		for (picojson::value &a: response.get<picojson::array>()) {
			picojson::object &p = a.get<picojson::object>();

			if (p["ignore_channels"].is<picojson::array>() && !p["ignore_channels"].get<picojson::array>().empty()) {
				XBMC->Log(ADDON::LOG_DEBUG, "Skipped - ignore channels specified rule: %d", i);
				continue;
			}

			if (p["reserve_titles"].is<picojson::array>() && !p["reserve_titles"].get<picojson::array>().empty() &&
				p["reserve_descriptions"].is<picojson::array>() && !p["reserve_descriptions"].get<picojson::array>().empty()) {
					XBMC->Log(ADDON::LOG_DEBUG, "Skipped - multi reserve pattern rule: %d", i);
					continue;
			}
			if (p["ignore_titles"].is<picojson::array>() && !p["ignore_titles"].get<picojson::array>().empty() &&
				p["ignore_descriptions"].is<picojson::array>() && !p["ignore_descriptions"].get<picojson::array>().empty()) {
					XBMC->Log(ADDON::LOG_DEBUG, "Skipped - multi ignore pattern rule: %d", i);
					continue;
			}
			if (p["reserve_titles"].is<picojson::array>() && !p["reserve_titles"].get<picojson::array>().empty() &&
				p["ignore_descriptions"].is<picojson::array>() && !p["ignore_descriptions"].get<picojson::array>().empty()) {
					XBMC->Log(ADDON::LOG_DEBUG, "Skipped - multi reserve/ignore pattern rule: %d", i);
					continue;
			}

			struct RULE_ITEM rule;
			rule.iIndex = i++;

			if (p["reserve_titles"].is<picojson::array>() && !p["reserve_titles"].get<picojson::array>().empty()) {
				rule.bFullTextEpgSearch = false;
				rule.strEpgSearchString = "";
				for (picojson::value &r: p["reserve_titles"].get<picojson::array>()) {
					if (r != *p["reserve_titles"].get<picojson::array>().begin())
						rule.strEpgSearchString += ", ";
					rule.strEpgSearchString += r.get<std::string>();
				}
				if (p["ignore_titles"].is<picojson::array>() && !p["ignore_titles"].get<picojson::array>().empty()) {
					for (picojson::value &r: p["ignore_titles"].get<picojson::array>()) {
						rule.strEpgSearchString += ", -";
						rule.strEpgSearchString += r.get<std::string>();
					}
				}
			} else if (p["reserve_descriptions"].is<picojson::array>() && !p["reserve_descriptions"].get<picojson::array>().empty()) {
				rule.bFullTextEpgSearch = true;
				rule.strEpgSearchString = p["reserve_descriptions"].get<picojson::array>()[0].get<std::string>();
				for (picojson::value &r: p["reserve_descriptions"].get<picojson::array>()) {
					if (r != *p["reserve_descriptions"].get<picojson::array>().begin())
						rule.strEpgSearchString += ", ";
					rule.strEpgSearchString += r.get<std::string>();
				}
				if (p["ignore_descriptions"].is<picojson::array>() && !p["ignore_descriptions"].get<picojson::array>().empty()) {
					for (picojson::value &r: p["ignore_descriptions"].get<picojson::array>()) {
						rule.strEpgSearchString += ", -";
						rule.strEpgSearchString += r.get<std::string>();
					}
				}
			} else {
				XBMC->Log(ADDON::LOG_DEBUG, "Skipped - invalid reserve/ignore pattern rule: %d", i);
				continue;
			}

			std::string strGenreType = "none";
			if (p["categories"].is<picojson::array>()){
				if (p["categories"].get<picojson::array>().size() == 1) {
					strGenreType = p["categories"].get<picojson::array>()[0].get<std::string>();
					rule.iGenreType = chinachu::iGenreTypePair[strGenreType] & chinachu::GENRE_TYPE_MASK;
					rule.iGenreSubType = chinachu::iGenreTypePair[strGenreType] & chinachu::GENRE_SUBTYPE_MASK;
				} else if (p["categories"].get<picojson::array>().size() > 1){
					strGenreType = "any";
				}
			}
			int startTime = 0;
			int endTime = 24;
			if (p["hour"].is<picojson::object>()) {
				startTime = p["hour"].get<picojson::object>()["start"].is<double>() ? (int)(p["hour"].get<picojson::object>()["start"].get<double>()) : 0;
				endTime = p["hour"].get<picojson::object>()["end"].is<double>() ? (int)(p["hour"].get<picojson::object>()["end"].get<double>()) : 0;
			}

			char title[PVR_ADDON_NAME_STRING_LENGTH];
			snprintf(title, PVR_ADDON_NAME_STRING_LENGTH - 1, "#%d: [%s]%d-%d %s", i, strGenreType.c_str(), startTime, endTime, rule.strEpgSearchString.c_str());
			rule.strTitle = title;

			if (p["channels"].is<picojson::array>() && p["channels"].get<picojson::array>().size() == 1) {
				char *endptr;
				rule.iClientChannelUid = strtoul(p["channels"].get<picojson::array>()[0].get<std::string>().c_str(), &endptr, 36) % 100000;
			}

			rule.bIsDisabled = (p["isDisabled"].is<bool>() && p["isDisabled"].get<bool>());
			rule.state = rule.bIsDisabled ? PVR_TIMER_STATE_DISABLED : PVR_TIMER_STATE_SCHEDULED;

			rules.push_back(rule);
		}

		XBMC->Log(ADDON::LOG_NOTICE, "Updated rules: ammount = %d", rules.size());

		return true;
	}
}
