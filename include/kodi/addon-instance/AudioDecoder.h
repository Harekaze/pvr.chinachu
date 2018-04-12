#pragma once
/*
 *      Copyright (C) 2005-2017 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "../AddonBase.h"
#ifdef BUILD_KODI_ADDON
#include "../AEChannelData.h"
#else
#include "cores/AudioEngine/Utils/AEChannelData.h"
#endif
#include <stdint.h>

namespace kodi { namespace addon { class CInstanceAudioDecoder; }}

extern "C"
{

  typedef struct AddonProps_AudioDecoder
  {
    int dummy;
  } AddonProps_AudioDecoder;

  typedef struct AddonToKodiFuncTable_AudioDecoder
  {
    void* kodiInstance;
  } AddonToKodiFuncTable_AudioDecoder;

  struct AddonInstance_AudioDecoder;
  typedef struct KodiToAddonFuncTable_AudioDecoder
  {
    kodi::addon::CInstanceAudioDecoder* addonInstance;
    bool (__cdecl* init) (const AddonInstance_AudioDecoder* instance,
                          const char* file, unsigned int filecache,
                          int* channels, int* samplerate,
                          int* bitspersample, int64_t* totaltime,
                          int* bitrate, AEDataFormat* format,
                          const AEChannel** info);
    int  (__cdecl* read_pcm) (const AddonInstance_AudioDecoder* instance, uint8_t* buffer, int size, int* actualsize);
    int64_t  (__cdecl* seek) (const AddonInstance_AudioDecoder* instance, int64_t time);
    bool (__cdecl* read_tag) (const AddonInstance_AudioDecoder* instance,
                              const char* file, char* title,
                              char* artist, int* length);
    int  (__cdecl* track_count) (const AddonInstance_AudioDecoder* instance, const char* file);
  } KodiToAddonFuncTable_AudioDecoder;

  typedef struct AddonInstance_AudioDecoder
  {
    AddonProps_AudioDecoder props;
    AddonToKodiFuncTable_AudioDecoder toKodi;
    KodiToAddonFuncTable_AudioDecoder toAddon;
  } AddonInstance_AudioDecoder;

} /* extern "C" */

namespace kodi
{
namespace addon
{

