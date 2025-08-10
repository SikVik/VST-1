
# RadioSauce Synth (JUCE VST3)

A modern, radio-ready polysynth for pop and hip‑hop production with a one‑click **New Sauce** generator to create unique, usable sounds fast.

## Features
- Polyphonic wavetable morph oscillator (Sine↔Saw↔Square) + Sub + Noise
- Unison (up to 7 voices), Detune, Stereo Spread
- Per-voice FM Amount, Waveshaper Drive
- ADSR Amp + ADSR Filter envelope
- Multimode Filter (LP/BP/HP) with Drive
- FX: Chorus • Delay • Reverb • Bitcrusher
- Master: Glue Comp • Soft Clip • Stereo Width • Limiter @ -1.0 dBTP
- Macro styles: **Pop Gloss**, **Trap 808**, **R&B Silk**
- **New Sauce** randomizer (musical ranges) for instant inspiration
- VST3; tested setup instructions below (AU/Standalone can be enabled via CMake toggles)

## Build (Windows / macOS)
1. Install a compiler and CMake
   - **Windows:** Visual Studio 2022 with Desktop C++
   - **macOS:** Xcode (Command Line Tools)
2. Install Git (for pulling JUCE)
3. From a terminal:
   ```bash
   git clone https://github.com/juce-framework/JUCE.git  # optional; FetchContent will do this automatically
   cd RadioSauceSynth
   cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release
   ```
4. The VST3 will be at:
   - Windows: `build/RadioSauceSynth_artefacts/Release/VST3/RadioSauceSynth.vst3`
   - macOS: `build/RadioSauceSynth_artefacts/Release/VST3/RadioSauceSynth.vst3`
5. Copy it to your VST3 directory:
   - **Windows:** `C:\Program Files\Common Files\VST3`
   - **macOS:** `~/Library/Audio/Plug-Ins/VST3`

> Open **FL Studio** and scan for new plugins. Look for **RadioSauceSynth**.

## Enable AU/Standalone (optional)
In `CMakeLists.txt`, set `PLUGIN_FORMATS` accordingly (e.g., `VST3 AU Standalone`).

## Licensing
This template uses JUCE via CMake FetchContent (GPL/Commercial). For closed‑source distribution, purchase a JUCE commercial license.

---

### Quick Use
- Turn the **Style** dropdown (Pop Gloss / Trap 808 / R&B Silk)
- Click **New Sauce** to generate a fresh, musical patch
- Fine‑tune with Macros: **Bite** (brightness), **Body** (low emphasis), **Air** (top-end), **Space** (reverb mix)
- Use **Limiter** ON to keep peaks clean for writing sessions

### Dev Notes
- The code aims to be clear and compact for extension.
- DSP lives in `SynthVoice.*` and `FXChain.*`. Parameters in `ParameterIDs.h`.
- GUI is basic JUCE; feel free to reskin with your brand later.
