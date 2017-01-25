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
#include "chinachu/chinachu.h"

#if defined(_WIN32) || defined(_WIN64)
#define sleep(sec) Sleep(sec)
#else
#include <unistd.h>
#endif

extern chinachu::Recorded g_recorded;
extern chinachu::Recording g_recording;
extern ADDON::CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr *PVR;

extern "C" {

int GetRecordingsAmount(bool deleted) {
	return g_recorded.programs.size();
}

PVR_ERROR GetRecordings(ADDON_HANDLE handle, bool deleted) {
	if (g_recorded.refresh()) {
		for (unsigned int i = 0, lim = g_recorded.programs.size(); i < lim; i++) {
			const chinachu::RECORDING rec = g_recorded.programs[i];

			PVR_RECORDING pvr_rec;
			memset(&pvr_rec, 0, sizeof(PVR_RECORDING));

			strncpy(pvr_rec.strRecordingId, rec.strRecordingId.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
			strncpy(pvr_rec.strTitle, rec.strTitle.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
			strncpy(pvr_rec.strEpisodeName, rec.strEpisodeName.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
			strncpy(pvr_rec.strPlotOutline, rec.strPlotOutline.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
			strncpy(pvr_rec.strPlot, rec.strPlot.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
			strncpy(pvr_rec.strChannelName, rec.strChannelName.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
			pvr_rec.recordingTime = rec.recordingTime;
			pvr_rec.iEpisodeNumber = rec.iEpisodeNumber;
			pvr_rec.iDuration = rec.iDuration;
			pvr_rec.iPriority = rec.iPriority;
			pvr_rec.iGenreType = chinachu::iGenreTypePair[rec.strGenreDescription] ^ chinachu::GENRE_TYPE_MASK;
			pvr_rec.iGenreSubType = chinachu::iGenreTypePair[rec.strGenreDescription] ^ chinachu::GENRE_SUBTYPE_MASK;
			pvr_rec.iEpgEventId = rec.iEpgEventId;
			pvr_rec.iChannelUid = rec.iChannelUid;
			pvr_rec.channelType = PVR_RECORDING_CHANNEL_TYPE_TV;
			strncpy(pvr_rec.strStreamURL, rec.strStreamURL.c_str(), PVR_ADDON_URL_STRING_LENGTH - 1);
			strncpy(pvr_rec.strThumbnailPath, rec.strThumbnailPath.c_str(), PVR_ADDON_URL_STRING_LENGTH - 1);
			strncpy(pvr_rec.strDirectory, rec.strDirectory.c_str(), PVR_ADDON_URL_STRING_LENGTH - 1);
			// pvr_rec.iSeriesNumber = 0; /* not implemented */
			// pvr_rec.iYear = 0; /* not implemented */
			// strncpy(pvr_rec.strIconPath, "IconPath", PVR_ADDON_URL_STRING_LENGTH - 1); /* not implemented */
			// strncpy(pvr_rec.strFanartPath, "FanartPath", PVR_ADDON_URL_STRING_LENGTH - 1); /* not implemented */
			// pvr_rec.iPriority = 100; /* not implemented */
			// pvr_rec.iLifetime = 0; /* not implemented */
			// pvr_rec.iPlayCount = 0; /* not implemented */
			// pvr_rec.iLastPlayedPosition = 0; /* not implemented */

			PVR->TransferRecordingEntry(handle, &pvr_rec);

		}

		return PVR_ERROR_NO_ERROR;
	}

	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR DeleteRecording(const PVR_RECORDING &recording) {
	if (chinachu::api::deleteRecordedProgram(recording.strRecordingId) != chinachu::api::REQUEST_FAILED) {
		XBMC->Log(ADDON::LOG_NOTICE, "Delete recording: %s", recording.strRecordingId);
		sleep(1);
		PVR->TriggerRecordingUpdate();
		return PVR_ERROR_NO_ERROR;
	}
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR GetDriveSpace(long long *iTotal, long long *iUsed) {
	picojson::value v;
	std::string response;
	const time_t refreshInterval = 10*60; // every 10 minutes
	static time_t lastUpdated;
	time_t now;
	static long long total, used;

	time(&now);
	if (now - lastUpdated < refreshInterval) {
		*iTotal = total;
		*iUsed = used;
		return PVR_ERROR_NO_ERROR;
	}

	if (chinachu::api::getStorage(response) == chinachu::api::REQUEST_FAILED) {
		XBMC->Log(ADDON::LOG_ERROR, "[storage.json] Request failed");
		return PVR_ERROR_SERVER_ERROR;
	}
	const std::string err = picojson::parse(v, response);
	if (!err.empty()) {
		XBMC->Log(ADDON::LOG_ERROR, "[storage.json] Failed to parse JSON string: %s", err.c_str());
		return PVR_ERROR_SERVER_ERROR;
	}

	picojson::object &o = v.get<picojson::object>();
	total = (long long)(o["size"].get<double>() / 1024);
	used = (long long)(o["used"].get<double>() / 1024);
	*iTotal = total;
	*iUsed = used;

	lastUpdated = now;
	return PVR_ERROR_NO_ERROR;
}

/* not implemented */
PVR_ERROR UndeleteRecording(const PVR_RECORDING& recording) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR DeleteAllRecordingsFromTrash(void) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR RenameRecording(const PVR_RECORDING &recording) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetRecordingPlayCount(const PVR_RECORDING &recording, int count) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetRecordingLastPlayedPosition(const PVR_RECORDING &recording, int lastplayedposition) { return PVR_ERROR_NOT_IMPLEMENTED; }
int GetRecordingLastPlayedPosition(const PVR_RECORDING &recording) { return -1; }
PVR_ERROR GetRecordingEdl(const PVR_RECORDING&, PVR_EDL_ENTRY[], int*) { return PVR_ERROR_NOT_IMPLEMENTED; };
bool OpenRecordedStream(const PVR_RECORDING &recording) { return false; }
void CloseRecordedStream(void) {}
int ReadRecordedStream(unsigned char *pBuffer, unsigned int iBufferSize) { return 0; }
long long SeekRecordedStream(long long iPosition, int iWhence) { return 0; }
long long PositionRecordedStream(void) { return -1; }
long long LengthRecordedStream(void) { return 0; }

}
