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
#include <climits>
#include "kodi/xbmc_pvr_dll.h"
#include "kodi/libKODI_guilib.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include "chinachu/chinachu.h"

#define MENUHOOK_FORCE_REFRESH_RECORDING 0x01
#define MENUHOOK_FORCE_REFRESH_TIMER 0x02
#define MENUHOOK_FORCE_EXECUTE_SCHEDULER 0x04

#define MSG_FORCE_REFRESH_RECORDING 30800
#define MSG_FORCE_REFRESH_TIMER 30801
#define MSG_FORCE_EXECUTE_SCHEDULER 30802

chinachu::Schedule g_schedule;
chinachu::Recorded g_recorded;
chinachu::Recording g_recording;
chinachu::Rule g_rule;
chinachu::Reserve g_reserve;
ADDON::CHelper_libXBMC_addon *XBMC = NULL;
CHelper_libXBMC_pvr *PVR = NULL;
time_t lastStartTime;

ADDON_STATUS currentStatus = ADDON_STATUS_UNKNOWN;

extern "C" {

ADDON_STATUS ADDON_Create(void* callbacks, void* props) {

	if (!callbacks || !props) {
		return ADDON_STATUS_UNKNOWN;
	}

	XBMC = new ADDON::CHelper_libXBMC_addon;
	PVR = new CHelper_libXBMC_pvr;

	if (!XBMC->RegisterMe(callbacks) || !PVR->RegisterMe(callbacks)) {
		delete PVR;
		delete XBMC;
		PVR = NULL;
		XBMC = NULL;
		return ADDON_STATUS_PERMANENT_FAILURE;
	}

	time(&lastStartTime);

	const char* strUserPath = ((PVR_PROPERTIES*)props)->strUserPath;

	if (!XBMC->DirectoryExists(strUserPath)) {
		XBMC->CreateDirectory(strUserPath);
	}

	char serverUrl[1024];
	if (XBMC->GetSetting("server_url", &serverUrl)) {
		chinachu::api::baseURL = serverUrl;
		const std::string httpPrefix = "http://";
		const std::string httpsPrefix = "https://";
		if (chinachu::api::baseURL.substr(0, httpPrefix.size()) != httpPrefix && chinachu::api::baseURL.substr(0, httpsPrefix.size()) != httpsPrefix) {
			if (currentStatus == ADDON_STATUS_UNKNOWN) {
				XBMC->QueueNotification(ADDON::QUEUE_WARNING, XBMC->GetLocalizedString(30600));
				currentStatus = ADDON_STATUS_NEED_SAVEDSETTINGS;
			}
			return currentStatus;
		}
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
		XBMC->Log(ADDON::LOG_NOTICE, "Video transcoding enabled.");

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
		char videoSize[16];
		XBMC->GetSetting("video_size", videoSize);
		snprintf(buffer, buf_len - 1, "&size=%s", videoSize);
		transcodeParams += buffer;
	} else {
		transcodeParams += "&c:v=copy";
	}

	if (XBMC->GetSetting("audio_transcode", &boolValue) && boolValue) {
		XBMC->Log(ADDON::LOG_NOTICE, "Audio transcoding enabled.");

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

	XBMC->Log(ADDON::LOG_NOTICE, "Transcoding parameter: %s", transcodeParams.c_str());

	g_schedule.liveStreamingPath += transcodeParams;
	g_recorded.recordedStreamingPath += transcodeParams;
	g_recording.recordingStreamingPath += transcodeParams;

	PVR_MENUHOOK menuHookRec;
	memset(&menuHookRec, 0, sizeof(PVR_MENUHOOK));
	menuHookRec.iLocalizedStringId = MSG_FORCE_REFRESH_RECORDING;
	menuHookRec.category = PVR_MENUHOOK_ALL;
	menuHookRec.iHookId = MENUHOOK_FORCE_REFRESH_RECORDING;
	PVR->AddMenuHook(&menuHookRec);

	PVR_MENUHOOK menuHookTimer;
	memset(&menuHookTimer, 0, sizeof(PVR_MENUHOOK));
	menuHookTimer.iLocalizedStringId = MSG_FORCE_REFRESH_TIMER;
	menuHookTimer.category = PVR_MENUHOOK_ALL;
	menuHookTimer.iHookId = MENUHOOK_FORCE_REFRESH_TIMER;
	PVR->AddMenuHook(&menuHookTimer);

	PVR_MENUHOOK menuHookScheduler;
	memset(&menuHookScheduler, 0, sizeof(PVR_MENUHOOK));
	menuHookScheduler.iLocalizedStringId = MSG_FORCE_EXECUTE_SCHEDULER;
	menuHookScheduler.category = PVR_MENUHOOK_ALL;
	menuHookScheduler.iHookId = MENUHOOK_FORCE_EXECUTE_SCHEDULER;
	PVR->AddMenuHook(&menuHookScheduler);

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
	currentStatus = ADDON_STATUS_UNKNOWN;
}

// Settings configuration

bool ADDON_HasSettings() {
	return true;
}

unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet) {
	return 0;
}

ADDON_STATUS ADDON_SetSetting(const char *settingName, const void *settingValue) {
	time_t now;
	time(&now);
	if (now - 2 < lastStartTime) {
		return currentStatus;
	}
	if (currentStatus == ADDON_STATUS_OK) {
		return ADDON_STATUS_NEED_RESTART;
	}
	return ADDON_STATUS_PERMANENT_FAILURE;
}

PVR_ERROR CallMenuHook(const PVR_MENUHOOK& menuhook, const PVR_MENUHOOK_DATA &item) {
	if (menuhook.category == PVR_MENUHOOK_ALL && menuhook.iHookId == MENUHOOK_FORCE_REFRESH_RECORDING) {
		PVR->TriggerRecordingUpdate();
		return PVR_ERROR_NO_ERROR;
	}
	if (menuhook.category == PVR_MENUHOOK_ALL && menuhook.iHookId == MENUHOOK_FORCE_REFRESH_TIMER) {
		PVR->TriggerTimerUpdate();
		return PVR_ERROR_NO_ERROR;
	}
	if (menuhook.category == PVR_MENUHOOK_ALL && menuhook.iHookId == MENUHOOK_FORCE_EXECUTE_SCHEDULER) {
		if (chinachu::api::putScheduler() == chinachu::api::REQUEST_FAILED) {
			XBMC->Log(ADDON::LOG_ERROR, "[scheduler.json] Request failed");
			XBMC->QueueNotification(ADDON::QUEUE_ERROR, "[scheduler.json] Request failed");
			return PVR_ERROR_SERVER_ERROR;
		}
		PVR->TriggerTimerUpdate();
		return PVR_ERROR_NO_ERROR;
	}
	return PVR_ERROR_FAILED;
}

/* not implemented */
void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data) {}
void ADDON_FreeSettings(void) {}
void OnSystemSleep() {}
void OnSystemWake() {}
void OnPowerSavingActivated() {}
void OnPowerSavingDeactivated() {}

}
