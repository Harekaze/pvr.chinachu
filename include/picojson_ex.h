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
#ifndef PICOJSON_EX_H
#define PICOJSON_EX_H

#include "picojson/picojson.h"

namespace json {
	template<typename T, typename C> C get(picojson::value v) {
		C null;
		return v.is<T>() ? (C)(v.get<T>()) : null;
	}
	template<typename T> T get(picojson::value v) {
		return get<T, T>(v);
	}
	inline void jstrncpy(char *dst, picojson::value src, unsigned int size) { /* JSON value-string copy */
		std::string str = get<std::string>(src);
		str[size - 1] = '\0';
		strncpy(dst, str.c_str(), size);
	}
}

#endif /* end of include guard */
