# DS Eclipse Core

This is an _attempt_ at porting [DeSmuME](https://github.com/TASVideos/desmume.git) to WebAssembly for [Eclipse](https://eclipseemu.me). Does it work? No. It gets about 3 frames in before the thread (wether it is a Worker or the main thread) it runs in gets overwhelmed. I'm just making this public for other people to play with and maybe try to figure out how to get working properly (~~and also to show that it's not really possible atm so people stop asking lmao~~).

The C++ wrapper code found here is based around Riley Testut's [DSDetlaCore](https://github.com/rileytestut/DSDeltaCore), so big props to him. Only used that code to get this process going; I intend to replace it with my own code as things progress.

Feel free to contribute and all that, would love to see this one day work. Chromium-based browsers and Firefox are currently the only things that can attempt to run this, as it requires `SharedArrayBuffer` and `Atomics`, **neither of which Safari implement** (or, in the case of `SharedArrayBuffer`, enable). **So even if this worked, it wouldn't be suitable for Eclipse until that happens.** Until Safari adds those, this is pretty much abandonware.

## Project Structure

Pretty straight forward, we use Docker to compile the C++, so it's consistent across enviornments. Once this works, it should be a package you can install via your package manager of choice, hence both `dist/` and `docs/`

**Folders**

- `desmume/` → submodule containing TASVideos/desmume.git
- `dist/` → Where the built files are sent (and eventually the EclipseCore wrapper)
- `docs/` → Where GitHub Pages points to, basic way to test out the core.
- `lib/` → The C++ wrapper and any associated files

## Some General Things To-Do

- [ ] Move the main logic to a Worker
- [ ] Check for memory leaks and thread-blocking loops
- [ ] See if we can build without needing pthreads (for Safari support)
- [ ] Find ways to optimize in general
- [ ] Render frame buffer
- [ ] Handle & render audio buffer
- [ ] Handle saves
- [ ] Add support for cheats

## How do I run this?

Well, first you need to build the Wasm file. They're too big for GitHub apparently, so you'll need to compile them yourself. This is as easy as having docker and node installed on your computer, just run `npm run build`. Give it a few minutes, it is a pretty big project, then you should get the files in both `dist/` and `docs/emu/`. You can then serve the docs folder with `npm run serve`.
