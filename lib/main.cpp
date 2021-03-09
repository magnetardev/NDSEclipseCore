#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <retro_miscellaneous.h>

#include "eclipse_core.h"

#include "types.h"
#include "render3D.h"
#include "rasterize.h"
#include "SPU.h"
#include "debug.h"
#include "NDSSystem.h"
#include "path.h"
#include "slot1.h"
#include "saves.h"
#include "cheatSystem.h"
#include "slot1.h"
#include "version.h"
#include "metaspu.h"
#include "GPU.h"

#define SNDCORE_ECLIPSE 1
void eclipse_update_audio(s16 *buffer, u32 num_samples);
u32 eclipse_get_audio_space();

bool _core_is_ready = false;
volatile bool execute = true;

SoundInterface_struct EclipseAudio = {
    SNDCORE_ECLIPSE,
    "CoreAudio Sound Interface",
    SNDDummy.Init,
    SNDDummy.DeInit,
    eclipse_update_audio,
    eclipse_get_audio_space,
    SNDDummy.MuteAudio,
    SNDDummy.UnMuteAudio,
    SNDDummy.SetVolume,
};

GPU3DInterface *core3DList[] = {
    &gpu3DNull,
    &gpu3DRasterize,
    NULL
};

SoundInterface_struct *SNDCoreList[] = {
    &SNDDummy,
    &EclipseAudio,
    NULL
};

core_buf_handler_t _core_audio_handler;
core_buf_handler_t _core_video_handler;
core_fs_handler_t _core_save_handler;

uint32_t _core_activated_inputs = 0;
uint32_t core_touch_screen_pos_x = 0;
uint32_t core_touch_screen_pos_y = 0;

enum DSInputMapping {
    ButtonUp = 1,
    ButtonDown = 2,
    ButtonLeft = 4,
    ButtonRight = 8,
    ButtonA = 16,
    ButtonB = 32,
    ButtonX = 64,
    ButtonY = 128,
    ButtonL = 256,
    ButtonR = 512,
    ButtonStart = 1024,
    ButtonSelect = 2048,
    TouchScreenX = 4096,
    TouchScreenY = 8192
};

void core_init() {}

int core_start(const char *gamePath) {
  path.ReadPathSettings();
  // General
  CommonSettings.num_cores = (int)sysconf( _SC_NPROCESSORS_ONLN );
  CommonSettings.advanced_timing = false;
  CommonSettings.cheatsDisable = true;
  CommonSettings.autodetectBackupMethod = 1;
  CommonSettings.use_jit = false;
  CommonSettings.micMode = TCommonSettings::Physical;
  CommonSettings.showGpu.main = 1;
  CommonSettings.showGpu.sub = 1;

  // HUD
  CommonSettings.hud.FpsDisplay = false;
  CommonSettings.hud.FrameCounterDisplay = false;
  CommonSettings.hud.ShowInputDisplay = false;
  CommonSettings.hud.ShowGraphicalInputDisplay = false;
  CommonSettings.hud.ShowLagFrameCounter = false;
  CommonSettings.hud.ShowMicrophone = false;
  CommonSettings.hud.ShowRTC = false;

  // Graphics
  CommonSettings.GFX3D_HighResolutionInterpolateColor = 0;
  CommonSettings.GFX3D_EdgeMark = 0;
  CommonSettings.GFX3D_Fog = 1;
  CommonSettings.GFX3D_Texture = 1;
  CommonSettings.GFX3D_LineHack = 0;

  // Sound
  CommonSettings.spuInterpolationMode = SPUInterpolation_Cosine;
  CommonSettings.spu_advanced = false;

  // Firmware
  CommonSettings.fwConfig.language = NDS_FW_LANG_ENG;
  CommonSettings.fwConfig.favoriteColor = 15;
  CommonSettings.fwConfig.birthdayMonth = 10;
  CommonSettings.fwConfig.birthdayDay = 7;
  CommonSettings.fwConfig.consoleType = NDS_CONSOLE_TYPE_LITE;

  // Set Const
  static const char *nickname = "Eclipse";
  CommonSettings.fwConfig.nicknameLength = strlen(nickname);
  for (int i = 0; i < CommonSettings.fwConfig.nicknameLength; ++i)
    CommonSettings.fwConfig.nickname[i] = nickname[i];

  static const char *message = "Eclipse can play DS now!";
  CommonSettings.fwConfig.messageLength = strlen(message);
  for (int i = 0; i < CommonSettings.fwConfig.messageLength; ++i)
    CommonSettings.fwConfig.message[i] = message[i];

  if (!_core_is_ready) {
    Desmume_InitOnce();

    NDS_Init();
    cur3DCore = 0;

    GPU->Change3DRendererByID(0);
    GPU->SetColorFormat(NDSColorFormat_BGR888_Rev);

    // SPU_ChangeSoundCore(SNDCORE_DELTA, DESMUME_SAMPLE_RATE * 8/60);

    _core_is_ready = true;
  }
  const char *gameDirectory = "/dev/null";
  path.setpath(PathInfo::BATTERY, gameDirectory);

  if (!NDS_LoadROM(gamePath)) printf("Error loading ROM: %s", gameDirectory);
  return 1;
}

void core_stop() {
  NDS_FreeROM();
}

void core_pause() {}
void core_resume() {}

