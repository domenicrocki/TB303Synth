# TB-303 Synthesizer Plugin

A TB-303 Bass Line synthesizer clone by **Rocki**, built as a **VST3 plugin** and **Standalone application** using the [JUCE](https://juce.com/) framework (C++17).

## Features

### Sound Engine
- **Polyblep oscillator** — Sawtooth and Square waveforms (band-limited, alias-free)
- **Diode ladder filter** — 4-pole 24dB/oct resonant lowpass with self-oscillation
- **Decay-only filter envelope** — near-instant attack (~3ms), variable decay (30ms–3s)
- **ENV MOD** — controls envelope depth to filter cutoff
- **Accent** — boosts filter envelope depth and output volume per step
- **Slide** — 60ms portamento glide between consecutive notes
- **Tuning** — oscillator detune (-12 to +12 semitones)
- **Master Tune** — fine tuning offset
- **VCF Trim / Condition** — additional filter character controls

### Effects
- **Drive** — 3 types: Soft clip, Hard clip, Tube overdrive
  - Tone control (post-drive EQ)
  - Depth control (drive amount)
- **Delay** — 3 types: Digital (clean), Tape (filtered feedback), Ping Pong (stereo)
  - Time, Level (wet mix), Feedback controls
  - **Tempo Sync** — snaps delay time to musical note divisions

### Step Sequencer
- 16-step pattern sequencer
- Per-step: Note, Octave, Accent, Slide, Active toggle
- **64 pattern slots** (8 banks x 8 patterns)
- **5 play modes**: Forward, Reverse, FWD&REV, Invert, Random
- **Scale**: 1/16, 1/16 Triplet, 1/32 note divisions
- **Shuffle/Swing** control
- **Randomize** with undo
- **Host tempo sync** when used as VST3 in a DAW

### Presets
- 10 factory patches: Acid Saw, Acid WideOD Saw, Squelch Square, Rubber Bass, Hard Acid, Mellow Square, Screaming Lead, Deep Sub, Delay Acid, Tube Distortion
- Save/load patches and patterns (persisted in plugin state)
- MIDI input for live keyboard play

## Quick Start

### macOS

```bash
# 1. Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 2. Install build tools
brew install cmake ninja

# 3. Clone and build
git clone https://github.com/domenicrocki/TB303Synth.git
cd TB303Synth
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# 4. Run standalone
./build/TB303Synth_artefacts/Release/Standalone/TB-303.app/Contents/MacOS/TB-303

# 5. Install VST3 plugin (optional)
cp -r build/TB303Synth_artefacts/Release/VST3/TB-303.vst3 ~/Library/Audio/Plug-Ins/VST3/
```

### Linux

```bash
# 1. Install build tools and JUCE dependencies
sudo apt-get install -y build-essential cmake ninja-build \
  libasound2-dev libfreetype-dev libx11-dev \
  libxrandr-dev libxinerama-dev libxcursor-dev libgl-dev

# 2. Clone and build
git clone https://github.com/domenicrocki/TB303Synth.git
cd TB303Synth
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# 3. Run standalone
./build/TB303Synth_artefacts/Release/Standalone/TB-303

# 4. Install VST3 plugin (optional)
cp -r build/TB303Synth_artefacts/Release/VST3/TB-303.vst3 ~/.vst3/
```

### Windows

1. Install [Visual Studio 2019+](https://visualstudio.microsoft.com/) with **C++ desktop development** workload
2. Install [CMake 3.22+](https://cmake.org/download/) (or use the one included with Visual Studio)

```powershell
git clone https://github.com/domenicrocki/TB303Synth.git
cd TB303Synth
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

VST3 output: `build\TB303Synth_artefacts\Release\VST3\TB-303.vst3`
Copy to: `C:\Program Files\Common Files\VST3\`

## VST3 Installation

After building, copy the VST3 bundle to your system's plugin folder:

| OS      | VST3 Path                                |
|---------|------------------------------------------|
| macOS   | `~/Library/Audio/Plug-Ins/VST3/`         |
| Linux   | `~/.vst3/`                               |
| Windows | `C:\Program Files\Common Files\VST3\`    |

Then rescan plugins in your DAW (Ableton Live, Logic Pro, Bitwig, Reaper, FL Studio, etc.).

## Build Notes

- **JUCE 7.0.9** is downloaded automatically via CMake FetchContent — no manual installation needed
- First build takes a few minutes (downloads and compiles JUCE)
- Subsequent builds are fast (only recompiles changed files)
- Build artifacts are located at:

```
build/TB303Synth_artefacts/Release/
├── Standalone/
│   └── TB-303              # Standalone application (.app on macOS)
└── VST3/
    └── TB-303.vst3/        # VST3 plugin bundle
```

## Project Structure

```
TB303Synth/
├── CMakeLists.txt                  # Build configuration (JUCE via FetchContent)
├── Source/
│   ├── PluginProcessor.h/cpp      # Audio processor, APVTS parameters, state
│   ├── PluginEditor.h/cpp         # Main GUI window (1500x680)
│   ├── DSP/
│   │   ├── TB303Oscillator.h/cpp  # Polyblep saw/square oscillator
│   │   ├── TB303Filter.h/cpp      # Diode ladder lowpass filter
│   │   ├── TB303Envelope.h/cpp    # Decay envelope with sustain
│   │   ├── TB303Voice.h/cpp       # Voice: osc + filter + env + accent + slide
│   │   ├── DriveEffect.h/cpp      # Overdrive (soft/hard/tube)
│   │   └── DelayEffect.h/cpp      # Delay (digital/tape/ping-pong)
│   ├── Sequencer/
│   │   ├── StepSequencer.h/cpp    # 16-step sequencer, 5 play modes
│   │   └── PatternData.h/cpp      # 8x8 pattern bank storage
│   ├── GUI/
│   │   ├── TB303LookAndFeel.h/cpp # Silver metallic UI theme
│   │   ├── KnobComponent.h/cpp    # Rotary knob with APVTS binding
│   │   ├── TopPanel.h/cpp         # Knobs, drive, delay, volume
│   │   ├── MiddlePanel.h/cpp      # Sequencer controls, presets
│   │   ├── BottomPanel.h/cpp      # Keyboard, transport, steps
│   │   ├── LEDButton.h/cpp        # Button with red LED
│   │   ├── PianoKeyboard.h/cpp    # 1-octave piano keyboard
│   │   └── StepButton.h/cpp       # Step toggle with LED
│   └── Presets/
│       └── PresetManager.h/cpp    # Factory & user patch management
└── README.md
```

## Controls Reference

| Control | Function |
|---------|----------|
| WAVEFORM | Toggle between Sawtooth and Square |
| TUNING | Oscillator pitch offset (-12 to +12 semitones) |
| CUT OFF FREQ | Filter cutoff frequency (20Hz–20kHz) |
| RESONANCE | Filter resonance (0 to self-oscillation) |
| ENV MOD | Filter envelope modulation depth |
| DECAY | Filter envelope decay time (30ms–3s) |
| ACCENT | Accent intensity (filter + volume boost) |
| VOLUME | Master output volume |
| DRIVE TYPE | Distortion type: Soft / Hard / Tube |
| TONE | Post-drive tone control |
| DEPTH | Drive amount |
| DELAY TYPE | Delay type: Digital / Tape / Ping Pong |
| TIME | Delay time |
| LEVEL | Delay wet/dry mix |
| TEMPO SYNC | Sync delay time to host BPM |
| SHUFFLE | Swing amount on even steps |
| SCALE | Note division: 1/16T, 1/16, 1/32 |
| PLAY MODE | Forward / Reverse / FWD&REV / Invert / Random |
| RUN/STOP | Start/stop sequencer |
| ACCENT/SLIDE | Toggle per-step accent and slide in edit mode |
| DOWN/UP | Octave shift for keyboard |
| RANDOMIZE | Generate random pattern |

## License

This project is for educational purposes. TB-303 is a trademark of Roland Corporation.
