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
#ifndef CHINACHU_GENRE_H
#define CHINACHU_GENRE_H
#include <map>
#include <string>

namespace chinachu {
	static std::map<std::string, unsigned char> iGenreTypePair = {
		{"anime", 0x55},
		{"information", 0x22},
		{"news", 0x21},
		{"sports", 0x40},
		{"variety", 0x32},
		{"drama", 0x10},
		{"music", 0x60},
		{"cinema", 0x76},
		{"documentary", 0x23},
		{"theater", 0x30},
		{"hobby", 0xa0},
		{"welfare", 0x95},
		{"etc", 0x00},
	};
	const unsigned char GENRE_TYPE_MASK = 0xf0;
	const unsigned char GENRE_SUBTYPE_MASK = 0x0f;
} // namespace chinachu

#endif /* end of include guard */
