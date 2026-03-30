# TB-303 Synthesizer Plugin

A Roland TB-303 Bass Line synthesizer clone built as a **VST3 plugin** and **Standalone application** using the [JUCE](https://juce.com/) framework.

## Features

### Sound Engine
- **Polyblep oscillator** with Sawtooth and Square waveforms (alias-free)
- **Diode ladder filter model** — 4-pole 24dB/oct resonant lowpass filter with self-oscillation
- **Decay-only filter envelope** with near-instant attack (~3ms), variable decay (30ms–3s)
- **ENV MOD** — controls envelope depth to filter cutoff
- **Accent** — boosts filter envelope depth and output volume per step
- **Slide** — 60ms portamento glide between notes
- **Tuning** — oscillator detune control (-12 to +12 semitones)

### Effects
- **Drive** — Soft clip, Hard clip, and Tube overdrive with Tone and Depth controls
- **Delay** — Digital, Tape, and Ping Pong delay types with Time, Level, and Feedback
- **Tempo Sync** — snaps delay time to musical note divisions

### Step Sequencer
- 16-step pattern sequencer with per-step Note, Accent, Slide, and Active toggles
- 8 banks × 8 patterns = 64 pattern slots
- 5 play modes: Forward, Reverse, FWD&REV (ping-pong), Invert, Random
- Scale: 1/16, 1/16 Triplet, 1/32 note divisions
- Shuffle/swing control
- Pattern randomize with undo
- Host tempo sync (when used as VST3 in a DAW)

### Presets
- 10 factory patches (Acid Saw, Acid WideOD Saw, Squelch Square, Rubber Bass, etc.)
- Save/load patches and patterns (persisted in plugin state)
- MIDI input for live keyboard play

## Prerequisites

### Linux
```bash
# C++ compiler and build tools
sudo apt-get install -y build-essential cmake ninja-build

# JUCE dependencies
sudo apt-get install -y libasound2-dev libfreetype-dev libx11-dev \
  libxrandr-dev libxinerama-dev libxcursor-dev libgl-dev
```

### macOS
```bash
# Install Xcode command line tools
xcode-select --install

# Install CMake (via Homebrew)
brew install cmake ninja
```

### Windows
- Visual Studio 2019+ with C++ desktop development workload
- CMake 3.22+ (included with Visual Studio or install separately)

## Build

JUCE is downloaded automatically via CMake FetchContent — no manual installation needed.

```bash
# Configure
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build
```

Build artifacts are located at:
```
build/TB303Synth_artefacts/Release/
├── Standalone/
│   └── TB-303           # Standalone application
└── VST3/
    └── TB-303.vst3/     # VST3 plugin bundle
```

## Installation

### Standalone
Run directly:
```bash
./build/TB303Synth_artefacts/Release/Standalone/TB-303
```

### VST3 Plugin
Copy the VST3 bundle to your system's VST3 folder:

| OS      | VST3 Path                                    |
|---------|----------------------------------------------|
| Linux   | `~/.vst3/`                                   |
| macOS   | `~/Library/Audio/Plug-Ins/VST3/`             |
| Windows | `C:\Program Files\Common Files\VST3\`        |

```bash
# Linux example
cp -r build/TB303Synth_artefacts/Release/VST3/TB-303.vst3 ~/.vst3/
```

Then rescan plugins in your DAW (Bitwig, Reaper, Ableton, etc.).

## Project Structure

```
Source/
├── PluginProcessor.h/cpp       # Audio processor, APVTS parameters, state
├── PluginEditor.h/cpp          # Main GUI window
├── DSP/
│   ├── TB303Oscillator.h/cpp   # Polyblep saw/square oscillator
│   ├── TB303Filter.h/cpp       # Diode ladder lowpass filter
│   ├── TB303Envelope.h/cpp     # Decay-only envelope generator
│   ├── TB303Voice.h/cpp        # Voice combining osc + filter + env
│   ├── DriveEffect.h/cpp       # Overdrive/distortion
│   └── DelayEffect.h/cpp       # Delay with tempo sync
├── Sequencer/
│   ├── StepSequencer.h/cpp     # 16-step sequencer engine
│   └── PatternData.h/cpp       # Pattern/bank storage & serialization
├── GUI/
│   ├── TB303LookAndFeel.h/cpp  # Custom metallic UI theme
│   ├── KnobComponent.h/cpp     # Rotary knob with APVTS binding
│   ├── TopPanel.h/cpp          # Synth knobs, drive, delay, volume
│   ├── MiddlePanel.h/cpp       # Sequencer controls, preset lists
│   ├── BottomPanel.h/cpp       # Keyboard, transport, step buttons
│   ├── LEDButton.h/cpp         # Button with red LED indicator
│   ├── PianoKeyboard.h/cpp     # 1-octave clickable keyboard
│   └── StepButton.h/cpp        # Pattern step toggle with LED
└── Presets/
    └── PresetManager.h/cpp     # Factory & user patch management
```

## License

This project is for educational purposes. Roland and TB-303 are trademarks of Roland Corporation.
