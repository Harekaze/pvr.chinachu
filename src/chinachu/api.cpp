/*
 *         Copyright (C) 2015-2016 Yuki MIZUNO
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
#include "api.h"
#include "xbmc/libXBMC_pvr.h"
#include "xbmc/libXBMC_addon.h"

extern ADDON::CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr *PVR;

namespace chinachu {
	namespace api {
		std::string baseURL = "";

		// Mirakurun live tv watching url
		std::string mirakurunURL = "";

		int requestGET(std::string apiPath, std::string &response) {
			std::string url = baseURL + apiPath;
			if (void* handle = XBMC->OpenFile(url.c_str(), 0)) {
				const unsigned int buffer_size = 4096;
				char buffer[buffer_size];
				response.clear();
				while (int bytesRead = XBMC->ReadFile(handle, buffer, buffer_size)) {
					response.append(buffer, bytesRead);
				}
				XBMC->CloseFile(handle);
			} else {
				return -1;
			}

			return response.length();
		}

		int requestDELETE(std::string apiPath) {
			std::string url = baseURL + apiPath;
			if (void* handle = XBMC->OpenFileForWrite(url.c_str(), 0)) {
				const unsigned int buffer_size = 20;
				char buffer[] = "{\"_method\":\"DELETE\"}";
				XBMC->WriteFile(handle, buffer, buffer_size);
				XBMC->CloseFile(handle);
				return 0;
			} else {
				return -1;
			}
		}

		int requestPUT(std::string apiPath) {
			std::string url = baseURL + apiPath;
			if (void* handle = XBMC->OpenFileForWrite(url.c_str(), 0)) {
				const unsigned int buffer_size = 17;
				char buffer[] = "{\"_method\":\"PUT\"}";
				XBMC->WriteFile(handle, buffer, buffer_size);
				XBMC->CloseFile(handle);
				return 0;
			} else {
				return -1;
			}
		}

		// GET /schedule.json
		int getSchedule(std::string &response) {
			const std::string apiPath = "schedule.json";
			return requestGET(apiPath, response);
		}

		// GET /recorded.json
		int getRecorded(std::string &response) {
			const std::string apiPath = "recorded.json";
			return requestGET(apiPath, response);
		}

		// GET /recording.json
		int getRecording(std::string &response) {
			const std::string apiPath = "recording.json";
			return requestGET(apiPath, response);
		}

		// GET /reserves.json
		int getReserves(std::string &response) {
			const std::string apiPath = "reserves.json";
			return requestGET(apiPath, response);
		}

		// DELETE /recorded/:id/file.m2ts
		int deleteRecordedFile(std::string id) {
			const std::string apiPath = "recorded/" + id + "/file.m2ts";
			return requestDELETE(apiPath);
		}

		// DELETE /recorded/:id.json
		int deleteRecordedInfo(std::string id) {
			const std::string apiPath = "recorded/" + id + ".json";
			return requestDELETE(apiPath);
		}

		// DELETE /recording/:id.json
		int deleteRecordingProgram(std::string id) {
			const std::string apiPath = "recording/" + id + ".json";
			return requestDELETE(apiPath);
		}

		// PUT /reserves/:id/skip.json
		int putReservesSkip(std::string id) {
			const std::string apiPath = "reserves/" + id + "/skip.json";
			return requestPUT(apiPath);
		}

		// PUT /reserves/:id/unskip.json
		int putReservesUnskip(std::string id) {
			const std::string apiPath = "reserves/" + id + "/unskip.json";
			return requestPUT(apiPath);
		}

		// PUT /program/:id.json
		int putProgram(std::string id) {
			const std::string apiPath = "program/" + id + ".json";
			return requestPUT(apiPath);
		}

		// DELETE /reserves/:id.json
		int deleteReserves(std::string id) {
			const std::string apiPath = "reserves/" + id + ".json";
			return requestDELETE(apiPath);
		}

		// GET /storage.json
		int getStorage(std::string &response) {
			const std::string apiPath = "storage.json";
			return requestGET(apiPath, response);
		}

	} // namespace api
} // namespace chinachu
