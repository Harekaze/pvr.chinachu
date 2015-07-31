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
#ifndef CHINACHU_GENRE_H
#define CHINACHU_GENRE_H
#include <map>
#include <string>

namespace chinachu {
	static std::map<std::string, int> iGenreType;
	static std::map<std::string, int> iGenreSubType;

	inline void initGenreType() {
		iGenreType["anime"] =          0x50;
		iGenreSubType["anime"] =       0x05;
		iGenreType["information"] =    0x20;
		iGenreSubType["information"] = 0x02;
		iGenreType["news"] =           0x20;
		iGenreSubType["news"] =        0x01;
		iGenreType["sports"] =         0x40;
		iGenreSubType["sports"] =      0x00;
		iGenreType["variety"] =        0x30;
		iGenreSubType["variety"] =     0x02;
		iGenreType["drama"] =          0x10;
		iGenreSubType["drama"] =       0x00;
		iGenreType["music"] =          0x60;
		iGenreSubType["music"] =       0x00;
		iGenreType["cinema"] =         0x70;
		iGenreSubType["cinema"] =      0x06;
		iGenreType["etc"] =            0x00;
		iGenreSubType["etc"] =         0x00;
	}

} // namespace chinachu

#endif /* end of include guard */
