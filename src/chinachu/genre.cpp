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
#include "genre.h"

namespace chinachu {
	std::map<std::string, int> genre;

	void initGenreType() {
		genre["anime"] =       0x01;
		genre["information"] = 0x02;
		genre["news"] =        0x03;
		genre["sports"] =      0x04;
		genre["variety"] =     0x05;
		genre["drama"] =       0x06;
		genre["music"] =       0x07;
		genre["cinema"] =      0x08;
		genre["etc"] =         0x09;
	}

} // namespace chinachu
