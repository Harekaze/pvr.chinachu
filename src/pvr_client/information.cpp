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
#include "kodi/libKODI_guilib.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"

extern ADDON::CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr *PVR;

char serverUrl[1024];

extern "C" {

const char* GetPVRAPIVersion(void) {
	return XBMC_PVR_API_VERSION;
}

const char* GetMininumPVRAPIVersion(void) {
	return XBMC_PVR_MIN_API_VERSION;
}

const char* GetGUIAPIVersion(void) {
	return KODI_GUILIB_API_VERSION;
}

const char* GetMininumGUIAPIVersion(void) {
	return KODI_GUILIB_MIN_API_VERSION;
}

PVR_ERROR GetAddonCapabilities(PVR_ADDON_CAPABILITIES* pCapabilities) {
	pCapabilities->bSupportsEPG                = true;
	pCapabilities->bSupportsTV                 = true;
	pCapabilities->bSupportsRadio              = false;
	pCapabilities->bSupportsRecordings         = true;
	pCapabilities->bSupportsRecordingsUndelete = false;
	pCapabilities->bSupportsTimers             = true;
	pCapabilities->bSupportsChannelGroups      = true;
	pCapabilities->bSupportsChannelScan        = false;
	pCapabilities->bSupportsChannelSettings    = false;
	pCapabilities->bHandlesInputStream         = false;
	pCapabilities->bHandlesDemuxing            = false;
	pCapabilities->bSupportsRecordingPlayCount = false;
	pCapabilities->bSupportsLastPlayedPosition = false;
	pCapabilities->bSupportsRecordingEdl       = false;

	return PVR_ERROR_NO_ERROR;
}

const char* GetConnectionString(void) {
	if (XBMC->GetSetting("server_url", &serverUrl)) {
		return serverUrl;
	}

	return "";
}

const char* GetBackendName(void) {
	return "Chinachu";
}

const char* GetBackendVersion(void) {
	return "gamma";
}

const char* GetBackendHostname(void) {
	return "";
}

}
