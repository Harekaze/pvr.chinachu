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
#include <iostream>
#include <cstdio>
#include "kodi/xbmc_pvr_dll.h"
#include "kodi/libKODI_guilib.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include "chinachu/chinachu.h"

using namespace ADDON;

chinachu::Schedule g_schedule;
chinachu::Recorded g_recorded;
chinachu::Recording g_recording;
CHelper_libXBMC_addon *XBMC = NULL;
CHelper_libXBMC_pvr *PVR = NULL;

ADDON_STATUS currentStatus = ADDON_STATUS_UNKNOWN;

extern "C" {

ADDON_STATUS ADDON_Create(void* callbacks, void* props) {

	if (!callbacks || !props) {
		return ADDON_STATUS_UNKNOWN;
	}

	XBMC = new CHelper_libXBMC_addon;
	PVR = new CHelper_libXBMC_pvr;

	if (!XBMC->RegisterMe(callbacks) || !PVR->RegisterMe(callbacks)) {
		delete PVR;
		delete XBMC;
		PVR = NULL;
		XBMC = NULL;
		return ADDON_STATUS_PERMANENT_FAILURE;
	}

	const char* strUserPath = ((PVR_PROPERTIES*)props)->strUserPath;

	if (!XBMC->DirectoryExists(strUserPath)) {
		XBMC->CreateDirectory(strUserPath);
	}

	char serverUrl[1024];
	if (XBMC->GetSetting("server_url", &serverUrl)) {
		chinachu::api::baseURL = serverUrl;
		if (*(chinachu::api::baseURL.end() - 1) != '/') {
			chinachu::api::baseURL += "/";
		}
		chinachu::api::baseURL += "api/";
	}

	g_schedule.liveStreamingPath = "channel/%s/watch.m2ts?ext=m2ts";
	g_schedule.channelLogoPath = "channel/%s/logo.png";
	g_recorded.recordedStreamingPath = "recorded/%s/watch.m2ts?ext=m2ts";
	g_recording.recordingStreamingPath = "recording/%s/watch.m2ts?ext=m2ts";

	int boolValue = 0;
	if (XBMC->GetSetting("show_thumbnail", &boolValue) && boolValue) {
		int intValue = 0;
		char valueString[4];

		XBMC->GetSetting("thumbnail_position", &intValue);
		snprintf(valueString, 4, "%d", intValue);
		g_recorded.recordedThumbnailPath = "recorded/%s/preview.png?pos=";
		g_recorded.recordedThumbnailPath += valueString;
		g_recording.recordingThumbnailPath = "recording/%s/preview.png?pos=";
		g_recording.recordingThumbnailPath += valueString;
	} else {
		g_recorded.recordedThumbnailPath = "";
	}

	std::string transcodeParams = "";

	if (XBMC->GetSetting("video_transcode", &boolValue) && boolValue) {
		XBMC->Log(LOG_NOTICE, "Video transcoding enabled.");

		unsigned int option;
		XBMC->GetSetting("video_codec", &option);
		if (option == 0) {
			transcodeParams += "&c:v=h264";
		} else if (option == 1) {
			transcodeParams += "&c:v=mpeg2video";
		}

		const unsigned int buf_len = 256;
		char buffer[buf_len];

		XBMC->GetSetting("video_bitrate", &option);
		snprintf(buffer, buf_len - 1, "&b:v=%dk", option);
		transcodeParams += buffer;
		XBMC->GetSetting("video_size", &option);
		snprintf(buffer, buf_len - 1, "&size=%dx%d", option, option * 9 / 16);
		transcodeParams += buffer;
	} else {
		transcodeParams += "&c:v=copy";
	}

	if (XBMC->GetSetting("audio_transcode", &boolValue) && boolValue) {
		XBMC->Log(LOG_NOTICE, "Audio transcoding enabled.");

		unsigned int option;
		XBMC->GetSetting("audio_codec", &option);
		if (option == 0) {
			transcodeParams += "&c:a=aac";
		} else if (option == 1) {
			transcodeParams += "&c:a=libvorbis";
		}

		const unsigned int buf_len = 256;
		char buffer[buf_len];

		XBMC->GetSetting("audio_bitrate", &option);
		snprintf(buffer, buf_len - 1, "&b:a=%dk", option);
		transcodeParams += buffer;
	} else {
		transcodeParams += "&c:a=copy";
	}

	XBMC->Log(LOG_NOTICE, "Transcoding parameter: %s", transcodeParams.c_str());

	g_schedule.liveStreamingPath += transcodeParams;
	g_recorded.recordedStreamingPath += transcodeParams;
	g_recording.recordingStreamingPath += transcodeParams;

	if (XBMC->GetSetting("try_playback", &boolValue) && boolValue) {
		g_recording.bPlayback = true;
		XBMC->Log(LOG_NOTICE, "Playback ongoing recording (Beta) enabled");
	} else {
		g_recording.bPlayback = false;
	}

	currentStatus = ADDON_STATUS_OK;

	return currentStatus;
}

ADDON_STATUS ADDON_GetStatus(void) {
	return currentStatus;
}

void ADDON_Destroy(void) {
	currentStatus = ADDON_STATUS_UNKNOWN;
}


void ADDON_Stop(void) {
}

// Settings configuration

bool ADDON_HasSettings() {
	return true;
}

unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet) {
	return 0;
}

ADDON_STATUS ADDON_SetSetting(const char *settingName, const void *settingValue) {
	return ADDON_STATUS_NEED_RESTART;
}

void ADDON_FreeSettings(void) {}

/* not implemented */
void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data) {}
PVR_ERROR CallMenuHook(const PVR_MENUHOOK& menuhook, const PVR_MENUHOOK_DATA &item) { return PVR_ERROR_NOT_IMPLEMENTED; }
void OnSystemSleep() {}
void OnSystemWake() {}
void OnPowerSavingActivated() {}
void OnPowerSavingDeactivated() {}

}
