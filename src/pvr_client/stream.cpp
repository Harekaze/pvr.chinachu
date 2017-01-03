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
#include "xbmc/libKODI_guilib.h"
#include "xbmc/libXBMC_addon.h"
#include "xbmc/libXBMC_pvr.h"

extern ADDON::CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr *PVR;

using namespace ADDON;

extern "C" {

bool CanPauseStream(void) {
	return true;
}

/* not implemented */
PVR_ERROR SignalStatus(PVR_SIGNAL_STATUS &signalStatus) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR GetStreamProperties(PVR_STREAM_PROPERTIES *pProperties) { return PVR_ERROR_NOT_IMPLEMENTED; }
void PauseStream(bool bPaused) {}
bool CanSeekStream(void) { return false; }
bool SeekTime(int time, bool backwards, double *startpts) { return false; }
void SetSpeed(int speed) {}
time_t GetPlayingTime() { return 0; }
time_t GetBufferTimeStart() { return 0; }
time_t GetBufferTimeEnd() { return 0; }

}
