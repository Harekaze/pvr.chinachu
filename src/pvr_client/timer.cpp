/*
 *         Copyright (C) 2015-2018 Yuki MIZUNO
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
#include <climits>
#include "kodi/libKODI_guilib.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include "chinachu/chinachu.h"

#if defined(_WIN32) || defined(_WIN64)
#define sleep(sec) Sleep(sec)
#else
#include <unistd.h>
#endif

#define CREATE_TIMER_MANUAL_RESERVED 0x11
#define CREATE_RULES_PATTERN_MATCHED 0x12
#define RULES_PATTERN_MATCHED 0x22

#define MSG_TIMER_MANUAL_RESERVED 30900
#define MSG_TIMER_PATTERN_MATCHED 30901
#define MSG_RULES_PATTERN_MATCHED_CREATION 30902
#define MSG_RULES_PATTERN_MATCHED 30903

#define TIMER_CLIENT_START_INDEX 1

extern ADDON::CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr *PVR;
extern chinachu::Recorded g_recorded;
extern chinachu::Recording g_recording;
extern chinachu::Schedule g_schedule;
extern chinachu::Rule g_rule;
extern chinachu::Reserve g_reserve;

std::string channel_id_string(unsigned int nid, unsigned int sid) {
	const static std::string base36 = "0123456789abcdefghijklmnopqrstuvwxyz";
	unsigned int val = nid * 100000 + sid;
	std::string result;
	result.reserve(14);
	do {
		result = base36[val % 36] + result;
	} while (val /= 36);
	return result;
}

extern "C" {

int GetTimersAmount(void) {
	return g_rule.rules.size() + g_reserve.reserves.size();
}

PVR_ERROR GetTimers(ADDON_HANDLE handle) {
	if (g_rule.refresh() && g_reserve.refresh() && g_recording.refresh()) {
		time_t now;
		time(&now);
		unsigned int index = 0;

		for (const chinachu::RULE_ITEM rule: g_rule.rules) {
			PVR_TIMER timer;
			memset(&timer, 0, sizeof(PVR_TIMER));

			timer.iClientIndex = index + TIMER_CLIENT_START_INDEX;
			timer.state = rule.state;
			strncpy(timer.strTitle, rule.strTitle.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
			timer.iClientChannelUid = rule.iClientChannelUid;
			timer.iGenreType = rule.iGenreType;
			timer.iGenreSubType = rule.iGenreSubType;
			timer.iTimerType = RULES_PATTERN_MATCHED;
			timer.bStartAnyTime = true;
			timer.bEndAnyTime = true;
			strncpy(timer.strEpgSearchString, rule.strEpgSearchString.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
			strncpy(timer.strSummary, rule.strEpgSearchString.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
			timer.bFullTextEpgSearch = rule.bFullTextEpgSearch;

			PVR->TransferTimerEntry(handle, &timer);
			index++;
		}

		for (PVR_TIMER timer: g_reserve.reserves) {
			if (timer.state == PVR_TIMER_STATE_SCHEDULED) {
				if (now < timer.startTime) {
					timer.state = PVR_TIMER_STATE_SCHEDULED;
				} else if (timer.startTime < now && now < timer.endTime) {
					timer.state = PVR_TIMER_STATE_ABORTED;
					for (const PVR_RECORDING program: g_recording.programs) {
						if (timer.iEpgUid == program.iEpgEventId) {
							timer.state = PVR_TIMER_STATE_RECORDING;
							break;
						}
					}
				} else {
					timer.state = PVR_TIMER_STATE_COMPLETED;
				}
			}

			PVR->TransferTimerEntry(handle, &timer);
		}

		return PVR_ERROR_NO_ERROR;
	}

	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR UpdateTimer(const PVR_TIMER &timer) {
	if (timer.iTimerType == RULES_PATTERN_MATCHED) {
		const unsigned int index = timer.iClientIndex - TIMER_CLIENT_START_INDEX;
		const chinachu::RULE_ITEM rule = g_rule.rules[index];

		// Only rule availability changing is supported
		if (timer.state != rule.state) {
			switch (timer.state) {
				case PVR_TIMER_STATE_SCHEDULED:
					if (chinachu::api::putRuleAction(rule.iIndex, true) != chinachu::api::REQUEST_FAILED) {
						XBMC->Log(ADDON::LOG_NOTICE, "Enable rule: #%d", index);
						break;
					}
					XBMC->Log(ADDON::LOG_ERROR, "Failed to enable rule: #%d", index);
					XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Failed to enable rule: #%d", index);
					return PVR_ERROR_SERVER_ERROR;
				case PVR_TIMER_STATE_DISABLED:
					if (chinachu::api::putRuleAction(rule.iIndex, false) != chinachu::api::REQUEST_FAILED) {
						XBMC->Log(ADDON::LOG_NOTICE, "Disable rule: #%d", index);
						break;
					}
					XBMC->Log(ADDON::LOG_ERROR, "Failed to disable rule: #%d", index);
					XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Failed to disable rule: #%d", index);
					return PVR_ERROR_SERVER_ERROR;
				default:
					XBMC->Log(ADDON::LOG_ERROR, "Unknown state change: #%d", index);
					XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Unknown state change: #%d", index);
					return PVR_ERROR_NOT_IMPLEMENTED;
			}

			sleep(1);
			PVR->TriggerTimerUpdate();
			return PVR_ERROR_NO_ERROR;
		}

		XBMC->Log(ADDON::LOG_DEBUG, "Nothing to do - Only state change is supported: #%d", index);
		return PVR_ERROR_NO_ERROR;
	}

	int index = -1;
	for (unsigned int i = 0, lim = g_reserve.reserves.size(); i < lim; i++) {
		if (g_reserve.reserves[i].iClientIndex == timer.iClientIndex) {
			index = i;
			break;
		}
	}
	if (index < 0) {
		XBMC->Log(ADDON::LOG_ERROR, "No timer found: %d", timer.iClientIndex);
		XBMC->QueueNotification(ADDON::QUEUE_ERROR, "No timer found: %d", timer.iClientIndex);
		return PVR_ERROR_FAILED;
	}

	const PVR_TIMER resv = g_reserve.reserves[index];
	// Only reserving state changing is supported
	if (timer.iTimerType != resv.iTimerType) {
		XBMC->Log(ADDON::LOG_ERROR, "Unsupport timer type change: %s", timer.strDirectory);
		XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Unsupport timer type change: %s", timer.strDirectory);
		return PVR_ERROR_NOT_IMPLEMENTED;
	}
	if (timer.state != resv.state) {
		switch (timer.state) {
			case PVR_TIMER_STATE_SCHEDULED:
				if (chinachu::api::putReservesUnskip(timer.strDirectory) != chinachu::api::REQUEST_FAILED) {
					XBMC->Log(ADDON::LOG_NOTICE, "Unskip reserving: %s", timer.strDirectory);
					break;
				}
				XBMC->Log(ADDON::LOG_ERROR, "Failed to enable state: %s", timer.strDirectory);
				XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Failed to enable state: %s", timer.strDirectory);
				return PVR_ERROR_SERVER_ERROR;
			case PVR_TIMER_STATE_DISABLED:
				if (chinachu::api::putReservesSkip(timer.strDirectory) != chinachu::api::REQUEST_FAILED) {
					XBMC->Log(ADDON::LOG_NOTICE, "Skip reserving: %s", timer.strDirectory);
					break;
				}
				XBMC->Log(ADDON::LOG_ERROR, "Failed to disable state: %s", timer.strDirectory);
				XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Failed to disable state: %s", timer.strDirectory);
				return PVR_ERROR_SERVER_ERROR;
			default:
				XBMC->Log(ADDON::LOG_ERROR, "Unknown state change: %s", timer.strDirectory);
				XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Unknown state change: %s", timer.strDirectory);
				return PVR_ERROR_NOT_IMPLEMENTED;
		}

		sleep(1);
		PVR->TriggerTimerUpdate();
		return PVR_ERROR_NO_ERROR;
	}

	XBMC->Log(ADDON::LOG_DEBUG, "Nothing to do - Only state change is supported: %s", timer.strDirectory);
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR AddTimer(const PVR_TIMER &timer) {
	for (const std::pair<unsigned int, std::vector<chinachu::EPG_PROGRAM>> schedule: g_schedule.schedule) {
		if (schedule.first == timer.iClientChannelUid) {
			if (timer.iTimerType == CREATE_RULES_PATTERN_MATCHED) {
				std::string genre;
				bool isLive = false;
				for (const chinachu::EPG_PROGRAM program: schedule.second) {
					if (program.startTime == timer.startTime && program.endTime == timer.endTime) {
						genre = program.strGenreDescription;
						time_t now;
						time(&now);
						if (program.startTime < now && now < program.endTime) { // Ongoing recording
							isLive = true;
						}
						break;
					}
				}
				std::string strChannelId;
				std::string strChannelType;
				for (const std::pair<std::string, std::vector<PVR_CHANNEL>> channelGroups: g_schedule.channelGroups) {
					for (const PVR_CHANNEL channel: channelGroups.second) {
						if (channel.iUniqueId == timer.iClientChannelUid) {
							strChannelType = channelGroups.first;
							strChannelId = channel_id_string(channel.iSubChannelNumber, channel.iUniqueId);
							break;
						}
					}
					if (!strChannelId.empty()) {
						break;
					}
				}
				if (chinachu::api::postRule(strChannelType, strChannelId, timer.strEpgSearchString, genre) != chinachu::api::REQUEST_FAILED) {
					XBMC->Log(ADDON::LOG_NOTICE, "Create new rule: [%s:%s]<%s> \"%s\"",
						strChannelType.c_str(), strChannelId.c_str(), genre.c_str(), timer.strEpgSearchString);
					sleep(isLive ? 5 : 1);
					PVR->TriggerTimerUpdate();
					return PVR_ERROR_NO_ERROR;
				} else {
					XBMC->Log(ADDON::LOG_ERROR, "Failed to create new rule: %s", strChannelId.c_str());
					XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Failed to create new rule: %s", strChannelId.c_str());
					return PVR_ERROR_SERVER_ERROR;
				}
				return PVR_ERROR_NO_ERROR;
			}
			for (const chinachu::EPG_PROGRAM program: schedule.second) {
				if (program.startTime == timer.startTime && program.endTime == timer.endTime) {
					if (chinachu::api::putProgram(program.strUniqueBroadcastId) != chinachu::api::REQUEST_FAILED) {
						XBMC->Log(ADDON::LOG_NOTICE, "Reserved new program: %s", program.strUniqueBroadcastId.c_str());
						bool isLive = false;
						time_t now;
						time(&now);
						if (program.startTime < now && now < program.endTime) { // Ongoing recording
							isLive = true;
						}
						sleep(isLive ? 5 : 1);
						PVR->TriggerTimerUpdate();
						return PVR_ERROR_NO_ERROR;
					} else {
						XBMC->Log(ADDON::LOG_ERROR, "Failed to reserve new program: %s", program.strUniqueBroadcastId.c_str());
						XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Failed to reserve new program: %s", program.strUniqueBroadcastId.c_str());
						return PVR_ERROR_SERVER_ERROR;
					}
				}
			}
			break;
		}
	}

	XBMC->Log(ADDON::LOG_ERROR, "Failed to reserve new program: nothing matched");
	XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Failed to reserve new program: nothing matched");
	return PVR_ERROR_FAILED;
}

PVR_ERROR DeleteTimer(const PVR_TIMER &timer, bool bForceDelete) {
	if (timer.iTimerType == TIMER_MANUAL_RESERVED || timer.iTimerType == TIMER_PATTERN_MATCHED) {
		for (const std::pair<unsigned int, std::vector<chinachu::EPG_PROGRAM>> schedule: g_schedule.schedule) {
			if (schedule.first == timer.iClientChannelUid) {
				for (const chinachu::EPG_PROGRAM program: schedule.second) {
					if (program.startTime == timer.startTime && program.endTime == timer.endTime) {
						time_t now;
						time(&now);
						if (program.startTime < now && now < program.endTime) { // Ongoing recording
							if (chinachu::api::deleteRecordingProgram(program.strUniqueBroadcastId) != chinachu::api::REQUEST_FAILED) { // Cancel recording
								XBMC->Log(ADDON::LOG_NOTICE, "Cancel ongoing recording program: %s", program.strUniqueBroadcastId.c_str());
								sleep(5);
								PVR->TriggerRecordingUpdate();
								PVR->TriggerTimerUpdate();
								return PVR_ERROR_NO_ERROR;
							} else {
								XBMC->Log(ADDON::LOG_ERROR, "Failed to cancel recording program: %s", program.strUniqueBroadcastId.c_str());
								XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Failed to cancel recording program: %s", program.strUniqueBroadcastId.c_str());
								return PVR_ERROR_SERVER_ERROR;
							}
						} else if (timer.iTimerType == TIMER_MANUAL_RESERVED) {
							if (chinachu::api::deleteReserves(program.strUniqueBroadcastId) != chinachu::api::REQUEST_FAILED) {
								XBMC->Log(ADDON::LOG_NOTICE, "Delete manual reserved program: %s", program.strUniqueBroadcastId.c_str());
								sleep(1);
								PVR->TriggerRecordingUpdate();
								PVR->TriggerTimerUpdate();
								return PVR_ERROR_NO_ERROR;
							} else {
								XBMC->Log(ADDON::LOG_ERROR, "Failed to delete reserved program: %s", program.strUniqueBroadcastId.c_str());
								XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Failed to delete reserved program: %s", program.strUniqueBroadcastId.c_str());
								return PVR_ERROR_SERVER_ERROR;
							}
						} else {
							if (chinachu::api::putReservesSkip(timer.strDirectory) != chinachu::api::REQUEST_FAILED) {
								XBMC->Log(ADDON::LOG_NOTICE, "Skip reserving: %s", timer.strDirectory);
								sleep(1);
								PVR->TriggerRecordingUpdate();
								PVR->TriggerTimerUpdate();
								return PVR_ERROR_NO_ERROR;
							} else {
								XBMC->Log(ADDON::LOG_ERROR, "Failed to skip reserved program: %s", program.strUniqueBroadcastId.c_str());
								XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Failed to skip reserved program: %s", program.strUniqueBroadcastId.c_str());
								return PVR_ERROR_SERVER_ERROR;
							}
						}
					}
				}
				break;
			}
		}

		XBMC->Log(ADDON::LOG_ERROR, "Failed to delete timer: nothing matched");
		XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Failed to delete timer: nothing matched");
		return PVR_ERROR_FAILED;
	} else {
		XBMC->Log(ADDON::LOG_ERROR, "Unknown timer type for deletion request: %d", timer.iTimerType);
		XBMC->QueueNotification(ADDON::QUEUE_ERROR, "Unknown timer type for deletion request: %d", timer.iTimerType);
		return PVR_ERROR_FAILED;
	}
}

PVR_ERROR GetTimerTypes(PVR_TIMER_TYPE types[], int *size) {
	int &count = *size;
	count = 0;

	memset(&types[count], 0, sizeof(types[count]));
	PVR_TIMER_TYPE &manualReserved = types[count];
	manualReserved.iId = TIMER_MANUAL_RESERVED;
	manualReserved.iAttributes = PVR_TIMER_TYPE_SUPPORTS_START_TIME | PVR_TIMER_TYPE_SUPPORTS_END_TIME |
		PVR_TIMER_TYPE_IS_MANUAL | PVR_TIMER_TYPE_FORBIDS_NEW_INSTANCES;
	strncpy(manualReserved.strDescription, XBMC->GetLocalizedString(MSG_TIMER_MANUAL_RESERVED), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);
	count++;

	memset(&types[count], 0, sizeof(types[count]));
	PVR_TIMER_TYPE &patternMatched = types[count];
	patternMatched.iId = TIMER_PATTERN_MATCHED;
	patternMatched.iAttributes = PVR_TIMER_TYPE_FORBIDS_NEW_INSTANCES |
		PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE | PVR_TIMER_TYPE_FORBIDS_NEW_INSTANCES;
	strncpy(patternMatched.strDescription, XBMC->GetLocalizedString(MSG_TIMER_PATTERN_MATCHED), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);
	count++;

	memset(&types[count], 0, sizeof(types[count]));
	PVR_TIMER_TYPE &manualReservedCreation = types[count];
	manualReservedCreation.iId = CREATE_TIMER_MANUAL_RESERVED;
	manualReservedCreation.iAttributes = PVR_TIMER_TYPE_IS_MANUAL | PVR_TIMER_TYPE_REQUIRES_EPG_TAG_ON_CREATE | PVR_TIMER_TYPE_IS_READONLY;
	strncpy(manualReservedCreation.strDescription, XBMC->GetLocalizedString(MSG_TIMER_MANUAL_RESERVED), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);
	count++;

	memset(&types[count], 0, sizeof(types[count]));
	PVR_TIMER_TYPE &instantReservedCreation = types[count];
	instantReservedCreation.iId = CREATE_TIMER_MANUAL_RESERVED;
	instantReservedCreation.iAttributes = PVR_TIMER_TYPE_REQUIRES_EPG_TAG_ON_CREATE;
	strncpy(instantReservedCreation.strDescription, XBMC->GetLocalizedString(MSG_TIMER_MANUAL_RESERVED), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);
	count++;

	memset(&types[count], 0, sizeof(types[count]));
	PVR_TIMER_TYPE &patternMatchedRuleCreation = types[count];
	patternMatchedRuleCreation.iId = CREATE_RULES_PATTERN_MATCHED;
	patternMatchedRuleCreation.iAttributes = PVR_TIMER_TYPE_SUPPORTS_CHANNELS |
		PVR_TIMER_TYPE_IS_REPEATING | PVR_TIMER_TYPE_SUPPORTS_TITLE_EPG_MATCH;
	strncpy(patternMatchedRuleCreation.strDescription, XBMC->GetLocalizedString(MSG_RULES_PATTERN_MATCHED_CREATION), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);
	count++;

	memset(&types[count], 0, sizeof(types[count]));
	PVR_TIMER_TYPE &patternMatchedRule = types[count];
	patternMatchedRule.iId = RULES_PATTERN_MATCHED;
	patternMatchedRule.iAttributes = PVR_TIMER_TYPE_SUPPORTS_CHANNELS | PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE |
		PVR_TIMER_TYPE_IS_READONLY | PVR_TIMER_TYPE_FORBIDS_NEW_INSTANCES |
		PVR_TIMER_TYPE_IS_REPEATING | PVR_TIMER_TYPE_SUPPORTS_TITLE_EPG_MATCH | PVR_TIMER_TYPE_SUPPORTS_FULLTEXT_EPG_MATCH;
	strncpy(patternMatchedRule.strDescription, XBMC->GetLocalizedString(MSG_RULES_PATTERN_MATCHED), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);
	count++;

	return PVR_ERROR_NO_ERROR;
}

/* not implemented */
bool IsTimeshifting() { return false; }

}