  class CInstanceAudioDecoder : public IAddonInstance
  {
  public:
    //==========================================================================
    /// @brief Class constructor
    ///
    /// @param[in] instance             The from Kodi given instance given be
    ///                                 add-on CreateInstance call with instance
    ///                                 id ADDON_INSTANCE_AUDIODECODER.
    explicit CInstanceAudioDecoder(KODI_HANDLE instance)
      : IAddonInstance(ADDON_INSTANCE_AUDIODECODER)
    {
      if (CAddonBase::m_interface->globalSingleInstance != nullptr)
        throw std::logic_error("kodi::addon::CInstanceAudioDecoder: Creation of multiple together with single instance way is not allowed!");

      SetAddonStruct(instance);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @ingroup cpp_kodi_addon_audiodecoder
    /// @brief Initialize a decoder
    ///
    /// @param[in] filename             The file to read
    /// @param[in] filecache            The file cache size
    /// @param[out] channels            Number of channels in output stream
    /// @param[out] samplerate          Samplerate of output stream
    /// @param[out] bitspersample       Bits per sample in output stream
    /// @param[out] totaltime           Total time for stream
    /// @param[out] bitrate             Average bitrate of input stream
    /// @param[out] format              Data format for output stream
    /// @param[out] channellist         Channel mapping for output stream
    /// @return                         true if successfully done, otherwise
    ///                                 false
    ///
    virtual bool Init(const std::string& filename, unsigned int filecache,
                      int& channels, int& samplerate,
                      int& bitspersample, int64_t& totaltime,
                      int& bitrate, AEDataFormat& format,
                      std::vector<AEChannel>& channellist) = 0;
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @ingroup cpp_kodi_addon_audiodecoder
    /// @brief Produce some noise
    ///
    /// @param[in] buffer               Output buffer
    /// @param[in] size                 Size of output buffer
    /// @param[out] actualsize          Actual number of bytes written to output buffer
    /// @return                         Return with following possible values:
    ///                                 | Value | Description                  |
    ///                                 |:-----:|:-----------------------------|
    ///                                 |   0   | on success
    ///                                 |  -1   | on end of stream
    ///                                 |   1   | on failure
    ///
    virtual int ReadPCM(uint8_t* buffer, int size, int& actualsize) = 0;
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @ingroup cpp_kodi_addon_audiodecoder
    /// @brief Seek in output stream
    ///
    /// @param[in] time                 Time position to seek to in milliseconds
    /// @return                         Time position seek ended up on
    ///
    virtual int64_t Seek(int64_t time) { return time; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @ingroup cpp_kodi_addon_audiodecoder
    /// @brief Read tag of a file
    ///
    /// @param[in] file                 File to read tag for
    /// @param[out] title               Title of file
    /// @param[out] artist              Artist of file
    /// @param[out] length              Length of file
    /// @return                         True on success, false on failure
    ///
    virtual bool ReadTag(const std::string& file, std::string& title, std::string& artist, int& length) { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @ingroup cpp_kodi_addon_audiodecoder
    /// @brief Get number of tracks in a file
    ///
    /// @param[in] file                 File to read tag for
    /// @return                         Number of tracks in file
    ///
    virtual int TrackCount(const std::string& file) { return 1; }
    //--------------------------------------------------------------------------

  private:
    void SetAddonStruct(KODI_HANDLE instance)
    {
      if (instance == nullptr)
        throw std::logic_error("kodi::addon::CInstanceAudioDecoder: Creation with empty addon structure not allowed, table must be given from Kodi!");

      m_instanceData = static_cast<AddonInstance_AudioDecoder*>(instance);

      m_instanceData->toAddon.addonInstance = this;
      m_instanceData->toAddon.init = ADDON_Init;
      m_instanceData->toAddon.read_pcm = ADDON_ReadPCM;
      m_instanceData->toAddon.seek = ADDON_Seek;
      m_instanceData->toAddon.read_tag = ADDON_ReadTag;
      m_instanceData->toAddon.track_count = ADDON_TrackCount;
    }

    inline static bool ADDON_Init(const AddonInstance_AudioDecoder* instance, const char* file, unsigned int filecache,
                                  int* channels, int* samplerate,
                                  int* bitspersample, int64_t* totaltime,
                                  int* bitrate, AEDataFormat* format,
                                  const AEChannel** info)
    {
      instance->toAddon.addonInstance->m_channelList.clear();
      bool ret = instance->toAddon.addonInstance->Init(file, filecache, *channels,
                                                           *samplerate, *bitspersample,
                                                           *totaltime, *bitrate, *format,
                                                           instance->toAddon.addonInstance->m_channelList);
      if (!instance->toAddon.addonInstance->m_channelList.empty())
      {
        if (instance->toAddon.addonInstance->m_channelList.back() != AE_CH_NULL)
          instance->toAddon.addonInstance->m_channelList.push_back(AE_CH_NULL);
        *info = instance->toAddon.addonInstance->m_channelList.data();
      }
      else
        *info = nullptr;
      return ret;
    }

    inline static int ADDON_ReadPCM(const AddonInstance_AudioDecoder* instance, uint8_t* buffer, int size, int* actualsize)
    {
      return instance->toAddon.addonInstance->ReadPCM(buffer, size, *actualsize);
    }

    inline static int64_t ADDON_Seek(const AddonInstance_AudioDecoder* instance, int64_t time)
    {
      return instance->toAddon.addonInstance->Seek(time);
    }

    inline static bool ADDON_ReadTag(const AddonInstance_AudioDecoder* instance, const char* file, char* title, char* artist, int* length)
    {
      std::string intTitle;
      std::string intArtist;
      bool ret = instance->toAddon.addonInstance->ReadTag(file, intTitle, intArtist, *length);
      if (ret)
      {
        strncpy(title, intTitle.c_str(), ADDON_STANDARD_STRING_LENGTH_SMALL-1);
        strncpy(artist, intArtist.c_str(), ADDON_STANDARD_STRING_LENGTH_SMALL-1);
      }
      return ret;
    }

    inline static int ADDON_TrackCount(const AddonInstance_AudioDecoder* instance, const char* file)
    {
      return instance->toAddon.addonInstance->TrackCount(file);
    }

    std::vector<AEChannel> m_channelList;
    AddonInstance_AudioDecoder* m_instanceData;
  };

} /* namespace addon */
} /* namespace kodi */
