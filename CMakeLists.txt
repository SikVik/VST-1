
cmake_minimum_required(VERSION 3.20)

project(RadioSauceSynth VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch JUCE
include(FetchContent)
FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 7.0.12
)
FetchContent_MakeAvailable(JUCE)

add_subdirectory(Source)

# ---- Plugin target ----
juce_add_plugin(RadioSauceSynth
    COMPANY_NAME "VicTheMonster"
    COMPANY_WEBSITE "https://example.com"
    PLUGIN_NAME "RadioSauceSynth"
    BUNDLE_ID com.victhemonster.radiosaucesynth
    ICON_BIG ""  # Add path to .png if you have one
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE VTMS
    PLUGIN_CODE RSS1
    FORMATS VST3
    PRODUCT_NAME "RadioSauceSynth")

target_sources(RadioSauceSynth PRIVATE
    ${PROJECT_SOURCE_DIR}/Source/PluginProcessor.cpp
    ${PROJECT_SOURCE_DIR}/Source/PluginEditor.cpp
    ${PROJECT_SOURCE_DIR}/Source/SynthVoice.cpp
    ${PROJECT_SOURCE_DIR}/Source/SynthSound.cpp
    ${PROJECT_SOURCE_DIR}/Source/Wavetable.cpp
    ${PROJECT_SOURCE_DIR}/Source/FXChain.cpp
)

target_include_directories(RadioSauceSynth PRIVATE ${PROJECT_SOURCE_DIR}/Source)

target_compile_definitions(RadioSauceSynth
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(RadioSauceSynth PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)
