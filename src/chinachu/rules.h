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
#ifndef CHINACHU_RULES_H
#define CHINACHU_RULES_H
#include <iostream>

#include "picojson/picojson.h"
#include "chinachu/genre.h"
#include "kodi/xbmc_pvr_types.h"

namespace chinachu {
	struct RULE_ITEM {
		unsigned int iIndex;
		std::string strTitle;
		std::string strEpgSearchString;
		unsigned int iClientChannelUid;
		PVR_TIMER_STATE state;
		bool bFullTextEpgSearch;
		bool bIsDisabled;
		int iGenreType;
		int iGenreSubType;
	};
	class Rule {
		public:
			std::vector<RULE_ITEM> rules;
			bool refresh();
	};
} // namespace chinachu

#endif /* end of include guard */
