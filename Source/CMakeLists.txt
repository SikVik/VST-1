
add_library(RadioSauceSynthLib INTERFACE)

target_sources(RadioSauceSynthLib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginProcessor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginEditor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SynthVoice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SynthSound.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Wavetable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ParameterIDs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/FXChain.h
)
