#include <stdio.h>
#include "types.h"

#ifndef _ECLIPSE_CORE_H_
#define _ECLIPSE_CORE_H_
#if defined(__cplusplus)
extern "C" {
#endif

// Emulation
void core_init();
int core_start(const char *gamePath);
void core_stop();
void core_pause();
void core_resume();
void core_run_frame(bool processVideo);

// Controls
void core_activate_input(uint32_t input, double pos);
void core_deactivate_input(uint32_t input);
void core_reset_inputs();

// Saves
void core_write_save(const char *path);
void core_load_save(const char *path);

// Save States
void core_write_save_state(const char *path);
void core_load_save_state(const char *path);

// Cheats
bool core_add_cheat(const char *cheatCode, const char *type);
void core_reset_cheats();
void core_update_cheats();

// Handlers
typedef void (*core_buf_handler_t)(const unsigned char *buffer, int size);
typedef void (*core_fs_handler_t)(const char *path, int size);
void core_attach_video_handler(core_buf_handler_t videoHandler);
void core_attach_audio_handler(core_buf_handler_t audioHandler);
void core_attach_save_handler(core_fs_handler_t saveHandler);

// Misc.
double core_frame_duration();

#if defined(__cplusplus)
}
#endif
#endif