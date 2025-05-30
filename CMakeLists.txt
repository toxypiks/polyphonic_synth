cmake_minimum_required(VERSION 3.16)
project(synthesizer_tryout)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake")

include_directories(include)

find_package(raylib)

find_package(Jack REQUIRED)

set(ENVELOP_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/envelop.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/envelop.h
  )

add_library(envelop)
target_sources(envelop
 PRIVATE
  ${ENVELOP_SOURCES}
  )

target_include_directories(envelop
 PRIVATE
 ${INCLUDE}
 )

set(OSCILLATOR_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/oscillator.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oscillator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/oscillator_func.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oscillator_func.h
  )

add_library(oscillator)
target_sources(oscillator
 PRIVATE
  ${OSCILLATOR_SOURCES}
  )

target_include_directories(oscillator
 PRIVATE
  ${raylib_INCLUDE_DIRS}
  )

target_link_libraries(oscillator
  ${raylib_LIBRARIES}
  m
  envelop
  )

set(RAY_OUT_BUFFER_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ray_out_buffer.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ray_out_buffer.h
  )

add_library(ray_out_buffer)
target_sources(ray_out_buffer
 PRIVATE
  ${RAY_OUT_BUFFER_SOURCES}
  )

target_include_directories(ray_out_buffer
 PRIVATE
 ${INCLUDE}
)

set(MIDI_MSG_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/midi_msg.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/midi_msg.h
  )

add_library(midi_msg)
target_sources(midi_msg
 PRIVATE
  ${MIDI_MSG_SOURCES}
  )

target_include_directories(midi_msg
 PRIVATE
 ${INCLUDE}
 )

set(JACK_STUFF_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jack_stuff.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jack_stuff.h
  )

add_library(jack_stuff)
target_sources(jack_stuff
 PRIVATE
  ${JACK_STUFF_SOURCES}
  )

target_include_directories(jack_stuff
 PRIVATE
 ${INCLUDE}
 )

target_link_libraries(jack_stuff
  ${JACK_LIBRARIES}
  )

set(ADSR_DISPLAY_HANDLER_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/adsr_display_handler.c
)
add_library(adsr_display_handler)
target_sources(adsr_display_handler
 PRIVATE
  ${ADSR_DISPLAY_HANDLER_SRC}
)
  
set(POLYPHONIC_SYNTH_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/polyphonic_synth.c
)

add_executable(polyphonic_synth)
target_sources(polyphonic_synth
 PRIVATE
  ${POLYPHONIC_SYNTH_SRC}
)

target_include_directories(polyphonic_synth
 PRIVATE
  ${raylib_INCLUDE_DIRS}
  ${JACK_INCLUDE_DIRS}
)

target_link_libraries(polyphonic_synth
  ${raylib_LIBRARIES}
  ${JACK_LIBRARIES}
  m
  oscillator
  ray_out_buffer
  jack_stuff
  ui_stuff
  ffmpeg_stuff
  synth_model
  thread_stuff
  model_gen_signal
  lf_queue
  adsr_display_handler
  )

set(UI_STUFF_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_stuff.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ui_stuff.h
  )

add_library(ui_stuff)
target_sources(ui_stuff
 PRIVATE
  ${UI_STUFF_SOURCES}
  )

target_include_directories(ui_stuff
 PRIVATE
 ${INCLUDE}
 )

target_link_libraries(ui_stuff
  ${RAYLIB}
  ray_out_buffer
  )

set(FFMPEG_STUFF_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg_stuff.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ffmpeg_stuff.h
  )

add_library(ffmpeg_stuff)
target_sources(ffmpeg_stuff
 PRIVATE
  ${FFMPEG_STUFF_SRC}
  )

target_include_directories(ffmpeg_stuff
 PRIVATE
 ${INCLUDE}
 )

target_link_libraries(ffmpeg_stuff
  ${RAYLIB}
  )

set(SYNTH_MODEL_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/synth_model.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/synth_model.h
  )

add_library(synth_model)
target_sources(synth_model
 PRIVATE
  ${SYNTH_MODEL_SRC}
  )

target_include_directories(synth_model
 PRIVATE
 ${INCLUDE}
 )

target_link_libraries(synth_model
  oscillator
  envelop
  )

set(THREAD_STUFF_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/thread_stuff.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/thread_stuff.h
  )

add_library(thread_stuff)
target_sources(thread_stuff
 PRIVATE
  ${THREAD_STUFF_SOURCES}
  )

target_include_directories(thread_stuff
 PRIVATE
 ${INCLUDE}
 )

target_link_libraries(thread_stuff
  ${JACK_LIBRARIES}
  )

set(LF_QUEUE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lf_queue.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lf_misc.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/lf_queue.h
  )

add_library(lf_queue)
target_sources(lf_queue
 PRIVATE
  ${LF_QUEUE_SOURCES}
  )

target_include_directories(lf_queue
 PRIVATE
 ${INCLUDE}
 )

 # msh_handler library
 set(MSG_HANDLER_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/msg_handler.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/msg_handler.h
)

add_library(msg_handler)
target_sources(msg_handler
 PRIVATE
  ${MSG_HANDLER_SOURCES}
)

target_include_directories(msg_handler
 PRIVATE
 ${INCLUDE}
)

# model_gen_signal library
set(MODEL_GEN_SIGNAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/model_gen_signal.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tone_handler.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/model_gen_signal.h
)

add_library(model_gen_signal)
target_sources(model_gen_signal
 PRIVATE
 ${MODEL_GEN_SIGNAL_SOURCES}
)

target_include_directories(model_gen_signal
 PRIVATE
 ${INCLUDE}
)

target_link_libraries(model_gen_signal
  synth_model
  msg_handler
  midi_msg
  m
)

set(HASH_MAP_TEST_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/test_scripts/hash_map_test.c
  )

add_executable(hash_map_test)
target_sources(hash_map_test
 PRIVATE
  ${HASH_MAP_TEST_SRC}
  )
