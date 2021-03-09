import CoreBridge from "./bridge.js";
class Core {
  /**
   * @param {any} game
   * @param {Uint8Array} rom
   * @param {HTMLCanvasElement} canvas
   */
  constructor(game, rom, canvas) {
    this.game = game;
    this.rom = rom;
    this.bridge = new this.constructor.bridge();
    this.state = 0;
  }

  init() {
    return this.bridge.init({
      saveHandler: (buffer) => console.log(buffer),
      audioHandler: (buffer, _) => console.log(buffer),
      videoHandler: (buffer, _) => console.log(buffer),
    });
  }

  start() {
    this.bridge.start(this.rom);
    this.state = 1;
    window.requestAnimationFrame(this.frame.bind(this));
  }

  pause() {
    this.state = 2;
  }

  stop() {
    this.state = 0;
    this.bridge.stop();
  }

  frame() {
    if (this.state == 1) this.bridge.frame();
    window.requestAnimationFrame(this.frame.bind(this));
  }

  /**
   * @param {{[k: string]: boolean}} inputs
   */
  setActiveInputs(inputs) {
    //@ts-ignore
    const inputMap = this.constructor.inputMap;
    for (const [input, active] of Object.entries(inputs)) {
      if (!inputMap.has(input)) continue;
      const value = inputMap.get(input);
      if (active) this.bridge.buttonDown(value);
      else this.bridge.buttonUp(value);
    }
  }

  /**
   * @param {string} code
   * @param {EclipseCore.CheatType} type
   */
  addCheat(code, type) {
    this.bridge.addCheat(code, type);
  }
}

Core.id = "dev.magnetar.NDSEclipseCore";
Core.fileTypes = ["nds"];
Core.cheatTypes = [];
Core.bridge = CoreBridge;

Core.system = {
  shortName: "NDS",
  longName: "Nintendo DS",
};

Core.inputMap = new Map([
  ["up", 1],
  ["down", 2],
  ["left", 4],
  ["right", 8],
  ["a", 16],
  ["b", 32],
  ["x", 64],
  ["y", 128],
  ["l_shoulder", 256],
  ["r_shoulder", 512],
  ["start", 1024],
  ["special", 2048],
  ["touch_screen_x", 4096],
  ["touch_screen_y", 8192],
]);

export default Core;
