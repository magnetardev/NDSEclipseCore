export default class CoreBridge {
  constructor() {
    this.frameDuration = 0;
  }

  /**
   * Initialize the core
   * @param {object} options An object with all the handlers
   * @param {EclipseCore.BufferHandler|undefined} options.videoHandler A function that handles when a video buffer is created
   * @param {EclipseCore.BufferHandler|undefined} options.audioHandler A function that handles when an audio buffer is created
   * @param {EclipseCore.FileCallback|undefined} options.saveHandler A function that handles when a save is created
   * @param {((module: any) => Promise<void>)|undefined} options.preCall Handle stuff before calling init
   */
  async init(
    { saveHandler, videoHandler, audioHandler, preCall } = {
      saveHandler: undefined,
      videoHandler: undefined,
      audioHandler: undefined,
      preCall: undefined,
    }
  ) {
    // Get module, if it doesn't already exist
    if (!this.module) {
      await new Promise(async (resolve) => {
        const { default: Module } = await import("./emu/emu.js");
        this.module = await Module({
          onRuntimeInitialized() {
            resolve();
          },
          locateFile: (path, prefix) => prefix + path,
        });
      });
    }

    // Set handlers
    if (saveHandler && !this.saveHandlerInitialized)
      this.attachSaveHandler(saveHandler);
    if (videoHandler && !this.videoHandlerInitialized)
      this.attachVideoHandler(videoHandler);
    if (audioHandler && !this.audioHandlerInitialized)
      this.attachAudioHandler(audioHandler);
    if (preCall) await preCall(this.module);

    // Initialize
    this.module.ccall("core_init", null, [], []);
    this.frameDuration = this.module.ccall("core_frame_duration", null, [], []);

    // Frequently used C calls should be wrapped instead
    this.frame = this.module.cwrap("core_run_frame", null, ["boolean"]);
    this.buttonUp = this.module.cwrap("core_deactivate_input", null, [
      "number",
    ]);
    this.buttonDown = this.module.cwrap("core_activate_input", null, [
      "number",
    ]);
  }

  /**
   * Load a game's ROM
   * @param {Uint8Array} rom The game's ROM file
   */
  start(rom) {
    const fileName = `game_file`;
    this.module.FS.writeFile(`./${fileName}`, rom);
    return this.module.ccall("core_start", null, ["string"], [fileName]);
  }

  /**
   * Stops emulation
   */
  stop() {
    this.module.ccall("core_stop", null, [], []);
  }

  /**
   * Sets all inputs as inactive
   */
  buttonsUp() {
    this.module.ccall("core_reset_inputs", null, [], []);
  }

  /**
   * Adds a cheat to the currently active game
   * @returns {Promise<boolean>} Wether or not the cheat was loaded
   * @param {string} code A cheat code
   * @param {EclipseCore.CheatType} type A cheat code type (e.g. GameGenie, ActionReplay, etc.)
   */
  addCheat(code, type) {
    return this.module.ccall(
      "core_add_cheat",
      "boolean",
      ["string", "string"],
      [code, type]
    );
  }

  /**
   * Removes all active cheats
   */
  resetCheats() {
    this.module.ccall("core_reset_cheats", null, [], []);
  }

  /**
   * Save a game's save state
   * @returns {Uint8Array} The save state
   */
  save() {
    const fileName = "./game_save.sav";
    this.module.ccall("core_write_save", null, ["string"], [fileName]);
    return this.module.FS.readFile(fileName);
  }

  /**
   * @param {Uint8Array} buffer A save state's file buffer
   */
  load(buffer) {
    const fileName = "./game_save.sav";
    this.module.FS.writeFile(`${fileName}`, buffer);
    this.module.ccall("core_load_save", null, ["string"], [buffer]);
    this.module.FS.unlink(`/${fileName}`);
  }

  /**
   * Save a game's save state
   * @returns {Uint8Array} The save state
   */
  saveState() {
    const filePath = `/save_state_${("" + Math.random()).substring(2)}`;
    this.module.ccall("core_write_save_state", null, ["string"], [filePath]);
    const res = this.module.FS.readFile(filePath);
    this.module.FS.unlink(filePath);
    return res;
  }

  /**
   * @param {Uint8Array} buffer A save state's file buffer
   */
  loadState(buffer) {
    const fileName = "game_save_state";
    this.module.FS.writeFile(`${fileName}`, buffer);
    this.module.ccall(
      "core_load_save_state",
      null,
      ["string"],
      [`/${fileName}`]
    );
    this.module.FS.unlink(`/${fileName}`);
  }

  /**
   * Attach a save handler to the core
   * @param {EclipseCore.FileCallback} handler Called when the game is saved
   */
  attachSaveHandler(handler) {
    const handlerFnPointer = this.module.addFunction((buffer, size) => {
      const typedArray = this.module.HEAPU8.subarray(buffer, buffer + size);
      const path = new TextDecoder("utf-8").decode(typedArray);
      console.log(this.module.FS.readdir("./"));
      console.log(path);
      try {
        const buff = this.module.FS.readFile(path);
        handler(buff);
      } catch (err) {}
    }, "vii");
    this.module.ccall(
      "core_attach_save_handler",
      null,
      ["number"],
      [handlerFnPointer]
    );
    this.saveHandlerInitialized = true;
  }

  /**
   * Attach a video handler to the core
   * @param {EclipseCore.BufferHandler} handler Called when a video buffer is available
   */
  attachVideoHandler(handler) {
    const handlerFnPointer = this.module.addFunction(
      function (buffer, size) {
        let typedArray = this.module.HEAPU8.subarray(buffer, buffer + size);
        let array = Array.from(typedArray);
        handler(array, size);
      }.bind(this),
      "vii"
    );
    this.module.ccall(
      "core_attach_video_handler",
      null,
      ["number"],
      [handlerFnPointer]
    );
    this.videoHandlerInitialized = true;
  }

  /**
   * Attach a audio handler to the core
   * @param {EclipseCore.BufferHandler} handler Called when an audio buffer is available
   */
  attachAudioHandler(handler) {
    // @ts-ignore
    const handlerFnPointer = this.module.addFunction(
      function (pointer, size) {
        // @ts-ignore
        let typedArray = this.module.HEAPU8.subarray(pointer, pointer + size);
        let array = Array.from(typedArray);
        handler(array, size);
      }.bind(this),
      "vii"
    );
    this.module.ccall(
      "core_attach_audio_handler",
      null,
      ["number"],
      [handlerFnPointer]
    );
    this.audioHandlerInitialized = true;
  }
}
