CXX = em++
CXXFLAGS = -O0
OUT = ./dist/emu
INCLUDE := -Idesmume/desmume/src -Idesmume/desmume/src/libretro-common/include -Idesmume/desmume/src/libretro-common/include/math -Idesmume/desmume/src/libretro-common/include/file -Idesmume/desmume/src/metaspu -Idesmume/desmume/src/metaspu/SoundTouch -Idesmume/desmume/src/metaspu/win32 -Ilib -Ilib/desmume_overrides
SOURCES_EXCLUDE = 
# desmume/desmume/src/libretro-common/file/retro_stat.c
SOURCES = \
	lib/main.cpp \
	lib/rthreads/rthreads.c \
	desmume/desmume/src/libretro-common/compat/compat_getopt.c \
	desmume/desmume/src/libretro-common/file/file_path.c \
	desmume/desmume/src/libretro-common/compat/compat_strl.c \
	desmume/desmume/src/libretro-common/features/features_cpu.c \
	desmume/desmume/src/libretro-common/file/retro_dirent.c \
	desmume/desmume/src/libretro-common/rthreads/async_job.c \
	desmume/desmume/src/libretro-common/rthreads/rsemaphore.c \
	desmume/desmume/src/libretro-common/encodings/encoding_utf.c \
	lib/desmume_overrides/path.cpp \
	desmume/desmume/src/armcpu.cpp \
	desmume/desmume/src/arm_instructions.cpp \
	desmume/desmume/src/bios.cpp \
	desmume/desmume/src/cp15.cpp \
	desmume/desmume/src/commandline.cpp \
	desmume/desmume/src/common.cpp \
	desmume/desmume/src/debug.cpp \
	desmume/desmume/src/driver.cpp \
	desmume/desmume/src/Database.cpp \
	desmume/desmume/src/emufile.cpp \
	desmume/desmume/src/encrypt.cpp \
	desmume/desmume/src/FIFO.cpp \
	desmume/desmume/src/firmware.cpp \
	desmume/desmume/src/GPU.cpp \
	desmume/desmume/src/mc.cpp \
	desmume/desmume/src/readwrite.cpp \
	desmume/desmume/src/wifi.cpp \
	desmume/desmume/src/MMU.cpp \
	desmume/desmume/src/NDSSystem.cpp \
	desmume/desmume/src/ROMReader.cpp \
	desmume/desmume/src/render3D.cpp \
	desmume/desmume/src/rtc.cpp \
	desmume/desmume/src/saves.cpp \
	desmume/desmume/src/slot1.cpp \
	desmume/desmume/src/slot2.cpp \
	desmume/desmume/src/mic.cpp \
	desmume/desmume/src/SPU.cpp \
	desmume/desmume/src/matrix.cpp \
	desmume/desmume/src/gfx3d.cpp \
	desmume/desmume/src/thumb_instructions.cpp \
	desmume/desmume/src/movie.cpp \
	desmume/desmume/src/frontend/modules/Disassembler.cpp \
	desmume/desmume/src/utils/advanscene.cpp \
	desmume/desmume/src/utils/datetime.cpp \
	desmume/desmume/src/utils/guid.cpp \
	desmume/desmume/src/utils/emufat.cpp \
	desmume/desmume/src/utils/fsnitro.cpp \
	desmume/desmume/src/utils/decrypt/crc.cpp \
	desmume/desmume/src/utils/decrypt/decrypt.cpp \
	desmume/desmume/src/utils/decrypt/header.cpp \
	desmume/desmume/src/utils/task.cpp \
	desmume/desmume/src/utils/vfat.cpp \
	desmume/desmume/src/utils/dlditool.cpp \
	desmume/desmume/src/utils/libfat/cache.cpp \
	desmume/desmume/src/utils/libfat/directory.cpp \
	desmume/desmume/src/utils/libfat/disc.cpp \
	desmume/desmume/src/utils/libfat/fatdir.cpp \
	desmume/desmume/src/utils/libfat/fatfile.cpp \
	desmume/desmume/src/utils/libfat/filetime.cpp \
	desmume/desmume/src/utils/libfat/file_allocation_table.cpp \
	desmume/desmume/src/utils/libfat/libfat.cpp \
	desmume/desmume/src/utils/libfat/libfat_public_api.cpp \
	desmume/desmume/src/utils/libfat/lock.cpp \
	desmume/desmume/src/utils/libfat/partition.cpp \
	desmume/desmume/src/utils/tinyxml/tinystr.cpp \
	desmume/desmume/src/utils/tinyxml/tinyxml.cpp \
	desmume/desmume/src/utils/tinyxml/tinyxmlerror.cpp \
	desmume/desmume/src/utils/tinyxml/tinyxmlparser.cpp \
	desmume/desmume/src/utils/xstring.cpp \
	desmume/desmume/src/utils/colorspacehandler/colorspacehandler.cpp \
	desmume/desmume/src/addons/slot2_auto.cpp \
	desmume/desmume/src/addons/slot2_mpcf.cpp \
	desmume/desmume/src/addons/slot2_paddle.cpp \
	desmume/desmume/src/addons/slot2_gbagame.cpp \
	desmume/desmume/src/addons/slot2_none.cpp \
	desmume/desmume/src/addons/slot2_rumblepak.cpp \
	desmume/desmume/src/addons/slot2_guitarGrip.cpp \
	desmume/desmume/src/addons/slot2_expMemory.cpp \
	desmume/desmume/src/addons/slot2_piano.cpp \
	desmume/desmume/src/addons/slot2_passme.cpp \
	desmume/desmume/src/addons/slot1_none.cpp \
	desmume/desmume/src/addons/slot1_r4.cpp \
	desmume/desmume/src/addons/slot1_retail_nand.cpp \
	desmume/desmume/src/addons/slot1_retail_auto.cpp \
	desmume/desmume/src/addons/slot1_retail_mcrom.cpp \
	desmume/desmume/src/addons/slot1_retail_mcrom_debug.cpp \
	desmume/desmume/src/addons/slot1comp_mc.cpp \
	desmume/desmume/src/addons/slot1comp_rom.cpp \
	desmume/desmume/src/addons/slot1comp_protocol.cpp \
	desmume/desmume/src/cheatSystem.cpp \
	desmume/desmume/src/texcache.cpp \
	desmume/desmume/src/rasterize.cpp \
	desmume/desmume/src/metaspu/metaspu.cpp \
	desmume/desmume/src/filter/2xsai.cpp \
	desmume/desmume/src/filter/bilinear.cpp \
	desmume/desmume/src/filter/deposterize.cpp \
	desmume/desmume/src/filter/epx.cpp \
	desmume/desmume/src/filter/hq2x.cpp \
	desmume/desmume/src/filter/hq3x.cpp \
	desmume/desmume/src/filter/hq4x.cpp \
	desmume/desmume/src/filter/lq2x.cpp \
	desmume/desmume/src/filter/scanline.cpp \
	desmume/desmume/src/filter/videofilter.cpp \
	desmume/desmume/src/filter/xbrz.cpp \
	desmume/desmume/src/version.cpp