void core_run_frame(bool processVideo) {
  // Inputs
  NDS_setPad(
    _core_activated_inputs & DSInputMapping::ButtonRight,
    _core_activated_inputs & DSInputMapping::ButtonLeft,
    _core_activated_inputs & DSInputMapping::ButtonDown,
    _core_activated_inputs & DSInputMapping::ButtonUp,
    _core_activated_inputs & DSInputMapping::ButtonSelect,
    _core_activated_inputs & DSInputMapping::ButtonStart,
    _core_activated_inputs & DSInputMapping::ButtonB,
    _core_activated_inputs & DSInputMapping::ButtonA,
    _core_activated_inputs & DSInputMapping::ButtonY,
    _core_activated_inputs & DSInputMapping::ButtonX,
    _core_activated_inputs & DSInputMapping::ButtonL,
    _core_activated_inputs & DSInputMapping::ButtonR,
    false,
    false
  );
  
  if (_core_activated_inputs & DSInputMapping::TouchScreenX || _core_activated_inputs & DSInputMapping::TouchScreenY)
      NDS_setTouchPos(core_touch_screen_pos_x, core_touch_screen_pos_y);
  else NDS_releaseTouch();
  
  NDS_beginProcessingInput();
  NDS_endProcessingInput();
  
  if (!processVideo) NDS_SkipNextFrame();
  
  NDS_exec<false>();
  
  if (processVideo) {
      (*_core_video_handler)((const unsigned char *)GPU->GetDisplayInfo().masterNativeBuffer, 256 * 384 * 4);
  }
  
  SPU_Emulate_user();
}

void core_activate_input(uint32_t input, double pos) {
  _core_activated_inputs |= input;
  switch ((DSInputMapping)input) {
  case DSInputMapping::TouchScreenX:
    core_touch_screen_pos_x = pos * 256;
    break;
  case DSInputMapping::TouchScreenY:
    core_touch_screen_pos_y = pos * 192;
    break;
  default: 
    break;
  }
}

void core_deactivate_input(uint32_t input) {    
  _core_activated_inputs &= ~input;
  switch ((DSInputMapping)input) {
  case DSInputMapping::TouchScreenX:
    core_touch_screen_pos_x = 0;
    break;
  case DSInputMapping::TouchScreenY:
    core_touch_screen_pos_y = 0;
    break;
  default: 
    break;
  }
}

void core_reset_inputs() {
  _core_activated_inputs = 0;
  core_touch_screen_pos_x = 0;
  core_touch_screen_pos_y = 0;
}

void core_write_save(const char *path) {
    MMU_new.backupDevice.export_raw(path);
}

void core_load_save(const char *path) {
    MMU_new.backupDevice.export_raw(path);
}

void core_write_save_state(const char *path) {
    MMU_new.backupDevice.export_raw(path);
}

void core_load_save_state(const char *path) {
    MMU_new.backupDevice.export_raw(path);
}

bool core_add_cheat(const char *cheatCode, const char *type) {
    return false;
}

void core_reset_cheats() {}
void core_update_cheats() {}

// MARK: Handlers
void core_attach_video_handler(core_buf_handler_t videoHandler) {
  puts("[DS] Core Attach Video Handler");
  _core_video_handler = videoHandler;
}

void core_attach_audio_handler(core_buf_handler_t audioHandler) {
  puts("[DS] Core Attach Audio Handler");
  _core_audio_handler = audioHandler;
}

void core_attach_save_handler(core_fs_handler_t saveHandler) {
  puts("[DS] Core Attach Save Handler");
  _core_save_handler = saveHandler;
}

void eclipse_update_audio(s16 *buffer, u32 num_samples) {
    // [DSEmulatorBridge.sharedBridge.audioRenderer.audioBuffer writeBuffer:(uint8_t *)buffer size:num_samples * 4];
}

u32 eclipse_get_audio_space() {
    // TODO: Actually do something here
    return 0;
}

double core_frame_duration() {
    return (1.0 / 60.0);
}

// retro_stat.c's symbols were not being supported properly, 
// so let's just put it here for now lmao

extern "C" {


/* Copyright  (C) 2010-2016 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this file (retro_stat.c).
 * ---------------------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */



enum stat_mode
{
   IS_DIRECTORY = 0,
   IS_CHARACTER_SPECIAL,
   IS_VALID
};

static bool path_stat(const char *path, enum stat_mode mode, int32_t *size)
{
  struct stat buf;
   if (stat(path, &buf) < 0)
      return false;

   if (size)
      *size = buf.st_size;

   switch (mode) {
      case IS_DIRECTORY:
         return S_ISDIR(buf.st_mode);
      case IS_CHARACTER_SPECIAL:
         return false;
         return S_ISCHR(buf.st_mode);
      case IS_VALID:
         return true;
   }

   return false;
}

/**
 * path_is_directory:
 * @path               : path
 *
 * Checks if path is a directory.
 *
 * Returns: true (1) if path is a directory, otherwise false (0).
 */
bool path_is_directory(const char *path)
{
   return path_stat(path, IS_DIRECTORY, NULL);
}

bool path_is_character_special(const char *path)
{
   return path_stat(path, IS_CHARACTER_SPECIAL, NULL);
}

bool path_is_valid(const char *path)
{
   return path_stat(path, IS_VALID, NULL);
}

int32_t path_get_size(const char *path)
{
   int32_t filesize = 0;
   if (path_stat(path, IS_VALID, &filesize))
      return filesize;

   return -1;
}

/**
 * path_mkdir_norecurse:
 * @dir                : directory
 *
 * Create directory on filesystem.
 *
 * Returns: true (1) if directory could be created, otherwise false (0).
 **/
bool mkdir_norecurse(const char *dir)
{
   int ret;
   ret = mkdir(dir, 0750);
   if (ret < 0 && errno == EEXIST && path_is_directory(dir))
      ret = 0;
   if (ret < 0)
      printf("mkdir(%s) error: %s.\n", dir, strerror(errno));
   return ret == 0;
}
}