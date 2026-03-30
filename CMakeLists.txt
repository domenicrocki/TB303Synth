cmake_minimum_required(VERSION 3.22)
project(TB303Synth VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.9
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(TB303Synth
    COMPANY_NAME "TB303Synth"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE Tb3s
    PLUGIN_CODE Tb03
    FORMATS VST3 Standalone AU AUv3
    PRODUCT_NAME "TB-303"
    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(TB303Synth)

target_include_directories(TB303Synth PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

target_sources(TB303Synth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DSP/TB303Oscillator.cpp
    Source/DSP/TB303Filter.cpp
    Source/DSP/TB303Envelope.cpp
    Source/DSP/TB303Voice.cpp
    Source/DSP/DriveEffect.cpp
    Source/DSP/DelayEffect.cpp
    Source/Sequencer/StepSequencer.cpp
    Source/Sequencer/PatternData.cpp
    Source/GUI/TB303LookAndFeel.cpp
    Source/GUI/KnobComponent.cpp
    Source/GUI/TopPanel.cpp
    Source/GUI/MiddlePanel.cpp
    Source/GUI/BottomPanel.cpp
    Source/GUI/LEDButton.cpp
    Source/GUI/PianoKeyboard.cpp
    Source/GUI/StepButton.cpp
    Source/Presets/PresetManager.cpp
)

target_compile_definitions(TB303Synth PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(TB303Synth
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