SOURCES := $(filter-out $(SOURCES_EXCLUDE), $(SOURCES))

EXPORTED_FUNCS = \
	"_core_init", \
	"_core_start", \
	"_core_pause", \
	"_core_stop", \
	"_core_run_frame", \
	"_core_activate_input", \
	"_core_deactivate_input", \
	"_core_reset_inputs", \
	"_core_add_cheat", \
	"_core_reset_cheats", \
	"_core_update_cheats", \
	"_core_write_save", \
	"_core_load_save", \
	"_core_write_save_state", \
	"_core_load_save_state", \
	"_core_attach_video_handler", \
	"_core_attach_audio_handler", \
	"_core_attach_save_handler", \
	"_core_frame_duration"

EXPORTED_RUNTIME_FUNCS = \
	"ccall", \
	"cwrap", \
	"allocate", \
	"intArrayFromString", \
	"FS", \
	"addFunction", \
	"ALLOC_NORMAL", \
	"getValue"
#65536
EMFLAGS = -s MODULARIZE=1 -s EXPORT_ES6=1 -s EXPORT_NAME=ds_eclipse_core -s INITIAL_MEMORY=107479040 -s ALLOW_MEMORY_GROWTH=1 -s USE_ZLIB=1 -pthread -s PROXY_TO_PTHREAD=1 -s WASM=1 -s EXPORTED_FUNCTIONS='[$(EXPORTED_FUNCS)]' -s EXTRA_EXPORTED_RUNTIME_METHODS='[$(EXPORTED_RUNTIME_FUNCS)]' -s RESERVED_FUNCTION_POINTERS=5

build: $(SOURCES)
	# path.h uses glibc, which Emscripten doesn't support, 
	# and xstring.cpp doesn't find symbols for some reason
	# to save us from a fork, we just swap it out. Hacky, but works
	-@mkdir tmp
	-@mv desmume/desmume/src/path.h desmume/desmume/src/utils/xstring.cpp tmp/
	-@cp lib/desmume_overrides/path.h desmume/desmume/src/path.h
	-@cp lib/desmume_overrides/xstring.cpp desmume/desmume/src/utils/xstring.cpp
	-@mkdir dist/
	$(CXX) $(CXXFLAGS) $(EMFLAGS) $(INCLUDE) $(SOURCES) -o $(OUT).js
	-@rm -rvf desmume/desmume/src/path.h desmume/desmume/src/utils/xstring.cpp
	-@mv tmp/path.h desmume/desmume/src/
	-@mv tmp/xstring.cpp desmume/desmume/src/utils/
	-@rm -rvf tmp/ docs/emu/
	-@mkdir docs/emu/
	-@cp $(OUT).js $(OUT).wasm $(OUT).worker.js docs/emu/

clean:
	-@rm -rvf dist/
	-@rm -rvf docs/emu/